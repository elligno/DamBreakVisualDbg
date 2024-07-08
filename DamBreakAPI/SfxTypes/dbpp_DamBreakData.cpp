// C++ includes
#include <cassert>
#include <iostream>
// STL include
#include <algorithm>
// Boost include
#include <boost/cast.hpp>
// Package includes
#include "dbpp_DamBreakData.h"
#include "../Utility/dbpp_EMcNeilUtils.h"

namespace dbpp 
{
	DamBreakData::DamBreakData( DamBreakData::DiscrTypes aDiscrType/*=DiscrTypes::hudson*/)
	: m_discrType(aDiscrType)
	{
		discrInit();
	}

	DamBreakData::DamBreakData( int32 aNx, const std::pair<float64,float64> aVal /*= default value of the domain extent  std::make_pair(0., 1.)*/)
	{
		// 			m_domainExtent=aVal;            // spatial domain extent
		// 			m_NumOfGridPts=aNx;             // number of grid nodes                   
		// 			m_DegOfFreedom=4;               // variables attached to each node
		// 			m_Phi0 = 1.;                    // water level at upstream
		// 			m_Phi1 = 0.5;                   // water level at downstream
		// 			m_dambreakExtent.first=m_Phi0;  // dam-break water level at downstream
		// 			m_dambreakExtent.second=m_Phi1; // dam-break water level at upstream
		// 			m_Xmin=m_domainExtent.first;    // x-coord of the first grid node
		// 			m_Xmax=m_domainExtent.second;   // x-coord of the last grid node
		// 			m_dX=  // spatial step
		// 				(m_Xmax-m_Xmin)/boost::numeric_cast<float64>(m_NumOfGridPts-1);
		// 			assert(m_dX!=0.f);              // debugging 
		// 			assert(m_Xmin < m_Xmax);
		// 			assert(m_Phi0 > m_Phi1);

	}
	void DamBreakData::discrInit()
	{
		switch( m_discrType)
		{
		case dbpp::DamBreakData::DiscrTypes::emcneil:
			{
				// configure the simulation property. 
				m_S0am=0.;                      // ...
				m_S0av=0.;                      // ...
				m_Phi1 = 10.;                   // water level at upstream
				m_Phi0 = 1.;                    // water level at downstream
				m_cfl  = 0.6;                   //
				m_sectWidth=1.;                 //
				m_NbSects = 101;               // number of sections
				m_Xmin = 0.;                    // x-coordinate of the left bound. node
				m_Xmax = 1000.;                 // x-coordinate of the right bound. node
				m_domainExtent.first=m_Xmin;    // ...
				m_domainExtent.second=m_Xmax;   // ...
				m_dambreakExtent.first=m_Phi0;  // dam-break water level at downstream
				m_dambreakExtent.second=m_Phi1; // dam-break water level at upstream
				m_unitSectWidth=true;           //
				m_frictionLess=true;
				m_flatBed=true;
				assert(m_Xmin < m_Xmax);
				// spatial step
				m_dX=
					(m_Xmax-m_Xmin)/boost::numeric_cast<float64>( m_NbSects-1);
				assert(m_Phi0<m_Phi1);
				assert(m_dX!=0.);
				m_resultFilename="DamBreakResult.txt";
				// set initial cnd
				setIC();
			}
			break;
		case dbpp::DamBreakData::DiscrTypes::hudson:
			{
				// configure the simulation property. 
				//m_DegOfFreedom = 3;               (A,Q,H) and X as coordinate
				m_S0am=0.;                      // ...
				m_S0av=0.;                      // ...
				m_Phi1 = 1.;                     // water level at upstream
				m_Phi0 = 0.5;                    // water level at downstream
				m_cfl=0.6;
				m_NbSects = 101;                 // number of sections
				m_Xmin = 0.;                     // x-coordinate of the left bound. node
				m_Xmax = 1.;                     // x-coordinate of the right bound. node
				m_domainExtent.first=m_Xmin;     // ...
				m_domainExtent.second=m_Xmax;    // ...
				m_dambreakExtent.first=m_Phi0;   // dam-break water level at downstream
				m_dambreakExtent.second=m_Phi1;  // dam-break water level at upstream
				m_unitSectWidth=true;            // 
				m_frictionLess=true;
				m_flatBed=true;
				m_sectWidth=1.;
				assert(m_Xmin < m_Xmax);
				// spatial step
				m_dX=
					(m_Xmax-m_Xmin)/boost::numeric_cast<float64>( m_NbSects-1);
				assert(m_Phi0<m_Phi1);
				assert(m_dX!=0.);
				// set initial cnd
				setIC();
				/*break;*/
			}
			break;
		default:
			std::cout << "Discretization not supported yet\n";
			break;
		}
	}

	void DamBreakData::setIC()
	{
		// discretization concept
		unsigned j=0;   // initialize
		double w_dx=m_dX; // not sure if i really need it
		m_X.resize(m_NbSects);

		std::for_each( m_X.begin(), m_X.end(), // range
			[w_dx,&j]( double& aVal)       // lambda (anonymous function)
		{
			try // make sure cast succeed
			{
				aVal = w_dx*boost::numeric_cast<double>(j++);
			}
			catch( boost::bad_numeric_cast& e) 
			{
				std::cerr << e.what() << std::endl;
			}
		} // lambda
		); //for_each

		// did i initialize the N and Z vector?
		m_IC.m_N.resize(m_NbSects);
		m_IC.m_Z.resize(m_NbSects);

		// setting bathymetry and Manning coefficient
		std::fill( m_IC.m_N.begin(),m_IC.m_N.end(),0.);
		std::fill( m_IC.m_Z.begin(),m_IC.m_Z.end(),0.);
		
		// some parameters used in B.C.
		//m_S0am = m_S0av = 0.;

		// ...
		m_IC.m_U1.resize(m_NbSects);
		m_IC.m_U2.resize(m_NbSects);
		m_IC.m_H.resize(m_NbSects);

		// use the DamBreak Step function to initialize 
		// combined with ... to be completed
		for( int32 j = 0; j < m_NbSects; j++)
		{
			if (m_X[j] < x_max()/2. /*500.*/)
				m_IC.m_H[j] = m_Phi1; //10.;
			else
				m_IC.m_H[j] = m_Phi0; //1.;

			/*m_IC.m_U2[j] = 0.;*/
		}
		
		// discharge set 0
		std::fill(m_IC.m_U2.begin(),m_IC.m_U2.end(),0.);
		
		// wetted section area
		for( int32 j = 0; j < m_NbSects; j++)
			m_IC.m_U1[j] = 
			HydroUtils::Evaluation_A_fonction_H( m_IC.m_H[j], m_IC.m_Z[j], getWidth());
	}
} // End of namespace

