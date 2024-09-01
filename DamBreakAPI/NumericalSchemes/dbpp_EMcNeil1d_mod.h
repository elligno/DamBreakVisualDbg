#pragma once

// C++ include
#include <cassert>
// library includes
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_GlobalDiscretization.h" // Global Discretization concept
#include "dbpp_HLL_RK2_Algo_Vec.h"

namespace dbpp {
/** Brief A different implementation of the EMcNeil1D default algorithm.
 * Use the Global Discretization concept to set boundary value.
 * Re-implement advance() method without modifying
 * base class version (Will be used from base pointer). its a model of
 * two-steps time stepping that belongs to Runge-Kutta family.
 */
class EMcNeil1d_mod final : public EMcNeil1D {
  // some usefull typedefs (all these are temporary )
  using vecdbl = std::vector<double>;
  /**< make code readable*/
  using FluxVector = std::pair<vecdbl, vecdbl>;
  /**< Flux and source term*/
  // using RhsStruct = std::pair<FluxVector,vecdbl>;
  /**< Face flux of first-variable*/
  using BaseNumPtr = std::unique_ptr<BaseNumTreatmemt>;

  typedef struct Rhs {
    FluxVector m_fVec; // flux vector
    vecdbl m_S;        // Source term
  } RhsStruct;

public:
  /** default ctor*/
  EMcNeil1d_mod();
  /** dtor*/
  //~EMcNeil1d_mod();

  // copy and assignment ctor not accessible ()
  StateVector getState() const override { return m_currU; }

protected:
  /** physical algorithm (more the numerical algorithm)*/
  void timeStep() override; // solve(could more appropriate)
  // ?? not sure yet         // we are time stepping anything
  void setH();

  /** boundary condition at both end (upstream/downstream)*/
  void setAmont(vecdbl &aU1, vecdbl &aU2);
  void setAval(vecdbl &aU1, vecdbl &aU2);

  // set all parameters for the
  void setInitSln(const StateVector &aU, ListSectFlow *aListofSect) override;

private:
  /** time-stepping algorithm (two-steps of Runge-Kutta family type)*/
  void predictor();
  void corrector();

  // ... to be completed
  void predictor(const RhsStruct &aRhs);
  void corrector(const RhsStruct &aRhs);

  // copy and assignment ctor not accessible
  // since inheriting from boost noncopyable

  // boundary condition node values at both end
  // what happens if
  Nodal_Value m_amontBC; /*< upstream boundary condition*/
  Nodal_Value m_avalBC;  /*< downstream boundary condition*/

  StateVector m_U12;   /**< state vector that hold state variables (U1,U2)*/
  StateVector m_U12p;  /**< state vector for mid time step*/
  StateVector m_currU; /**< state vector for current step*/
  ListSectFlow *m_listSections; /**< List of sections flow*/
  void setH(vecdbl &aVecofH);   /**< ... */

  // i like this concept (could be wrapped in a class for prototyping purpose)
  RhsStruct
  createRhsDiscretization(const StateVector &aU); /**< create POM ... */
};
} // namespace dbpp
