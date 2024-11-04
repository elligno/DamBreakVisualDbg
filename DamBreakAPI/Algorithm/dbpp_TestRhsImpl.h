#pragma once

// Package include
#include "dbpp_SweRhsAlgorithm.h"

namespace dbpp {
/** Brief This is a prototype of how to  implement RHS algorithm concept (under
 * construction). Responsible to evaluate the RHS of the ODE, discretization of
 * the spatial terms. Time stepping will be done outside of the method calculate
 * (it fill the structure SWERHS and return it).
 *
 *   Usage
 *    SweRhsAlgorithm* w_rhsAlgo = new TestRhsImpl;
 *    need to manipulate from base pointer.
 */
class TestRhsImpl : public SweRhsAlgorithm {
public:
  // Ctor
  TestRhsImpl(ListSectFlow *aListSectFlow) : m_listSectFlow{aListSectFlow} {}

  /** an implementation of the algorithm based on Eric McNeil*/
  void calculate(const StateVector &aU) final override;

  /**Compute right hand side according to base numerical discretization*/
  void calculate(const StateVector &aU, // shall pass the PhysicalSystem!!!
                 const GlobalDiscretization *aGblDiscr) override final;

  /** An implementation of the boundary condition (A,Q,H format)
   *  (deprecated but not sure)
   * @param values at both end of the global domain (computaional domain
   * extended)
   */
  void setBCNodes(const std::tuple<double, double, double> &aBcnodeAM,
                  const std::tuple<double, double, double> &aBcnodeAV) override;

  /** to be completed*/
  ListSectFlow *getListPhysicsObjects() const { return m_listSectFlow; }

private:
  ListSectFlow *m_listSectFlow;
  // all that stuff is deprecated (don't make any sense)
  // std::vector<double> m_vH; /**< water level*/
  std::tuple<double, double, double>
      m_bcnodeAM; /**< boundary condition at upstream*/
  std::tuple<double, double, double>
      m_bcnodeAV; /**< boundary condition at downstream*/
};
} // namespace dbpp
