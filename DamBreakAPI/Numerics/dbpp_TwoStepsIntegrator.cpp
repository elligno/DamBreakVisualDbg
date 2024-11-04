
// Numeric include
#include <numeric> //adjacent_difference
// Library includes
#include "SfxTypes/VectorField.h"
#include "SfxTypes/dbpp_BaseMacros.h"
#include "SfxTypes/dbpp_ExceptionUtils.h"
#include "dbpp_TwoStepsIntegrator.h"

namespace dbpp {

void TwoStepsIntegrator::predictor(SweRhsAlgorithm::SWERHS &&aRhs, double aDt) {
  using namespace std;

  // although pass an rvalue reference, within the function
  // itself it is treated as an lvalue (cast to rvalue ref)
  const auto w_rhs = std::move(aRhs);

  // Keep backward compatibility with the StateVector which will be deprecated
  const vectorField w_currState{*m_currState.first, *m_currState.second};

  const valarray<double> w_valU1(w_currState.A().values().getPtr(),
                                 w_currState.A().grid().getNoPoints());

  const valarray<double> w_valU2(w_currState.Q().values().getPtr(),
                                 w_currState.Q().grid().getNoPoints());

  // first-order derivative with backward upwind stencil: F_i-F_i-1
  // NOTE first element is irrelevant and shall be removed, but since
  // first global node is tied (set by b.c.), we just keep it and overwrite
  valarray<double> w_dFF1(w_currState.A().grid().getNoPoints());
  adjacent_difference(std::begin(w_rhs.m_FF1), std::end(w_rhs.m_FF1),
                      std::begin(w_dFF1));

  valarray<double> w_dFF2(w_currState.Q().grid().getNoPoints());
  adjacent_difference(std::begin(w_rhs.m_FF2), std::end(w_rhs.m_FF2),
                      std::begin(w_dFF2));

#if _DEBUG
  SFX_REQUIRE(w_dFF2.size() == w_rhs.m_S.size(),
              "Scalar field of different size");
  SFX_REQUIRE(w_valU1.size() == w_rhs.m_S.size(),
              "Scalar field of different size");
#endif

  const auto w_lambda = aDt / w_currState.A().grid().Delta(1);

  // mass equations
  auto w_valU1p = w_valU1 - w_lambda * w_dFF1;
  // momentum equation (with source term: frictionless and flat bed)
  auto w_valU2p = w_valU2 - w_lambda * w_dFF2 + aDt * w_rhs.m_S;

  // IMPORTANT??? about what?
  Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  const Nodal_Value &w_bcAmont = w_bc.getBCNodeAmont();

  if (GlobalDiscretization::instance()->Uh()[0].isTiedNode()) {
    // A,Q (overwrite value at boundary node)
    w_valU1p[0] = w_bcAmont[0]; // m_amontBC[0];
    w_valU2p[0] = w_bcAmont[1]; // m_amontBC[1];
  }
  // sanity check
  if (GlobalDiscretization::instance()->Uh()[100].isTiedNode()) // open boundary
  {
    std::cout << "this node is part of the global domain, not of math "
                 "equations domain\n";
  }

  // put it back
  std::copy(std::begin(w_valU1p), std::end(w_valU1p),
            m_prevState.first->values().getPtr());
  std::copy(std::begin(w_valU2p), std::end(w_valU2p),
            m_prevState.second->values().getPtr());
}

void TwoStepsIntegrator::corrector(SweRhsAlgorithm::SWERHS &&aRhs, double aDt) {
  // add code here
}

void TwoStepsIntegrator::step(double dt) { // not sure about this
  m_step = dt;
}

void TwoStepsIntegrator::step(SweRhsAlgorithm *aRhsAlgo, const double aDt) {
  // add code here
  m_step = aDt;
}

} // namespace dbpp
