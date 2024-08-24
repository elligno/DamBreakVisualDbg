
// stl includes
#include <memory>   // smart pointer
#include <cassert>
#include <iostream>
// STL numeric library
#include <numeric>
//#include "dbapp_EmcilNumTreatment.h"
#include "dbpp_HLL_RK2_Algo_Vec.h"
// src package includes
// #include "../src/dbapp_ListSectFlow.h"
#include "../Discretization/dbpp_GlobalDiscretization.h"
// #include "../Test/ReconstrUtility.h"
#include "../SfxTypes/dbpp_Simulation.h"
// Utility package includes
#include "../Utility/dbpp_TestLogger.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
#include "../Utility/dbpp_EMcNeilUtils.h"

namespace dbpp 
{
	// default implementation with E. MCNeil parameters
	EMcNeil1D::EMcNeil1D()
	{
		// set pointer to numerical representation (Observer Design Pattern)
		dbpp::GlobalDiscretization::instance()->num_rep() = this;
	}

	// dtor NO!!
	EMcNeil1D::~EMcNeil1D()
	{
	}

	//	Exécution de la phase intermédiaire de calcul:
	//	schéma d'intégration Runge-Kutta (Nujic, 1995)
	//
	//	Au cours de cette phase, une valeur intermédiaire est déterminée
	//	comme suit:
	//
	//	Up = U - dt/dx * (F[j+1/2] - F[j-1/2]) - dt/dx * (P) -dt*S
	// ...
	// stepping through time (pair of smart pointer)
	// NOTE: in this version (single iteration, we have reproduced the result).
	// But there is a problem, by using the std::vector and assigning values,
	// work on copy (vector own it's element), we make change on vector element
	// but this doesn't affect the StateVector element value, even if we pass it
	// as reference and use reference on RealNumArray, because vector takes the
	// ownership of the values. What we have to do, copy vector into RealNumArray. 
	// Or use vector of pointer (pointer vector, std::vector<double*> or boost::ptr_vector)  
	void EMcNeil1D::advance( /*StateVector& aU*/)
	{
		// set state variables for this iteration
//		m_U12 = aU;
		
		// EMcNeil1DSettings will be configured by the user and will provide
		// the initial profile of the initial cnd and then the solver hold the
		// state and update it at each time step. We need the list of section
		// (H values) that we are interested for. Solver needs IC 
		//setInitSln(aU); deprecated
		
		// don't real need it, this is something that will be removed 
		// in future version. H is used by the numerical algorithm and
		// will be handled by the ListSectionFlow.
		//setH(); 

		// lines above replaced by this line
		//dbapp::DbgLogger::instance()->write2file( std::string(""),++NbIterations);

		// compute and set B.C. and initialize temporary variable 
		//setTmpAndBC();  deprecated (add the ghost node to U1,U2,H -> 101)

		// advance the physics algorithm (overided by subclass) 
		// Template Design Pattern 
		timeStep();
	}

#if 0 // deprecated stuff
	// IMPORTANT: 
	// ----------
	// we are using the characteristic method to solve st-venant for 
	// boundary condition upstream/downstream (this method is explained
	// in the following document from springer.com).
	// Reference: Modeling Of The Open Channel Flow (chapter #2)
	// google it with "an introduction to st-venant one-dimensional model"
	// Reminder
	//   Curve in the x/t plane (phase space) ... to be completed 
	//   Technical report ....
	//   Another, we are considering the supercritical flow (Fr>1)
	//   it will be nice if we could measure during the simulation 
	//   the Froude number. 
	//
	//  DESIGN NOTE: this method setAmont() and setAval() need to be re-designed
	//  for now i just leave it like this, because it's an implementation taht we 
	//  want to experiment. Anyway all the boundary condition will be handled by 
	//  only one class not by many as we do in this current version of the simulator.
	void EMcNeil1D::setAmont()
	{
		double n1, V0, V1, R1, Y1, Sf1/*, S0am, S0av*/;
		double /*CA,*/ CB, CN/* CP*/;

		// debugging purpose
		const double S0av=0.;

		// Retrieve the water level from global discretization since we are not 
		// using the variable H as a member of the base class
		GlobalDiscretization::instance()->to_stdVector(H,GlobalDiscretization::NodalValComp::H);
		// sanity check
		assert(H.size() == NbSections);

		//	Amont, spécification de l'élévation de la surface libre
		H[0] = 10.; // water level
		U1[0] = HydroUtils::A( B, H[0]-Z[0]); // wetted area

		//	Calcul des caractéristiques à la section située directement en aval
		n1 = n[1]; // manning
		V1 = U2[1]/U1[1]; // velocity
		R1 = HydroUtils::R(U1[1], B); // hydraulic radius
		Y1 = H[1] - Z[1]; // water depth
		
		// because we are considering a rectangular channel
		// see reference (Modeling Of Open Channel Flow, chapter #2)
		const double w_grav=PhysicalConstant::sGravity;
		CB = ::sqrt( w_grav*U1[1]/HydroUtils::T(B));

		// Manning formula (see documentation)
		Sf1 = n1*n1*V1*::fabs(V1)/::pow(R1,4./3.);
		
		// time step at ... 
		const double dt = dbapp::Simulation::instance()->simulationTimeStep();

		// see again documentation (characteristics equation solution
		// i do not understand yet the *Y1, probably a unit  
		CN = V1 - CB*Y1 + w_grav*(S0av-Sf1)*dt; // negative C characteristic (subcritical regime) 
		// i do not understand this line (i don't get it)
		V0 = CN + CB*(H[0]-Z[0]); // velocity at upstream along characteristic
		
		// set discharge Q at boundary node 
		U2[0] = V0*U1[0]; // discharge[m3/s]=velocity*surface

		printf ("EMcNeil1D implementation\n");
		printf( "Condition amont: A: %lf  Q: %lf\n", U1[0], U2[0]);
		dbapp::Logger::instance()->OutputSuccess("Condition amont: A: %lf  Q: %lf", U1[0], U2[0]);
	}

	// see comment above for the setAmont()
	// IMPORTANT: 
	//   we need to add an element to our vector No, because we allocated
	//   memory for it, so we do not need to do anything
	void EMcNeil1D::setAval()
	{
		double n1, /*V0,*/ V1, R1, Y1, Sf1/*, S0am, S0av*/;
		double CA, /*CB, CN,*/ CP;

		// debugging purpose
		const double S0am=0.;

		// Retrieve the water level from global discretization since we are not 
		// using the variable H as a member of the base class
		GlobalDiscretization::instance()->to_stdVector(H,GlobalDiscretization::NodalValComp::H);
		// sanity check
		assert(H.size() == NbSections);

		//	Aval, spécification du débit
	//	U2[NbSections-1] = 0.; // because we reserve and set i=0,...,99
		U2.push_back(0.);      // need to add i=100

		n1 = n[NbSections-2];
		V1 = U2[NbSections-2]/U1[NbSections-2];
		R1 = HydroUtils::R (U1[NbSections-2], B);
		Y1 = H[NbSections-2] - Z[NbSections-2];

		const double w_grav=PhysicalConstant::sGravity;
		CA = ::sqrt (w_grav*U1[NbSections-2]/HydroUtils::T(B));
		Sf1 = n1*n1*V1*::fabs(V1)/::pow(R1,4./3.);
		
		// time step at ... 
		const double dt = dbapp::Simulation::instance()->simulationTimeStep();
		CP = CA*Y1 + w_grav*(S0am-Sf1)*dt + V1;

		// because we reserve i=0,...,99 need to add i=100
 		H[NbSections-1] = (CP/CA) + Z[NbSections-1]; // since we resize at startup (AllocateMem) 
// 		U1[NbSections-1] = HydroUtils::Evaluation_A_fonction_H( H[NbSections-1], Z[NbSections-1], B);
//		H.push_back(( CP/CA) + Z[NbSections-1]);
		U1.push_back( HydroUtils::Evaluation_A_fonction_H( H[NbSections-1], Z[NbSections-1], B));

		printf( "Condition aval: A: %lf  Q: %lf\n", U1[NbSections-1], U2[NbSections-1]);
		dbapp::Logger::instance()->OutputSuccess("Condition aval: A: %lf  Q: %lf", U1[NbSections-1], U2[NbSections-1]);
	}
#endif // bc impl

  // do we really need it?
// 	void EMcNeil1D::clearField()
// 	{
// 		U1.clear();
// 		U2.clear();
		// in the original version we used the reserve()
		// in this new version we now resize(), don't need
		// to clear since it set the size to zero, it's not what we want
		//H.clear();
		
		// do i really need to do it, or just re-initialize it!!
		// when i clear, i just empty the container. Then in the 
		// next iteration (size=0) i call fill again to set vector,
		// but i think fill doesn't push element in the array 
// 		U1p.clear();  make sure that it's clear, size=0, no more
// 		U2p.clear();  element capacity is still at NbSections
//	}

#if 0
	// want to check values of global discretization
	// with state variable (U1,U2,H) for future impl.
	// of global discretization concept (temp. fix)
	void EMcNeil1D::checkGdiscrWithVec()
	{
		// retrieve values from global discretization
		boost::ptr_vector<Nodal_Value>& w_aqh = GlobalDiscretization::instance()->Uh();
		using dist_type = std::iterator_traits<boost::ptr_vector<Nodal_Value>::iterator>::difference_type;
		
		// retrieve A values from nodal_value
		auto beg_iterA = boost::make_transform_iterator( w_aqh.begin(), dbpp::GetA());
		auto end_iterA = boost::make_transform_iterator( w_aqh.end(),   dbpp::GetA());

		// retrieve Q values from nodal_value
		auto beg_iterQ = boost::make_transform_iterator( w_aqh.begin(), dbpp::GetQ());
		auto end_iterQ = boost::make_transform_iterator( w_aqh.end(),   dbpp::GetQ());

		// retrieve H values from nodal_value
		auto beg_iterH = boost::make_transform_iterator( w_aqh.begin(), dbpp::GetH());
		auto end_iterH = boost::make_transform_iterator( w_aqh.end(),   dbpp::GetH());
		
		// distance between begin and end iterator (size)
		dist_type w_Adist=std::distance( beg_iterA, end_iterA);
		dist_type w_Qdist=std::distance( beg_iterQ, end_iterQ);
		dist_type w_Hdist=std::distance( beg_iterH, end_iterH);

		// sanity check for debugging purpose
		assert( w_Adist == U1.size());
		assert( w_Qdist == U2.size());
		assert( w_Hdist == H.size());

		// equal range check (U1)
		if( !std::equal( beg_iterA, end_iterA, U1.begin()))
		{
			std::cerr << "Global Discretization different from 'A' state variables\n";
			dbpp::Logger::instance()->OutputError("Global Discretization different from 'A' state variables");

			// debugging purpose stuff
			typedef std::vector<double>::iterator vec_iter;
			typedef boost::ptr_vector<Nodal_Value>::iterator ptr_vec_iter;
			typedef boost::transform_iterator<dbpp::GetA, ptr_vec_iter> transf_iter;
			std::pair<transf_iter,vec_iter> w_noMatch = std::mismatch( beg_iterA, end_iterA, U1.begin());

			// REMARK 
			//  We found the bug, located in the initialization of the global discretization. We use
			//  the DiscrInfo type which is deprecated and replaced by the new type DamBreakData
			
			// print value (need to dereference because we have a pair of iterator)
			std::cout << "Following difference found: " << *w_noMatch.first << " and " << *w_noMatch.second << "\n"; 
			dist_type w_diffDist = std::distance( beg_iterA, w_noMatch.first); // checking iterator

			std::cout << "Found difference at location: " << w_diffDist << "\n";
			dbpp::Logger::instance()->OutputError( "Found difference at location: %d", w_diffDist);
			dbpp::Logger::instance()->OutputError( "Following difference found : %lf %lf", *w_noMatch.first, *w_noMatch.second);

// 			std::cout << "Check some values at upstream to validate: " << *(beg_iterA+50) << "\n";
// 			std::cout << " Just finished checking\n"; 
		}

    // equal range check (U2)
		if( !std::equal( beg_iterQ, end_iterQ, U2.begin()))
		{
			std::cerr << "Global Discretization different from 'Q' state variables\n";
			dbpp::Logger::instance()->OutputError("Global Discretization different from 'Q' state variables\n");
			
			// debugging purpose stuff
			typedef std::vector<double>::iterator vec_iter;
			typedef boost::ptr_vector<Nodal_Value>::iterator ptr_vec_iter;
			typedef boost::transform_iterator<dbpp::GetQ, ptr_vec_iter> transf_iter;
			std::pair<transf_iter,vec_iter> w_noMatch = std::mismatch( beg_iterQ, end_iterQ, U2.begin());

			// print value (need to dereference because we have a pair of iterator)
			std::cout << "Following difference found: " << *w_noMatch.first << " and " << *w_noMatch.second << "\n"; 
			
			dist_type w_diffDist = std::distance( beg_iterQ, w_noMatch.first); // checking iterator
			dbpp::Logger::instance()->OutputError( "Found difference at location: %d", w_diffDist);
			dbpp::Logger::instance()->OutputError( "Following difference found : %lf %lf", *w_noMatch.first, *w_noMatch.second);
		}

		if( !std::equal( beg_iterH, end_iterH, H.begin()))
		{
			std::cerr << "Global Discretization different from 'H' state variables\n";
			dbpp::Logger::instance()->OutputError("Global Discretization different from 'H' state variables\n");
		}
	}
#endif

	// setting the nodal_value structure according to vector or state variable
	// computed with the given algorithm (...).
// 	void EMcNeil1D::setGobalDiscrFromVec()
// 	{
//		throw std::exception("The method or operation is not implemented.");
		
		// retrieve the nodal values of the global discretization
// 		boost::ptr_vector<Nodal_Value>& w_aqh = GlobalDiscretization::instance()->Uh();
// 		unsigned i=0; // index var
// 		for( Nodal_Value& w_nval : w_aqh) // just copying values from a container to another 
// 		{
// 			w_nval[0]=U1[i];
// 			w_nval[1]=U2[i];
// 			w_nval[2]=H[i++];
// 		}
// 	}

// 	void EMcNeil1D::setInitSln( StateVector &aU)
// 	{
// 		// we need the state vector (we hold reference computational
// 		// values, instead of referencing to some values)
// 		const jb::RealNumArray<real>& w_U1 = aU.first->values();  // (A)
// 		const jb::RealNumArray<real>& w_U2 = aU.second->values(); // (Q)

		// NOTE: U1 and U2 make a copy of the data provided by StateVector
		// I think that's what we are doing? By changing U1 and U2 we work
		// on the copy, it won't affect the StateVector value and that's why 
		// when we compute dt (in the simulator) we end up with same value as 
		// initial setting (because field value hasn't change). Work on copy!!! 
		// for the rest we work with vector (U1,U2,H).
// 		U1.assign( w_U1.getPtr(), w_U1.getPtr() + w_U1.size());
// 		U2.assign( w_U2.getPtr(), w_U2.getPtr() + w_U2.size());
// 	}

	// deprecated just don't understand
// 	void EMcNeil1D::setState( jb::RealNumArray<real> &aH)
// 	{
// 		using namespace std::placeholders;
// 
// 		// ...
// 		jb::RealNumArray<real> &w_U1 = m_U12.first->values();
// 		jb::RealNumArray<real> &w_U2 = m_U12.second->values();
// 
// 		// +++++++++++++++++++temporary fix+++++++++++++++++
// 		// A values (need to copy ...) end-1 because field is carrying out 
// 		// only computational values or grid (not ghost node), for now not 
// 		// much of a choice.
// 		std::copy( std::begin(U1), std::end(U1)-1, w_U1.getPtr());
// 		std::copy( std::begin(U2), std::end(U2)-1, w_U2.getPtr());
// 		std::copy( std::begin(H),  std::end(H)-1,  aH.getPtr());
// 	}

	void EMcNeil1D::setState()
	{
		// all copy that we are doing at the end of advance(...)
		// then call notify to all attached observer and update()
		// for global discretization 
    		
		using namespace std::placeholders;

    // notify section flow (notify all observers)
		notify(); // call observer update() method

		// update the global discretization
		dbpp::GlobalDiscretization::instance()->update();

    // update boundary condition (need to check for this one)
    dbpp::GlobalDiscretization::instance()->gamma().applyBC();

#if 0 //maybe in the next version??
    // Current version these values are set inside applyBC()
    // which doesn't make sense, why this method set b.c. to global?
    // Also, shall pass Uh as argument (for now ok) but keep in mind
    // the final version will take it as an arg GblDiscr{Uh,Omega,Gamma}
    // the the following: Gamma w_bcEMcNeil{Uh} hold a reference 
    // when GblDiscr is updated, Gamma get its value up-to-date, since
    // it use reference semantic ('Uh' ptr-to-Object).
    auto w_gDiscr = dbpp::GlobalDiscretization::instance();
    auto& w_frontVal = w_gDiscr->Uh().front();
    w_frontVal[0] = 1.3;
    auto& w_backVal = w_gDiscr->Uh().back();
    w_backVal[0] = 0.3;
#endif
	}

  void EMcNeil1D::attach(Observer* aObs)
  {
    m_listofObs.push_back(aObs);
  }

  void EMcNeil1D::detach(Observer* aObs)
  {
    // stl find algorithm return an iterator and then delete it
    if ( std::find(m_listofObs.begin(),m_listofObs.end(),aObs) != m_listofObs.end())
    {
      // return iterator at position if found it (dereference it to get the object)
      std::list<Observer*>::iterator w_found =
        std::find(m_listofObs.begin(),m_listofObs.end(),aObs);

      // we found something, must delete it
      delete *w_found;
      *w_found=nullptr;
    }
  }

	void EMcNeil1D::notify()
	{
		// in this version we have only one observer
		// no need to loop through the list
		assert(1 == m_listofObs.size());
		Observer* w_obs = m_listofObs.front();
		// sanity check
		assert( w_obs!=nullptr);
		// call update method (not implemented yet, s it completed)
		w_obs->update();  //list of section flow updated
	}
} // End of namespace