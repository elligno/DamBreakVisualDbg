#pragma once

// stl includes
#include <algorithm>
#include <list>
#include <tuple>
#include <vector>
// Util package includes
#include "../Utility/dbpp_EMcNeilUtils.h"
#include "../Utility/dbpp_HydroCodeDef.h"
// SfxTypes package include
//#include "../SfxTypes/dbpp_DamBreakData.h"
//#include "../SfxTypes/dbpp_Observer.h"
#include "../SfxTypes/dbpp_TimePrm.h"

namespace dbpp { // forward declaration
class GlobalDiscretization;
}

// We have a similar implementation of this in the EMcNeil1D
// library (exported)
// Design Note:
//   This is a representation of the Godunov-type scheme
//   where a Riemann problem is used to compute the flux
//   at cell interface. dependency with global discretization,
//   for now we just use the grid lattice as argument to ctor.
//   In this version it's more a concept that we wold like to
//   experiment. ... to be completed
//   Method that we expect from a Godunov-Type Scheme are the
//   following but not restricted to:
//     -- advance() evolveable
//     -- calculFF() numerical flux
//     -- ... more coming it's a first draft
//   Design Note
//   -----------
//     "Model of" solver that find the solution for the St-Venant
//     equations in a finite volume formulation. More coming
//

namespace dbpp {
/** @brief Model of a solver that find the solution for the
          St-Venant equations in a finite volume formulation.
*
* Create a physical algorithm based on E. McNeil source code.
* This is a representation of the Godunov-type scheme
* where a Riemann problem is used to compute the flux
* at cell interface (based on Godunov-type algorithm
* and use a reconstruction of the state variable at cell face).
* Order is achieved by this technique of piecewise-constant
* state variable on each cell, an extrapolation procedure is
* done at a given order. Then a Riemann solver is used to compute
* numerical flux at cell face (representation of a Riemann problem).
* This system of conservation law coupled with 2-state tied by
* a discontinuity define a Riemann Problem. This algorithm solve
* at cell-face a Riemann problem.
* E. McNeil has chosen a solver HLL (Harten-Lax-Levy) more well
* suited for problem with torrential flow.
* emcil::EMcNeil1D w_emcnil;
*/
class EMcNeil1D {
public:
  /**
   * default ctor
   */
  EMcNeil1D();

  /**
   *  dtor (shall be protected and non-virtual, next version)
   */
  virtual ~EMcNeil1D() = default;

  /** @brief Advance the algorithm step-by-step (default implementation)
   *  this is new version of our new designed algorithm
   * may be the H value shall computed with hydro util, i don't see
   * we need to pass it as an argument in the advance time stepping
   * @param aU state vector representing state variables
   */
  virtual void advance();

  /** @brief Advance the algorithm step-by-step (default implementation)*/
  virtual void mainLoop(const GlobalDiscretization *aGblDiscr,
                        const double aTimeTo) = 0;

  /** @brief to be completed*/
  virtual void initialize(const GlobalDiscretization *aGblDiscr,
                          double aTime) = 0;

  /** this shall be implemented by subclass, no need to call it from
   * base class algorithm, shall be handled by subclass.
   */
  virtual void setState();

  /**
   * @return state vector
   */
  virtual StateVector getState() const {
    return StateVector{nullptr, nullptr}; /*return m_U12;*/
  }

  /** @brief Initial solution or profile
   *   set initial solution (must be virtual, can be re-implemented by derived
   * class)
   */
  virtual void setInitSln(const StateVector &aU) = 0;

protected:
  /** advance the algorithm step-by-step (default implementation)
   *   Design Note
   *     Next version will be abstract class, no default implementation
   */
  virtual void timeStep() = 0;
};
} // namespace dbpp
