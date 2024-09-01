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
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../SfxTypes/dbpp_Observer.h"

namespace dbpp {
/** Brief Model of a solver that find the solution for the
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

  /** this is new version of our new designed algorithm
   * may be the H value shall computed with hydro util, i don't see
   * we need to pass it as an argument in the advance time stepping
   * @param aU state vector representing state variables
   */
  virtual void advance();

  /**
   *  Observer design pattern (attaching an observer)
   */
  // Observer Design Pattern interface
  void attach(Observer *aObs); // add to the list of Observer

  // TODO: don't forget to add a unit test
  /**
   *  Observer design pattern (detaching an observer)
   */
  void detach(Observer *aObs);

  /**
   * call by setState() and go through the list of all observer and call update
   */
  void notify();

  /** this shall be implemented by subclass, no need to call it from
   * base class algorithm, shall be handled by subclass, actually
   * call at the end of advance() inherited by subclass
   * set it as virtual but in the next version it should be removed
   */
  virtual void setState();

  /**
   * @return state vector
   */
  virtual StateVector getState() const {
    return StateVector{nullptr, nullptr}; /*return m_U12;*/
  }

  /** @Brief Initial solution or profile
   *   set initial solution (must be virtual, can be re-implemented by derived
   * class)
   */
  virtual void setInitSln(const StateVector &aU,
                          ListSectFlow *aListofSect = nullptr) = 0;

protected:
  /** advance the algorithm step-by-step (default implementation)
   *   Design Note
   *     Next version will be abstract class, no default implementation
   */
  virtual void timeStep() = 0;

  std::list<Observer *> m_listofObs; /**< list of observer*/

  // temporary variable (not sure about that)
  //	StateVector m_U12;                 /**< state vector that hold sate
  // variables (U1,U2)*/
  //		StateVector m_U12p;              /**< state vector for mid time
  // step*/

private:
  // shall be part of a separate class with the following signature
  // checkGdiscrWithVec(vector<double>&, vector<double>&)
  // void checkGdiscrWithVec();
  //		void setGobalDiscrFromVec();

  // double t;              /**< deprecated*/
  // int j, NbSections;     /**< deprecated*/
  //	double B, dx/*, dtc, dtn, dt*/;  /**< deprecated*/
  //	double V, c;                     /**< deprecated*/
  //		unsigned NbIterations;
  // errno_t err;           /**< deprecated*/
  // FILE *FichierResultat; /**< deprecated*/
  // char NomFichierResultat[256]; /**< deprecated*/

  // flag for file status (still using this?)
  bool m_opened; // false is default value
  bool m_dbgOpen;
};
} // namespace dbpp
