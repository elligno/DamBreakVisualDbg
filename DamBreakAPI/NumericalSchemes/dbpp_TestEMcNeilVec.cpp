
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
// TestEMcNeilVec::TestEMcNeilVec(
//    SweRhsAlgorithm *aRhsAlgo /*, const Gamma &aBCnd*/)
//    : EMcNeil1D(),         // base class implementation
//      m_rhsAlgo{aRhsAlgo}, // default value
//      m_timePrm(0., 0., 0.) /*, m_bc{aBCnd}*/ {}

TestEMcNeilVec::TestEMcNeilVec(SweRhsAlgorithm *aRhsAlgo,
                               const TimePrm &aTimeprm)
    : EMcNeil1D(),         // base class implementation
      m_rhsAlgo{aRhsAlgo}, // default value
      m_timePrm(aTimeprm) {
  // add code here
}

// DESIGN NOTE: this algorithm need to be re-factored, there is a lot of crapt.
// Basically the skeleton of the way it should be is there, in the future we
// manipulate the SweRhsAlgorithm that is responsible for applying the physical
// algorithm (for example Nujic take into ... and introduce a new concept of
// compatibility relationship). All of this is under construction, but we are
// on the right track.
void TestEMcNeilVec::advance(/*GlobalDiscretization aGbl*/) {

  using namespace std;

  // require resource
  dbpp::DbgLogger::instance()->open();
  // sanity check
  if (!dbpp::DbgLogger::instance()->isOpen()) {
    dbpp::Logger::instance()->OutputError( // C++17 return char* and not const
        std::string{"Couldn't open file for writing debug info\n"}.data());
  }

#if 0
  // debugging
  std::vector<double> w_U1;
  w_U1.reserve(GlobalDiscretization::instance()->Uh().size());
  GlobalDiscretization::instance()->to_stdVector(
      w_U1, GlobalDiscretization::NodalValComp::A);
  std::vector<double> w_U2;
  w_U1.reserve(GlobalDiscretization::instance()->Uh().size());
  GlobalDiscretization::instance()->to_stdVector(
      w_U2, GlobalDiscretization::NodalValComp::Q);
#endif

  // just to make sure we are on the right track
  // current version we only support E. McNeil data
  auto w_activeDiscr = Simulation::instance()->getActiveDiscretization();
  if (DamBreakData::DiscrTypes::emcneil == w_activeDiscr) {
    // Design Note
    //  this method signature is deprecated
    auto &m_bc = GlobalDiscretization::instance()->gamma();
    if (m_bc.getBCtype() == Gamma::eBCtypes::Hten_Qzero) {
      // set b.c. nodes
      m_rhsAlgo->setBCNodes(m_bc.getBCNodeAmont().Values(),
                            m_bc.getBCNodeAval().Values());
    }
  } else {
    return; // for now not supporting other
  }

  // IMPORTANT
  // this is a temporary fix, only for debugging purpose
  // set m_U12 values!! based on global discretization
  // these are updated at each iteration
  // initialize(GlobalDiscretization::instance(), 0.);

  // just testing a new impl of b.c.
  // TestRhsImpl *w_downCast = dynamic_cast<TestRhsImpl *>(m_rhsAlgo);
  // check if it succeed
  //  assert(nullptr != w_downCast);
  //  auto w_listOfSections = w_downCast->getListPhysicsObjects();
  // EMcNeilBCImpl getUpStream/DownStream
  // need to pass a shared_ptr, need a review
  //  EMcNeilBCimpl w_testImpl{w_listOfSections};

  auto checkU1 = m_U12.first->values().to_stdVector();
  auto checkU2 = m_U12.second->values().to_stdVector();

  // (rhs numerical treatment of convective flux, source terms)
  m_rhsAlgo->calculate(m_U12);

  // ++++++ half time step (predictor) +++++++++++++

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

  // write state values for debugging purpose
  const auto w_iterNostr =
      std::to_string(Simulation::instance()->getIterationNumber());
  dbpp::DbgLogger::instance()->write2file(
      std::string{"++++ Iteration number: "} + w_iterNostr);
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(static_cast<unsigned>(m_U1p.size()), m_U1p, m_U2p));
  // write to debug file
  // dbpp::DbgLogger::instance()->write2file_p(
  //     std::make_tuple(static_cast<unsigned>(m_U1p.size()), m_U1p, m_U2p));

  // ++++++ final time step (corrector) +++++++++++++

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
  dbpp::DbgLogger::instance()->write2file(
      std::make_tuple(static_cast<unsigned>(m_U1.size()), m_U1, m_U2));

  // update solution
  // since we don't need the final state anymore
  m_U12 = std::move(w_finalState); //('move' xvalue: eXpiring value)

  // release respource
  if (dbpp::DbgLogger::instance()->isOpen()) {
    dbpp::DbgLogger::instance()->close();
  }
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

  // Create step integrator (pass RHS as argument)
  TwoStepsIntegrator w_timeStepper; // only one supported

  // logical time (pdate all nodes)
  TimePrm w_timePrm(0., 0.01, aTimeTo); // simulator set to 22.5
  while (!w_timePrm.finished()) {

    // rhs numerical treatment of convective flux, source terms
    m_rhsAlgo->calculate(m_U12);

    // ++++++ half time step (predictor) +++++++++++++
    w_timeStepper.setInitSln(m_U12.first->values().to_stdVector(),
                             m_U12.second->values().to_stdVector());

    w_timeStepper.setIntegratorStep(
        TwoStepsIntegrator::eIntegratorStep::predictorStep);
    // advance one time-step
    w_timeStepper.step(m_rhsAlgo, Simulation::instance()->simulationTimeStep());

    // ++++++ final time step (corrector) +++++++++++++

    StateVector w_midState{nullptr, nullptr}; // debugging purpose
    // rhs numerical treatment of convective flux, source terms
    m_rhsAlgo->calculate(w_midState); // updated values

    // n+1 time step
    w_timeStepper.setIntegratorStep(
        TwoStepsIntegrator::eIntegratorStep::correctorStep);

    // advance one time-step
    w_timeStepper.step(m_rhsAlgo, Simulation::instance()->simulationTimeStep());
  }
}

// just testing an implementation
void TestEMcNeilVec::initialize(const GlobalDiscretization *aGblDiscr,
                                double aTime) {
  assert(aTime == 0.); // debugging purpose
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
  // StateVector w_pairAQ = {std::move(w_U1shrptr), std::move(w_U2shrptr)};
  m_U12 = {std::move(w_U1shrptr), std::move(w_U2shrptr)};

#if 0 // for now we don't need that
  m_timePrm.setStartTime(aTime);
  m_timePrm.setTimeStepMode(dbpp::TimePrm::TimeStepMode::VARIABLE_TIME_STEP);
  m_timePrm.initTimeLoop(); // initialize simulation time
  //  parameters m_tip->increaseTime(calculateDt()); // t1=t0+dt;
  // dbpp::Simulation::instance()->setSimulationTimeStep(TimeStepCriteria(...));
#endif
}
} // namespace dbpp
