
// C++ include
#include <iostream>
// boost include
#include <boost/range/iterator_range.hpp>
// Package includes
//#include "../Algorithm/dbpp_TestRhsImpl.h"
//#include "../Numerics/dbpp_TestCalculFF.h"
#include "../Discretization/dbpp_EMcNeilBCimpl.h"
#include "../Numerics/dbpp_TwoStepsIntegrator.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
//#include "../Utility/dbpp_TestLogger.h"
#include "dbpp_TestEMcNeilVec.h"

namespace dbpp {
TestEMcNeilVec::TestEMcNeilVec(SweRhsAlgorithm *aRhsAlgo, const Gamma &aBCnd)
    : EMcNeil1D(),         // base class implementation
      m_rhsAlgo{aRhsAlgo}, // default value
      m_timePrm(0., 0., 0.), m_bc{aBCnd} {}

TestEMcNeilVec::TestEMcNeilVec(SweRhsAlgorithm *aRhsAlgo,
                               const TimePrm &aTimeprm)
    : EMcNeil1D(),         // base class implementation
      m_rhsAlgo{aRhsAlgo}, // default value
      m_timePrm(aTimeprm), m_bc{} {
  // add code here
}

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

  TestRhsImpl *w_downCast = dynamic_cast<TestRhsImpl *>(m_rhsAlgo);
  // check if it succeed
  assert(nullptr != w_downCast);
  auto w_listOfSections = w_downCast->getListPhysicsObjects();
  // EMcNeilBCImpl getUpStream/DownStream
  EMcNeilBCimpl w_testImpl{w_listOfSections};

  // apply algorithm (rhs numerical treatment of convective flux, source terms)
  m_rhsAlgo->calculate(m_U12);

  // ++++++ half time step +++++++++++++

  // Create step integrator (pass RHS as argument)
  TwoStepsIntegrator w_timeStepper;
  w_timeStepper.setInitSln(m_U12.first->values().to_stdVector(),
                           m_U12.second->values().to_stdVector());

  w_timeStepper.setIntegratorStep(
      TwoStepsIntegrator::eIntegratorStep::predictorStep);
  // advance one time-step
  w_timeStepper.step(m_rhsAlgo, Simulation::instance()->simulationTimeStep());

  // step through time (n+1/2) by using Runge-Kutta family integrator
  auto w_midState = w_timeStepper.getMidState();
  auto m_U1p = w_midState.first->values().to_stdVector();
  auto m_U2p = w_midState.second->values().to_stdVector();

  // write to debug file
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(static_cast<unsigned>(m_U1p.size()), m_U1p, m_U2p));

  // ++++++ final time step +++++++++++++

  // apply algorithm (rhs numerical treatment of convective flux, source terms)
  m_rhsAlgo->calculate(w_midState); // updated values

  // n+1 time step
  w_timeStepper.setIntegratorStep(
      TwoStepsIntegrator::eIntegratorStep::correctorStep);

  // advance one time-step
  w_timeStepper.step(m_rhsAlgo, Simulation::instance()->simulationTimeStep());

  auto w_finalState = w_timeStepper.getCurrentState();
  auto m_U1 = w_finalState.first->values().to_stdVector();
  auto m_U2 = w_finalState.second->values().to_stdVector();

  // write to debug file
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(static_cast<unsigned>(m_U1.size()), m_U1p, m_U2));

  // notify all observers
  // setState();
  // update the global discretization
  dbpp::GlobalDiscretization::instance()->update();

  // update boundary condition (need to check for this one)
  dbpp::GlobalDiscretization::instance()->gamma().applyBC();

  // update section flow (one element in the list)
  m_listofObs.front()->update();
}

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
