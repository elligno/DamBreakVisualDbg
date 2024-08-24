#pragma once

#include "dbpp_MathEquations.h"

namespace dbpp {
/** @brief St-Venant 1D Equations (mass and momentum) in conservative form
 */
class StVenant1DEq : public SCLEquation {
  enum class eFluxType {
    // ...
  };

public:
  StVenant1DEq() = default;
  StVenant1DEq(const std::string &aName);

  // copy and assignment ctor (not sure about this one)
  StVenant1DEq(const StVenant1DEq &) = delete;
  StVenant1DEq &operator=(const StVenant1DEq &) = delete;
  /**
   * @brief flux
   * @param aStateVec
   * @return
   */
  double flux(const StateVector &aStateVec) override;
  /**
   * @brief getDimension
   * @return pressure at grid node
   */
  unsigned getDimension() const override final { return 1; }
  /**
   * @brief getNumberOfEquations
   * @return pressure at grid node
   */
  unsigned getNumberOfEquations() const override final { return 2; }

private:
  /**
   * @brief CalculTermePressionHydrostatique1D
   * @param A state variable
   * @param B cross-section width at free surface
   * @return pressure at grid node
   */
  double CalculTermePressionHydrostatique1D(double A, double B = 1.);
  /**
   * @brief EvaluationF2_C_1D incomplet flux without pressure term
   * @param U1 state variable
   * @param U2 state variable
   * @param B cross-section width at free surface
   * @return flux value
   */
  double EvaluationF2_C_1D(double U1, double U2, double B = 1.);
  /**
   * @brief EvaluationF2_I_1D incomplet flux without pressure term
   * @param U1 state variable
   * @param U2 state variable
   * @param B cross-section width at free surface
   * @return flux value
   */
  double EvaluationF2_I_1D(double U1, double U2, double B = 1.);
};
} // namespace dbpp
