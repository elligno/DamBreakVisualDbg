
// stl include
#include <numeric> // UpwindScheme1st algorithm
// package includes
#include "dbpp_TestNewAlgo.h"
// packages includes
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../Discretization/dbpp_ReconstrUtility.h"
#include "../Numerics/dbpp_TestCalculFF.h"
#include "../Numerics/dbpp_UpwindScheme1st.hpp"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
#include "../Utility/dbpp_TestLogger.h"

namespace dbpp {
//
// Design Note:
//   this implementation we could re-use the vector U1,U2 from base
//   class to fill the algorithm for this first test. The point ...
// 	TestNewAlgo::TestNewAlgo( GlobalDiscretization* aGdiscr)
// 	: EMcNeil1D(),       // default ctor: cfl=0.6, nbsections=101, dx=10.
// 	  m_gdisr(aGdiscr),   // E. McNeil data in the original code
// 		m_NbSections(101), // default value
// 		m_ListSectFlow(nullptr)
// //	  m_dt(0.)          // time step of the simulation
// 	{
// 		// debugging purpose
//     emcil::Logger::instance()->OutputSuccess("We are in the TestNewAlgo
//     ctor\n");
//
//     // kind of observer design pattern
// //		aGdiscr->Uh() = this; ???
// 	}

// Design Note:
// ------------
// i am not sure if we are going to keep this code below.
// may be for he first test, ...
// in the next version we will make use of the finite volume
// discretization (need omega and gamma (B.C. to compute calculFF))
// numerical flux at cell face and then use it the time stepper.
// It could also be done in SweRhsAlgorithm where we could provide
// a new method calculate with the following signature
// calculate( StateVector, GlobalFVdiscr)
//
//  NOTE: need to refactor this method
//
void TestNewAlgo::timeStep() {
  // memory allocation
  // Resize();

  // not sure about all this variables
  // 		std::vector<double> m_FF1;
  // 		std::vector<double> m_FF2;
  // 		std::vector<double> m_S;
  std::vector<double> m_U1p;
  std::vector<double> m_U2p;
  std::vector<double> m_H;
  constexpr auto m_NbSections = 101;

  // create a spatial discretization (rhs terms numerical discretization)
  // design note shall inherit from Acccess mechanism interface retrieving

  // std::unique_ptr<TestCalculFF> m_numTreat =
  // std::make_unique<TestCalculFF>(); m_numTreat.reset( new TestCalculFF);

  // testing the same implementation but with state vector
  //     std::vector<std::pair<real,real>> w_sU1LR;       // A
  //     w_sU1LR.reserve( m_U12.first->values().size());
  //
  //     std::vector<std::pair<real,real>> w_sU2LR;       // Q
  //     w_sU2LR.reserve( m_U12.second->values().size());

  // same reconstruction but state vector implementation
  // pass as argument the scalarField
  // NOTE: check if we use the reconstr_vec
  // shall return a map with key=cell face global index
  // and value = cell face variables
  // check implementaion
  //    ReconstrUtil::reconstrv_sv( m_U12, w_sU1LR, w_sU2LR);

  // base right-hand-side discretization terms
  //		std::unique_ptr<BaseNumTreatmemt> w_baseNum = nullptr;

  // Design NOTE: this doesn't make any sense. Next version we will
  // have the signature of this method changed
  // void traitementTermeSource2(m_S, U2, U1,ListOfSectionFlow)
  // we won't need that kind of crapt below anymore
  // discretization parameters from E. McNeil settings
  // from simulation global instance shall retrieve the active discretization
  //		DamBreakData w_mcmilData( DamBreakData::DiscrTypes::emcneil);

  // const double dx = w_mcmilData.dx();
  //		assert( w_mcmilData.dx() == 10.); // sanity check

  // Initialized from variable of base class (range ctor)
  //		std::vector<double> w_U1(U1.cbegin(), U1.cend());
  //		std::vector<double> w_U2(U2.cbegin(), U2.cend());

  // compute flux at cell face using the flux algorithm
  // with different  implementation of default base class
  // 		if( IFluxAlgoImpl* w_testImpl =
  // dynamic_cast<IFluxAlgoImpl*>(w_numTreat.get()))
  // 		{
  // 			// interface supported
  // 			w_testImpl->calculFF( m_FF1, m_FF2, w_sU1LR, w_sU2LR);
  // 		}
  // 		else // doesn't support interface
  // 		{    // use base implementation of rhs discretization
  // 			// default numerical treatment for rhs discretization
  // 			w_baseNum.reset(new BaseNumTreatmemt);
  //
  // 			// these are deprecated, gradient of state variable over
  // each cell 			std::vector<double> w_dU1(U1.size()); // all
  // that stuff doesn't make sense 			std::vector<double>
  // w_dU2(U2.size()); // but keep it this way for now
  //
  // 			// compute flux using a Riemann HLL solver (unit section
  // width)
  // 			// base implementation in terms of vector
  // 			if( w_mcmilData.isUnitSectionWidth()) // check section
  // flow type
  // 			{
  // 				// section flow (for now will do)
  // 				w_baseNum->CalculFF(m_FF1,m_FF2,w_U1,w_U2,w_dU1,w_dU2,m_NbSections,1.);
  // 			}
  // 			else
  // 			{
  // 				w_baseNum->CalculFF(m_FF1,m_FF2,w_U1,w_U2,w_dU1,w_dU2,m_NbSections,w_mcmilData.getWidth());
  // 			}
  //		}

  // apply boundary condition to section flow (upstream and downstream)
  // according to current boundary condition
  setSectionFlowBC();

  //		w_numTreat->TraitementTermeSource2( m_S, U2, U1, H, n, dx,
  // U1.size(), B); 		 w_numTreat->TraitementTermeSource2( m_S, w_U2,
  // w_U1, m_U12.first->grid().Delta(1), m_ListSectFlow);

  // first step of two-phase algorithm (n+1/2)
  // (computational domain: 1,...,99 with 0 tied)
  predictor(); // compute U1p, U2p (computational node except i=0 which is set
               // by B.C.)

  // set boundary node values, in this case i=0 and i=N+1 (ghost node)
  // U1p=..., U2p=... intermediate state
  // use the base class implementation of the b.c.
  setBC(); // really need it? because we are not using U1p and U2p
           // of course since we are copying from scalarFiled which contains
           // values of computational domain

  // write to debug file
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(m_NbSections, m_U1p, m_U2p));

  // now clear some of the data but still have the capacity since we used
  // reserve
  // 		w_sU1LR.clear();
  // 		w_sU2LR.clear();
  //    ClearAndResize();

  // Reconstruction of the intermediate state (m_U12p updated in the predictor)
  //		ReconstrUtil::reconstrv_sv(m_U12p,w_sU1LR, w_sU2LR);

  // assert();
  // 		if( IFluxAlgoImpl* w_testImpl =
  // dynamic_cast<IFluxAlgoImpl*>(w_numTreat.get()))
  // 		{
  // 			// in this case we assume that unit section flow (also
  // deprecated)
  //       w_testImpl->calculFF(m_FF1,m_FF2,w_sU1LR, w_sU2LR);
  // 		}
  // 		else
  // 		{
  // 			// compute according to the base numerical algorithm
  // 			if( w_baseNum==nullptr) // sanity check
  // 			{
  // 				// what to do? throw an exception? think about
  // it? 				std::cerr << "We have a problem with the
  // rhs discretization\n";
  // 			}
  // 			std::vector<double> w_dU1(U1.size()); // all that stuff
  // doesn't
  // make sense 			std::vector<double> w_dU2(U2.size()); //
  // but keep it this way for now
  //
  // 			// compute flux using a Riemann HLL solver (unit section
  // width)
  // 			// base implementation in terms of vector
  // 			if( w_mcmilData.isUnitSectionWidth()) // check section
  // flow type
  // 			{
  // 				// set width to 1.
  // 				w_baseNum->CalculFF(m_FF1,m_FF2,m_U1p,m_U2p,w_dU1,w_dU2,m_NbSections,1.);
  // 			}
  // 			else
  // 			{
  // 				// not unit width, then retrieve width from
  // section
  // 				w_baseNum->CalculFF(m_FF1,m_FF2,m_U1p,m_U2p,w_dU1,w_dU2,m_NbSections,w_mcmilData.getWidth());
  // 			}
  // 		}

  // compute source term
  // w_numTreat->TraitementTermeSource2( m_S, m_U2p, m_U1p, H, n, dx, U1.size(),
  // B);
  // H of section flow used inside the algorithm
  //	w_numTreat->TraitementTermeSource2(m_S, m_U2p, m_U1p,
  // m_U12.first->grid().Delta(1), m_ListSectFlow);

  // n+1 final state of the two steps algorithm
  corrector();

  // cleaned-up properly
  //    Clear();

  // sure, because it takes section flow water level and update 'H' from base
  // class
  //	updateH();

  // observer design patter (update list of section flow)
  setState();
}

// predictor algorithm
// want to to use RealNumArray<real> implementation
void TestNewAlgo::predictor() {
  using namespace dbpp;

  // reminder: auto decay here deduce to const char*
  auto w_msg = "We are in the TestNewAlgo::predictor()\n";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));

  // testing the same implementation but with state vector
  std::vector<std::pair<real, real>> w_sU1LR; // A
  w_sU1LR.reserve(m_U12.first->values().size());

  std::vector<std::pair<real, real>> w_sU2LR; // Q
  w_sU2LR.reserve(m_U12.second->values().size());

  // same reconstruction but state vector implementation
  // pass as argument the scalarField
  // NOTE: check if we use the reconstr_vec
  // shall return a map with key=cell face global index
  // and value = cell face variables
  // check implementaion
  ReconstrUtil::reconstrv_sv(m_U12, w_sU1LR, w_sU2LR);

  // some working variables
  std::vector<double> m_FF1{100}; // initalize to zero
  std::vector<double> m_FF2{100}; // ditto
  std::vector<double> m_S{100};   // ditto
  // this class will be refactored
  std::unique_ptr<TestCalculFF> m_numTreat = std::make_unique<TestCalculFF>();

  // compute flux at cell face using the flux algorithm
  // with different  implementation of default base class
  if (IFluxAlgoImpl *w_testImpl =
          dynamic_cast<IFluxAlgoImpl *>(m_numTreat.get())) {
    // interface supported
    w_testImpl->calculFF(m_FF1, m_FF2, w_sU1LR, w_sU2LR);
  } // else we do? stop with error msg?

  // set in the base class advance() method (initial set up)
  const auto &w_U1 = m_U12.first->values();
  const auto &w_U2 = m_U12.second->values();

  // spatial grid spacing or step (finite difference discretization)
  const auto w_dx = m_U12p.first->grid().Delta(1);

  // dF_i = (F_i - F_i-1)/dx first-order derivative with backward upwind stencil
  auto w_dFF1 = UpwindDerivatr1st(m_FF1);
  auto w_dFF2 = UpwindDerivatr1st(m_FF2);

  // water level of each section
  std::vector<double> w_H;
  w_H.reserve(101);

  // retrieve water level of each section (from physical system)
  //  transform(aListSectFlow->getList().cbegin(),
  //  aListSectFlow->getList().cend(),
  //            std::back_inserter(w_H), std::bind(&SectFlow::H, _1));

  //  std::vector<double> w_N{101}; // set to zero
  //  BaseNumTreatmemt w_baseNumTreatment;
  //  w_baseNumTreatment.TraitementTermeSource2();

  // this code os obsolete
  //  m_numTreat->TraitementTermeSource2(
  //      m_S, w_U2pvec, m_U12.first->values().to_stdVector(),
  //      m_U12.first->grid().Delta(1), m_ListSectFlow);

  //     w_U1p(1) = m_amontBC[0];
  //     w_U2p(1) = m_amontBC[1];

  // Design Note
  //  when we use to inherit from base class EMcNeil1D which hold
  //  data for solution (protected), but it is not anymore supported
  //  actually these are node BC (fixed node) shall retrieve BC from
  //  global discretization and set these node

  //  const Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  // scalar field values over the grid
  auto &w_U1p = m_U12p.first->values();
  auto &w_U2p = m_U12p.second->values();
  w_U1p(1) = U1[0]; // upstream cnd. A
  w_U2p(1) = U2[1]; // upstream cnd. Q

  // time step of the simulation
  const auto w_dt = dbpp::Simulation::instance()->simulationTimeStep();
  const auto w_lambda = w_dt / w_dx;

  // computational domain (don't consider first node (tied node by bc))
  for (auto l = 2; l <= m_U12p.first->grid().getDivisions(1); ++l) {
    // mass equations
    w_U1p(l) = w_U1(l) - w_lambda * w_dFF1[l - 1];

    // momentum equation (with source term)
    w_U2p(l) = w_U2(l) - w_lambda * w_dFF2[l - 1] + w_dt * m_S[l - 1];
  }
}

void TestNewAlgo::corrector() {
  using namespace dbpp;

  auto w_msg = "We are in the TestNewAlgo::corrector()\n";

  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));

  // testing the same implementation but with state vector
  std::vector<std::pair<real, real>> w_sU1LR; // A
  w_sU1LR.reserve(m_U12.first->values().size());

  std::vector<std::pair<real, real>> w_sU2LR; // Q
  w_sU2LR.reserve(m_U12.second->values().size());

  // Reconstruction of the intermediate state (m_U12p updated in the predictor)
  ReconstrUtil::reconstrv_sv(m_U12p, w_sU1LR, w_sU2LR);

  // some working variables
  std::vector<double> m_FF1{100};
  std::vector<double> m_FF2{100};
  std::vector<double> m_S{100};

  // this class will be refactored
  std::unique_ptr<TestCalculFF> m_numTreat = std::make_unique<TestCalculFF>();

  // assert();
  if (IFluxAlgoImpl *w_testImpl =
          dynamic_cast<IFluxAlgoImpl *>(m_numTreat.get())) {
    // in this case we assume that unit section flow (also deprecated)
    w_testImpl->calculFF(m_FF1, m_FF2, w_sU1LR, w_sU2LR);
  }

  // retrieve grid from field
  const auto &w_grid1d = m_U12p.first->grid();

  // scalar field values over the grid (this time they will change)
  auto &w_U1 = m_U12.first->values();
  auto &w_U2 = m_U12.second->values();

  // half-time step data
  const auto &w_U1p = m_U12p.first->values();
  const auto &w_U2p = m_U12p.second->values();

  // flux at cell face
  auto w_dFF1_u = UpwindDerivatr1st(m_FF1);
  auto w_dFF2_u = UpwindDerivatr1st(m_FF2);

  // ToDo: to be completed copy to vector
  // H of section flow used inside the algorithm
  std::vector<double> w_U1prev;
  std::vector<double> w_U2prev;
  m_numTreat->TraitementTermeSource2(
      m_S, w_U2prev, w_U1prev, m_U12.first->grid().Delta(1), m_ListSectFlow);

  // do i need to apply B.C. as we did in the predictor?
  // certainly, just call global discretization
  w_U1(1) = U1[0]; // set B.C. value
  w_U2(1) = U2[0]; // set B.C. value

  // time step of the simulation
  const auto w_dt = dbpp::Simulation::instance()->simulationTimeStep();
  // constant at each time step
  const auto w_lambda = w_dt / m_U12p.first->grid().Delta(1);

  // computational node except i=0 which is set by B.C.
  for (auto l = 2; l <= w_grid1d.getDivisions(1); ++l) {
    w_U1(l) = 0.5 * (w_U1(l) + w_U1p(l) -
                     w_lambda * w_dFF1_u[l - 1]); // mass equation
    w_U2(l) = 0.5 * (w_U2(l) + w_U2p(l) -
                     w_lambda * w_dFF2_u[l - 1] + // momentum equation
                     w_dt * m_S[l - 1]);
  }
}

void TestNewAlgo::setInitSln(const StateVector &aU,
                             ListSectFlow *aListofSect /*=nullptr*/) {
  // not completed
  m_U12 = aU;
  m_U12p = aU;
  if (nullptr != aListofSect) // debug purpose, should be removed
    aListofSect = nullptr;
}

// overriden for debugging purpose
void TestNewAlgo::setState() {
  std::copy(m_U12.first->values().getPtr(),
            m_U12.first->values().getPtr() + m_U12.first->values().size(),
            std::begin(U1));

  std::copy(m_U12.second->values().getPtr(),
            m_U12.second->values().getPtr() + m_U12.second->values().size(),
            std::begin(U2));

  // not all implemented
  notify(); // call observer update() method
}

// TODO: need to check the setBCNode() method, for now is just set to 0
void TestNewAlgo::setSectionFlowBC() {
  // good practice
  try {
    // list observer from base class
    m_ListSectFlow =
        boost::polymorphic_downcast<ListSectFlow *>(m_listofObs.front());
  } catch (const std::bad_cast &bc) {
    std::cerr << "Couldn't cast down to ListSectionFlow " << bc.what() << "\n";
  }

  std::vector<double> m_H;
  // create the vector of H
  m_H.reserve(m_ListSectFlow->getList().size());
  std::transform(m_ListSectFlow->getList().begin(),
                 m_ListSectFlow->getList().end(), std::back_inserter(m_H),
                 std::bind(&SectFlow::H, std::placeholders::_1));

  // set boundary value at both end (downstream and upstream)
  // 		m_ListSectFlow->getList()[0]->setH(m_amontBC[2]);
  // 		m_ListSectFlow->getList()[m_U12.first->values().size()]->setH(m_avalBC[2]);
  // // ghost node

  // temporary fix
  m_ListSectFlow->getList()[0]->setH(m_H[0]);
  m_ListSectFlow->getList()[m_U12.first->values().size()]->setH(
      m_H[100]); // ghost node
}
} // namespace dbpp
