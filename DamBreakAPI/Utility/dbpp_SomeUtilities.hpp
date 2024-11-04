#pragma once

// C++ include
#include <functional>
#include <vector>
// boost include
#include <boost/filesystem.hpp> // boost file system utility
// Package includes
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "dbpp_EMcNeilUtils.h"
// src package include
#include "../Discretization/dbpp_NodalVariables.h"
// Base numeric type library
#include "../SfxTypes/dbpp_RealNumArray.h"

namespace dbpp {
typedef double float64;

//
// Manning formula to evaluate friction
//
// Manning formula (friction law) check this formula
//   Sf1 = g*n1*n1*V1*::fabs(V1)/::pow(R1, 4./3.);
//
//   Usage:
//
class ManningFormula //:
                     // public std::unary_function<Nodal_Value,float64>
{
public:
  ManningFormula(float64 aFricCoeff) : m_N(aFricCoeff) {}
  float64 operator()(const Nodal_Value aNval) const {
    // Hydraulic radius (unit width).
    // HydroUtils::R w_R;
    float64 Rh = HydroUtils::R(aNval[0], 1.); // A

    // define some for clarity
    const float64 gANN = PhysicalConstant::sGravity * aNval[0] * m_N * m_N;
    const float64 V = aNval[1] / aNval[0]; // velocity (Q/A)
    const float64 Vsquaredivh = V * ::fabs(V) / ::pow(Rh, 4. / 3.);

    return gANN * Vsquaredivh;
  }

private:
  float64 m_N; // friction coefficient
};

//
//  setNodalVariables (field to Nodal Values)
//
//  IMPORTANT:
//    In the current project, the scalar field hold 100 pts
//    global discretization hold 101 pts for the extra node.
//    Usage:
//      To update the global discretization nodal variables
//      at the end of each iteration. For example can use stl
//      for_each(begin(),end(),setNodalValues(m_u.first,m_u.second));
#if 0
    class setNodalValues :
		public std::unary_function<Nodal_Value,void>
	{
	public:
		setNodalValues( double* aPtr2D, unsigned aOffset) : m_ptr(aPtr2D) {}
		setNodalValues( const dbpp::RealNumArray<float64>& aU1,
			const dbpp::RealNumArray<float64>& aU2)
		{
#ifdef _DEBUG
			BOOST_ASSERT(aU1.size()==
				aU2.size() && "Field with different size" );
#endif
			m_vec1.reserve(aU1.size());
			m_vec1.assign(aU1.getPtr(),aU1.getPtr()+aU1.size());
			m_vec2.reserve(aU2.size());
			m_vec2.assign(aU2.getPtr(),aU2.getPtr()+aU2.size());
		}
		// H will be updated out side of this function, there 
		// is some helper function ... to be completed
		result_type operator() ( argument_type& aArg) const
		{
// 			aArg[0]=*m_beg1++; // A
// 			aArg[1]=*m_beg2++; // Q

			// flat bathymetry z=0.
			//	aArg[2]=hyd::Evaluation_H_fonction_A(aArg[0],0.);    H
// 			m_beg1++;
// 			m_beg2++;
		}
	private:
		mutable double* m_ptr;
		mutable std::vector<double> m_vec1;
		mutable std::vector<double> m_vec2;
	};
#endif // 0

// Just a function that initialize a nodal value
// For example it could be the step function for
// the dam-break problem.
//   Usage
// 	class DamBreakStepFunc :
// 	 		public std::unary_function< Nodal_Value, void>
// 	{
// 	 	public:
// 		// default ctor
// 		DamBreakStepFunc( DamBreakData aData)
// 		: m_Discr( aData) // ...
// 		{}
//
// 		// to be used with the for_each algorithm
// 		result_type operator() ( argument_type& aNval) const
// 		{
// 			// retrieve the node attributes (i,x)
// 			Nodal_Value::nodeIdX w_idx = aNval.getIdX();
// 			aNval[2] = m_Discr.getPhi0();           // H
// 			if( w_idx.second < m_Discr.x_max()/2.) // breaking point
// 				aNval[2] = m_Discr.getPhi1();        // H
// 			aNval[1] = 0.;                          // Q
// 			// unit width section
// 			aNval[0]=aNval[2];
// 		}
// 	private:
// 		// discretization (shall we use the shared pointer)
// 		DamBreakData m_Discr;
//	 	};

/**
 * Functor to retrieve various element of the Nodal_Value structure
 * Usage:
 *   To be used with
 */
class GetA {
public:
  using result_type = float64;
  using argument_type = Nodal_Value;

  // default ctor
  GetA() = default;
  result_type operator()(const argument_type &aVar) const {
    return aVar[0]; // A
  }
};

/**
 * Functor to retrieve various element of the Nodal_Value structure
 * Usage:
 *   To be used with
 */
class GetQ {
public:
  using result_type = float64;
  using argument_type = Nodal_Value;
  // default ctor
  GetQ() = default;
  result_type operator()(const argument_type &aVar) const {
    return aVar[1]; // Q
  }
};

/** Retrieve water level component from ...
 */
class GetH {
public:
  using result_type = float64;
  using argument_type = Nodal_Value;
  // default ctor
  GetH() = default;
  result_type operator()(const argument_type &aVar) const {
    try {
      // throw an exception
      // 				Nodal_Value& w_Nvar =
      // 					dynamic_cast<Nodal_Value&>(aVar);
      return aVar[2]; // Q
    } catch (std::bad_cast &bc) {
      std::cerr << "Bad cast caught: " << bc.what() << "\n";
      return -1.; // debugging purpose (J.B. 20 fev. 2012)
    }
  }
};

#if 0
  /** Recursively find the location of a file on a given directory
  * make use of some of the c++11 features : auto keyword and lambda function
  */
  bool FindFile11(const boost::filesystem::path& directory,
    boost::filesystem::path& path,
    const std::string& filename)
  {
    bool found = false;

    const boost::filesystem::path file = filename;
    const boost::filesystem::recursive_directory_iterator end;
    const boost::filesystem::recursive_directory_iterator dir_iter(directory);

    // use auto and lambda function &file input argument of the lambda
    const auto it = std::find_if(dir_iter,
      end,
      [&file](const boost::filesystem::directory_entry& e)
    {
      return e.path().filename() == file;
    });

    if (it != end)
    {
      path = it->path();
      found = true;
    }

    return found;
  }
#endif

#if 0
	/** Update state variable for each section flow. 
	  * create subrange because value at both end are fixed by some boundary condition
	  * we make use of the range library to accomplish this task, ... to be completed
	  */
	void updateH()
	{
		// NOTE this function is used? i really not sure!!
		using namespace std;
		using namespace boost;
		using namespace dbpp;
		using namespace std::placeholders;

		// Nov. 19
		// Debugging purpose (not in use for now but could be in the future)
		vector<double> U1;
		vector<double> Z;
		vector<double> H; // debugging purpose

		// use range (no copy semantic) and model of forward iterator 
		// exactly what we need for the range j=1 to j=99 excluding 
		// BC nodes(j=0 and j=100) at both ends of the mesh.
		iterator_range<vector<double>::const_iterator> w_rngU1(U1);
		// ]begin,end[
		w_rngU1.advance_begin(1);
		w_rngU1.advance_end(-1);

		iterator_range<vector<double>::const_iterator> w_rngZ(Z);
		// ]begin,end[
		w_rngZ.advance_begin(1);
		w_rngZ.advance_end(-1);

		// by doing this we are modifying the H values (don't need that anymore)
		iterator_range<vector<double>::iterator> w_tmpH = // ok i understand, 
			make_iterator_range( H.begin()+1, H.end()-1); 

		// What we should it's to take the List of section flow and
		// go through the list to search if there is a section with 
		// width not unity
		// downcast (convert base to derived) need an explicit cast 
		ListSectFlow* w_listSectgF = nullptr; // debugging purpose
			//static_cast<ListSectFlow*>( m_listofObs.front()); // IObserver cast to ListSectionFlow

		// returns true if predicate returns true for all element 
		if( all_of( w_listSectgF->begin(), w_listSectgF->end(), // range of section flow
			mem_fn( &SectFlow::isUnitWidth)) == true) // check if section is unit width
		{
			std::cout << "All section flow are of unit width, then we set width to unity\n"; 

			// C++11 new algorithm (we are checking if at least one section flow)
			// is not unit width
			const double w_sectionUnitrWidth = 1.;

			// H=A-Z (water depth of each section flow)
			transform( w_rngU1.begin(), w_rngU1.end(), w_rngZ.begin(), // A,Z range 
				make_iterator_range( H.begin()+1, H.end()-1).begin(), /*w_tmpH.begin(),*/
				bind( &HydroUtils::Evaluation_H_fonction_A, _1, w_sectionUnitrWidth, _2)); // algorithm
		}
		else
		{
			const double w_sectionWidth = 2.;
			transform( w_rngU1.begin(), w_rngU1.end(), w_rngZ.begin(), 
				make_iterator_range( H.begin()+1, H.end()-1).begin(), /*w_tmpH.begin(),*/
				bind( &HydroUtils::Evaluation_H_fonction_A, _1, w_sectionWidth, _2));
		}
	}
#endif
} // namespace dbpp
