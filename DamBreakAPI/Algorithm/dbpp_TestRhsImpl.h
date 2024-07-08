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
  // what about the ctor?? good question!!
  // default is ok??
  // from base numerical discretization

  // shall declare a default ctor?? isn't?
  //		~TestRhsImpl()
  //		{
  //			dbpp::Logger::instance()->OutputSuccess("TestRhsImpl
  //dtor");
  //		}

  /** an implementation of the algorithm based on Eric McNeil*/
  void calculate(const StateVector &aU) final override;

  /**Compute right hand side according to base numerical discretization*/
  void calculate(const StateVector &aU,
                 BaseNumTreatmemt *aBaseTreatment) override final;

  /** compute water level for this step of the simulation (just don't get it?)
   * nothing to with water level (deprecated)
   *
   * @param scalar field
   */
  void setH(const dbpp::scalarField &aA) override;

  /** An implementation of the boundary condition (A,Q,H format)
   *  (deprecated but not sure)
   * @param values at both end of the global domain (computaional domain
   * extended)
   */
  void setBCNodes(const std::tuple<double, double, double> &aBcnodeAM,
                  const std::tuple<double, double, double> &aBcnodeAV) override;

private:
  // all that stuff is deprecated (don't make any sense)
  std::vector<double> m_vH; /**< water level*/
  std::tuple<double, double, double>
      m_bcnodeAM; /**< boundary condition at upstream*/
  std::tuple<double, double, double>
      m_bcnodeAV; /**< boundary condition at downstream*/
};
} // namespace dbpp
