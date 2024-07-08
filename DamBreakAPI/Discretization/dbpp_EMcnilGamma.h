#pragma once

// Test include
#include "dbpp_NodalVariables.h"

namespace dbpp {
/** Brief Model of boundary condition ().
 *  we are using the characteristic method to solve St-Venant for
 * boundary condition upstream/downstream (this method is explained
 * in the following document from springer.com).
 * Reference: Modeling Of The Open Channel Flow (chapter #2)
 * google it with "an introduction to st-venant one-dimensional model"
 * Reminder
 *   Curve in the x/t plane (phase space) ... to be completed
 *   Technical report ....
 *   Another, we are considering the supercritical flow (Fr>1)
 *   it will be nice if we could measure during the simulation
 *   the Froude number.
 *   (A,Q,H)
 */
class Gamma {
public:
  enum class eBCtypes { // b.c. H/Q are fixed at both ends
    Hten_Qzero = 0,     // what for? EMCneil b.c. values
    Hone_Qzero = 1,     // Hudson b.c. values
    all_fixed = 2
  };

public:
  /** default ctor (what for??)*/
  Gamma() : m_bctype{eBCtypes::Hten_Qzero} {
    // const Nodal_Value& w_bndAmont = aNval[0];
  }

  //
  void applyBC();
  void applyBC() const;

  /** returns boundary nodes (returning what exactly)
   */
  Nodal_Value getBCNodeAmont() const {
#if 0
    // debugging purpose
    const double w_A=m_amont[0];
    const double w_Q=m_amont[1];
    const double w_H=m_amont[2];
#endif

    return m_amont;
  }
  /**
   */
  Nodal_Value getBCNodeAval() const {
#if 0
    // debugging purpose
    const double w_A=m_aval[0];
    const double w_Q=m_aval[1];
    const double w_H=m_aval[2];
#endif

    return m_aval;
  }

  /**
   */
  eBCtypes getBCtype() const { return m_bctype; }

protected:
  /* for now i don't give a shit about how it is implemented
   * all we want its to have something going on and test concept
   * */
  virtual void setAmont();
  virtual void setAval();

private:
  /** boundary condition (default implementation)
   * fill nodal_value with b.c. value computed according
   * to the active boundary type for the simulation running.
   */
  // virtual void getBCAmont(Nodal_Value &aNvalbnd) const;
  // virtual void getBCAval(Nodal_Value &aNvalbnd) const;

  // Design Note shall use the nodal constraint type
  // for now we sue
  Nodal_Value m_aval;  /**< */
  Nodal_Value m_amont; /**< */

  // Design Note this is a temporary fix,
  // i am not sure about this implementation (passing a pointer)
  // but at least need some kind of reference on nodal values (U)
  // void initialization( const boost::ptr_vector<Nodal_Value>* aUh);
  eBCtypes m_bctype;
}; // Gamma
} // namespace dbpp
