
// C++ include
#include <iostream>
// boost include
#include <boost/range/iterator_range.hpp>
// Package includes
//#include "../Algorithm/dbpp_TestRhsImpl.h"
//#include "../Numerics/dbpp_TestCalculFF.h"
#include "../Numerics/dbpp_TwoStepsIntegrator.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
//#include "../Utility/dbpp_TestLogger.h"
#include "dbpp_TestEMcNeilVec.h"

namespace dbpp {
TestEMcNeilVec::TestEMcNeilVec(SweRhsAlgorithm *aRhsAlgo, const Gamma &aBCnd)
    : EMcNeil1D(),         // base class implementation
      m_rhsAlgo{aRhsAlgo}, // default value
      m_timePrm(0., 0., 0.), m_bc{aBCnd} {
  //  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));
}
TestEMcNeilVec::TestEMcNeilVec(SweRhsAlgorithm *aRhsAlgo,
                               const TimePrm &aTimeprm)
    : EMcNeil1D(),         // base class implementation
      m_rhsAlgo{aRhsAlgo}, // default value
      m_timePrm(aTimeprm), m_bc{} {
  // add code here
}

// Design Note:
// ------------
//  In this implementation we are using the 'predictor' and 'corrector'
//  implementation of the base class (use std::vector). But another
//  implementation could make use of of an integrator (explicit)
//  in which the rhs is pass as argument.
// some implementation of advance (i am just exploring some design
// and testing).
// We are using the new concept of the RHS algorithm (step of the algo)
// wrapped inside of the algo, this way we can make different algorithm
// base on the numerical representation (treatment of the rhs).
//
// 		std::cout << "Upstream bc values: " << U1[0] << " " << U2[0] <<
// "
// "
// << H[0] << "\n"; 		std::cout << "Downstream bc values: " << U1[100]
// << " " << U2[100] << " " << H[100] << "\n";
//
// DESIGN NOTE: this algorithm need to be re-factored, there is a lot of crapt.
// Basically the skeleton of the way it should be is there, in the future we
// manipulate the SweRhsAlgorithm that is responsible for applying the physical
// algorithm (for example Nujic take into ... and introduce a new concept of
// compatibility relationship). All of this is under construction, but we are
// on the right track.
void TestEMcNeilVec::advance() {
  using namespace std;

  // just to make sure we are on the right track
  // current version we only support E. McNeil data
  auto w_activeDiscr = Simulation::instance()->getActiveDiscretization();
  if (DamBreakData::DiscrTypes::emcneil == w_activeDiscr) {
    if (m_bc.getBCtype() == Gamma::eBCtypes::Hten_Qzero) {
      // set b.c. nodes
      m_rhsAlgo->setBCNodes(m_bc.getBCNodeAmont().Values(),
                            m_bc.getBCNodeAval().Values());
    }
  }

  // apply algorithm (rhs numerical treatment of convective flux, source terms)
  m_rhsAlgo->calculate(m_U12);

  // ++++++ half time step +++++++++++++

  // Create step integrator (pass RHS as argument)
  TwoStepsIntegrator w_timeStepper;
  w_timeStepper.setInitSln(m_U12.first->values().to_stdVector(),
                           m_U12.second->values().to_stdVector());
  w_timeStepper.setIntegratorStep(
      TwoStepsIntegrator::eIntegratorStep::predictorStep);
  w_timeStepper.step(m_rhsAlgo, Simulation::instance()->simulationTimeStep());

  // step through time (n+1/2) by using Runge-Kutta family integrator
  // don't have much choice, because advance is call only once. If 2-steps
  // algorithm is needed we have toi do it inside.
  // predictor(); // U1p and U2p up-to-date, will be used for next step

  auto w_midState = w_timeStepper.getMidState();
  auto m_U1p = w_midState.first->values().to_stdVector();
  auto m_U2p = w_midState.second->values().to_stdVector();

  // write to debug file
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(static_cast<unsigned>(m_U1p.size()), m_U1p, m_U2p));

  // ++++++ final time step +++++++++++++

  // state vector for intermediate state (need to be updated)
  std::shared_ptr<gridLattice> w_gridLattice( // notsure about this one
      new gridLattice(w_midState.first->grid()));

  // sanity check
  assert(w_gridLattice->getNoPoints() ==
         w_midState.first->grid().getNoPoints());

  // apply algorithm (rhs numerical treatment of convective flux, source terms)
  m_rhsAlgo->calculate(w_midState /*, w_rhsDiscr.get()*/); // updated values

  // n+1 time step
  // corrector();

  // notify all observers
  setState();
}

#if 0
// those equations are evaluated over each cell
// then for each cell we compute the cell face
// numerical flux
void TestEMcNeilVec::predictor() {
  // predictor step of the Runge-Kutta family
  // deprecated, in future version use a separate class
  // that is responsible for time stepping algorithm
  // time step at ...
  const auto dt = dbpp::Simulation::instance()->simulationTimeStep();
  // const int NbSections = m_rhs.m_FF1.size(); // debugging purpose
  const auto dx = m_U12.first->grid().Delta(1);

  // predictor-step (this loop doesn't make sense)
  // so confusing, actually we are looping on grid node
  // and at each grid node there is a section flow
  // associated with it (that what it is)
  for (unsigned j = 1; j < m_rhs.m_FF1.size(); j++) {
    m_U1p[j] = U1[j] - dt / dx * (m_rhs.m_FF1[j] - m_rhs.m_FF1[j - 1]);
    m_U2p[j] = U2[j] - dt / dx * (m_rhs.m_FF2[j] - m_rhs.m_FF2[j - 1]) +
               dt * m_rhs.m_S[j];
  }
}

void TestEMcNeilVec::corrector() {
  // time step at ...
  const auto dt = dbpp::Simulation::instance()->simulationTimeStep();
  // const int NbSections = m_rhs.m_FF1.size(); // debugging purpose
  const auto dx = m_U12.first->grid().Delta(1);

  //	Calcul des valeurs des variables d'état (compute U1, U2)
  for (unsigned j = 1; j < m_rhs.m_FF1.size();
       j++) // computational node except i=0 which is set by B.C.
  {
    U1[j] = 0.5 * (U1[j] + m_U1p[j] -
                   dt / dx * (m_rhs.m_FF1[j] - m_rhs.m_FF1[j - 1]));
    U2[j] = 0.5 * (U2[j] + m_U2p[j] -
                   dt / dx * (m_rhs.m_FF2[j] - m_rhs.m_FF2[j - 1])
                   //- dt/dx * (PF2[j] - PF2[j-1]) not considering pressure for
                   // this simulation
                   + dt * m_rhs.m_S[j]);
  }
}
#endif

// void TestEMcNeilVec::setBC() {
//  //	Calcul des valeurs intermédiaires des variables d'état
//  m_U1p[0] = U1[0]; // w_amVal[0]
//  m_U2p[0] = U2[0]; // w_amVal[1]

//  m_U1p[m_NbSections - 1] = U1[m_NbSections - 1]; // w_avVal[0]
//  m_U2p[m_NbSections - 1] = U2[m_NbSections - 1]; // w_avVal[1]
//}

void TestEMcNeilVec::mainLoop(const GlobalDiscretization *aGblDiscr,
                              const double aTimeTo) {
  const auto &w_bc = aGblDiscr->gamma();

  // just to make sure we are on the right track
  // current version we only support E. McNeil data
  auto w_activeDiscr = Simulation::instance()->getActiveDiscretization();
  if (DamBreakData::DiscrTypes::emcneil == w_activeDiscr) {
    if (w_bc.getBCtype() == Gamma::eBCtypes::Hten_Qzero) {
      // set b.c. nodes
      m_rhsAlgo->setBCNodes(w_bc.getBCNodeAmont().Values(),
                            w_bc.getBCNodeAval().Values());
    }
  }

  // logical time (pdate all nodes)
  TimePrm w_timePrm(0., 0.01, aTimeTo); // simulator set to 22.5
  while (!w_timePrm.finished()) {
    // apply algorithm (rhs numerical treatment of convective flux, source
    // terms)
    m_rhsAlgo->calculate(m_U12);
  }
}

// just testing an implementation
void TestEMcNeilVec::initialize(const GlobalDiscretization *aGblDiscr,
                                double aTime) {
  std::vector<double> w_U1;
  w_U1.reserve(aGblDiscr->Uh().size());
  aGblDiscr->to_stdVector(w_U1, GlobalDiscretization::NodalValComp::A);
  std::vector<double> w_U2;
  w_U1.reserve(aGblDiscr->Uh().size());
  aGblDiscr->to_stdVector(w_U2, GlobalDiscretization::NodalValComp::Q);

  std::shared_ptr<scalarField> w_U1shrptr{nullptr};
  std::shared_ptr<scalarField> w_U2shrptr{nullptr};

  DamBreakData w_activeData{Simulation::instance()->getActiveDiscretization()};
  std::string w_gridprms{"d=1 [0,"};
  w_gridprms += std::to_string(w_activeData.x_max()) +
                std::string{"] [1:"}; // ui value int x-max coord ] [1:
  w_gridprms += std::to_string(w_activeData.nbSections() - 1) +
                std::string{"]"}; // ui value nbgridpoints

  std::shared_ptr<gridLattice> w_grid1D =
      std::make_shared<gridLattice>(w_gridprms);

  // attach a field of values on the grid
  w_U1shrptr.reset(new scalarField(w_grid1D, w_U1, std::string{"A"}));
  w_U2shrptr.reset(new scalarField(w_grid1D, w_U1, std::string{"Q"}));

  // what's going here?
  StateVector w_pairAQ = {std::move(w_U1shrptr), std::move(w_U2shrptr)};

  m_timePrm.setStartTime(aTime);
  m_timePrm.setTimeStepMode(dbpp::TimePrm::TimeStepMode::VARIABLE_TIME_STEP);
  m_timePrm.initTimeLoop(); // initialize simulation time
  //  parameters m_tip->increaseTime(calculateDt()); // t1=t0+dt;
  // dbpp::Simulation::instance()->setSimulationTimeStep(TimeStepCriteria(...));
}
} // namespace dbpp
