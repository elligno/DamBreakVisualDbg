
#pragma once

// STL includes
#include <valarray> // fast floating-point array
#include <vector>
// Lib includes
#include "Algorithm/dbpp_SweRhsAlgorithm.h"

namespace dbpp {
//
// Explicit Integrator
//
class TwoStepsIntegrator {
  // using solnU12 = std::pair<std::vector<double>, std::vector<double>>;

public:
  enum class eIntegratorStep { predictorStep = 0, correctorStep = 1 };

public:
  TwoStepsIntegrator() = default; //??

  void setInitSln(const std::vector<double> &aU1,
                  const std::vector<double> &aU2) {
    // NOTE we need first to initialize the scalar field,
    // at this point it is set to nullptr
    // need to do something like that
    // auto w_activeDiscr = Simulation::instance()->getActiveDiscretization();
    //  if (DamBreakData::DiscrTypes::emcneil == w_activeDiscr) {
    //  }
    //  m_currState.first.reset(make_shared<gridLattice>(100,0.,1000.),name,vector);
    auto &w_U1array = m_currState.first->values();
    std::copy(aU1.cbegin(), aU1.cend(), w_U1array.begin());
    auto &w_U2array = m_currState.second->values();
    std::copy(aU2.cbegin(), aU2.cend(), w_U2array.begin());
  }
  void setInitSln(const double *aU1, const double *aU2, const unsigned aSize) {
    // design note: add a check on the size of both vector, must be equal
    // U1.reserve(aSize);
    // U2.reserve(aSize);
    // design note: check pointer null value (try-catch)
    auto &w_U1array = m_currState.first->values();
    std::copy(&aU1[0], &aU1[0] + aSize, w_U1array.begin());
    auto &w_U2array = m_currState.second->values();
    std::copy(&aU2[0], &aU2[0] + aSize, w_U2array.begin());
  }

  // design note: what about move semantic?? in some situation ...
  // to be completed ... not sure about this one!!
  void setInitSln(const StateVector &aStateVecInit) {
    auto nbPts = aStateVecInit.first->grid().getNoPoints();
    auto maxGrid = aStateVecInit.first->grid().xMax(1);
    auto minGrid = aStateVecInit.first->grid().xMin(1);

    auto w_grid1D =
        std::make_shared<gridLattice>(nbPts, 0, minGrid, maxGrid, 0., 0.);

    // reset pointer
    m_currState.first.reset(
        new scalarField{w_grid1D, std::string{"Mid State A"}});

    m_currState.second.reset(
        new scalarField{w_grid1D, std::string{"Mid State A"}});
  }

  // 2-step integrator that belongs to Runge-Kutta family (second-order)
  void step(double dt);
  void step(SweRhsAlgorithm *aRhsAlgo, const double aDt);

  void setIntegratorStep(const eIntegratorStep aIntegratorStep) {
    m_integratorStep = aIntegratorStep;
  }

  eIntegratorStep getIntegratorStep() const { return m_integratorStep; }

  StateVector getCurrentState() const { return m_currState; }
  StateVector getMidState() const { return m_prevState; }

  void setStep(const double aStep) { m_step = aStep; }
  double getStep() const { return m_step; }

  // ...
  //  void predictor(double dt /*, RHS*/);
  //  void corrector(double dt /*, RHS*/);

  // Since C++17 "Copy Elison is Mandatory"
  //(unmaterialize to "xvalue" eXpiring)
  //  solnU12 getSolutionU12() const { return m_soln; }

protected:
  // can be overrided by subclass
  virtual void predictor(SweRhsAlgorithm::SWERHS &&aRhs, double aDt);
  virtual void corrector(SweRhsAlgorithm::SWERHS &&aRhs, double aDt);

private:
  double m_step;
  eIntegratorStep m_integratorStep;
  // SweRhs m_rhs; made of valarray
  StateVector m_prevState;
  //  StateVector m_finalState;
  StateVector m_currState;
  //  std::vector<double> U1; //**< initial cnd*/
  //  std::vector<double> U2; //**< */

  //  std::vector<double> m_vH;  //**< water level*/
  //  std::vector<double> m_U1p; //**< imtermediate state */
  //  std::vector<double> m_U2p; //**< */
};
} // namespace dbpp
