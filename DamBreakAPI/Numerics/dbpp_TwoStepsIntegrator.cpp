
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

  // Although pass an rvalue reference, within the function
  // itself it is treated as an lvalue (cast to rvalue ref)
  const auto w_rhs = std::move(aRhs);

  // initialize numeric array
  const valarray<double> w_valU1(m_currState.first->values().getPtr(),
                                 m_currState.first->grid().getNoPoints());

  const valarray<double> w_valU2(m_currState.second->values().getPtr(),
                                 m_currState.second->grid().getNoPoints());

  // Compute the first-order derivative with backward upwind stencil: F_i-F_i-1
  // NOTE first element is irrelevant and shall be removed, but since
  // first global node is tied (set by b.c.), we just keep it and overwrite
  valarray<double> w_dFF1(m_currState.first->grid().getNoPoints());
  adjacent_difference(std::begin(w_rhs.m_FF1), std::end(w_rhs.m_FF1),
                      std::begin(w_dFF1));

  valarray<double> w_dFF2(m_currState.second->grid().getNoPoints());
  adjacent_difference(std::begin(w_rhs.m_FF2), std::end(w_rhs.m_FF2),
                      std::begin(w_dFF2));

#if _DEBUG
  SFX_REQUIRE(w_dFF2.size() == w_rhs.m_S.size(),
              "Scalar field of different size");
  SFX_REQUIRE(w_valU1.size() == w_rhs.m_S.size(),
              "Scalar field of different size");
#endif

  // NOTE
  //  we didn't bother to compute the state on whole computational
  //  domain. The "adjacent_difference" algorithm keep the first
  //  element unchanged (first node), but this node is a tied node
  //  (fix by physical boundary condition), it will be overwritten.
  const auto w_lambda = aDt / m_currState.first->grid().Delta(1);
  // mass equations
  auto w_valU1p = w_valU1 - w_lambda * w_dFF1;
  // momentum equation (with source term: frictionless and flat bed)
  auto w_valU2p = w_valU2 - w_lambda * w_dFF2 + aDt * w_rhs.m_S;

  // IMPORTANT??? about what?
  Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  const auto &w_bcAmont = w_bc.getBCNodeAmont();

  // GlobalDiscretization::instance()->Uh().front(); show the intent
  // Overwrite the first node (set physical n=boundary cond.)
  if (GlobalDiscretization::instance()->Uh().front().isTiedNode()) {
    // A,Q (overwrite value at boundary node)
    w_valU1p[0] = w_bcAmont[0]; // m_amontBC[0]; A-variable
    w_valU2p[0] = w_bcAmont[1]; // m_amontBC[1]; Q-variable
  }

  // Design Note
  // scalar field with valarray as grid point values instead of copying!!
  // m_prevState.first->values(
  //       new std::valarray<double>(std::begin(w_valU1p), w_valU1p.size()));

  // put it back
  std::copy(std::begin(w_valU1p), std::end(w_valU1p),
            m_prevState.first->values().getPtr());
  std::copy(std::begin(w_valU2p), std::end(w_valU2p),
            m_prevState.second->values().getPtr());
}

void TwoStepsIntegrator::corrector(SweRhsAlgorithm::SWERHS &&aRhs, double aDt) {

  SweRhsAlgorithm::SWERHS w_rhs = std::move(aRhs);

  // Keep backward compatibility with the StateVector which will be deprecated
  //    const SfxType::vectorField w_prevState{ *m_prevState.first,
  //    *m_prevState.second }; const SfxType::vectorField w_currState{
  //    *m_currState.first, *m_currState.second };

  const std::valarray<double> w_valU1p(m_prevState.first->values().getPtr(),
                                       m_prevState.first->grid().getNoPoints());

  const std::valarray<double> w_valU2p(
      m_prevState.second->values().getPtr(),
      m_prevState.second->grid().getNoPoints());

  std::valarray<double> w_valU1(m_currState.first->values().getPtr(),
                                m_currState.first->grid().getNoPoints());

  std::valarray<double> w_valU2(m_currState.second->values().getPtr(),
                                m_currState.second->grid().getNoPoints());

  // Design Note
  //  Important to note that return of the Upwind derivative
  //   Could use the move semantic or perfect forwarding
  // dF_i = (F_i - F_i-1)/dx first-order derivative with backward upwind stencil
  std::valarray<double> w_valdFF1(m_currState.first->grid().getNoPoints());
  std::adjacent_difference(std::begin(w_rhs.m_FF1), std::end(w_rhs.m_FF1),
                           std::begin(w_valdFF1));

  std::valarray<double> w_valdFF2(m_currState.second->grid().getNoPoints());
  std::adjacent_difference(std::begin(w_rhs.m_FF2), std::end(w_rhs.m_FF2),
                           std::begin(w_valdFF2));

  // const std::valarray<double> w_valS(w_rhs.m_S),
  // w_currState.A().grid().getNoPoints());

#if _DEBUG
  SFX_REQUIRE(w_valdFF1.size() == w_rhs.m_S.size(),
              "Scalar field of different size");
  SFX_REQUIRE(w_valU1.size() == w_rhs.m_S.size(),
              "Scalar field of different size");
#endif

  // computational domain (don't consider first node i=1 (tied))
  const auto w_lambda = aDt / m_prevState.first->grid().Delta(1);

  // mass equation
  w_valU1 = 0.5 * (w_valU1 + w_valU1p - w_lambda * w_valdFF1);
  // momentum equation (with source: S_f - S_0) friction slope and bottom
  w_valU2 = 0.5 * (w_valU2 + w_valU2p - w_lambda * w_valdFF2 + aDt * w_rhs.m_S);

  if (GlobalDiscretization::instance()->Uh().front().isTiedNode()) {
    const auto nodeIdx =
        GlobalDiscretization::instance()->Uh().front().node_no();

    // sanity check
    assert(0 == nodeIdx);

    // global discretization in use
    const Gamma &w_bc = GlobalDiscretization::instance()->gamma();

    // set BC
    const Nodal_Value &w_bcAmont = w_bc.getBCNodeAmont();

    // set boundary cnd
    w_valU1[nodeIdx] = w_bcAmont[0]; // A
    w_valU2[nodeIdx] = w_bcAmont[1]; // Q
  }

  // put it back
  std::copy(std::begin(w_valU1), std::end(w_valU1),
            m_currState.first->values().getPtr());
  std::copy(std::begin(w_valU2), std::end(w_valU2),
            m_currState.second->values().getPtr());

  // ready for next step (maybe i should dereference instead of assigning
  // shared_ptr?)
  //     auto shr = *m_prevState.first;
  *(m_prevState.first) = *(m_currState.first);   // copy ctor
  *(m_prevState.second) = *(m_currState.second); // ditto

  // no point to do that, don't need, in the predictor
  // use valarray, then copy valarray in m_prevState
  // m_prevState = m_currState;
}

void TwoStepsIntegrator::step(double dt) { // not sure about this
  m_step = dt;
}

void TwoStepsIntegrator::step(SweRhsAlgorithm *aRhsAlgo, const double aDt) {
  // add code here
  m_step = aDt;

  //
  switch (auto w_stepType = getIntegratorStep(); w_stepType) {
  case eIntegratorStep::predictorStep:
    predictor(aRhsAlgo->getRHS(), aDt); // prvalue
    break;
  case eIntegratorStep::correctorStep:
    corrector(aRhsAlgo->getRHS(), aDt); // prvalue
    break;
  default:
    // log entry!!!
    break;
  }
}
} // namespace dbpp
