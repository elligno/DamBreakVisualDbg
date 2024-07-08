
#pragma once

#include "../SfxTypes/dbpp_scalarField.h"
// #include "PhyConstant.h"
// #include "Simulation.h"
// #include "EMcNeilUtils.h"
// #include "DamBreakData.h"

namespace dbpp {
/** compute square root*/
// struct sqroot_d : std::unary_function<double, double> {
// result_type operator()(const argument_type aType) const {
//  return ::sqrt(aType);
// }
//};
/** absolute value*/
// struct fabs_d : std::unary_function<double, void> {
// result_type operator()(argument_type &aType) const { std::fabs(aType); }
//};

/** Brief Compute the time stepping according to stability criteria.
 *  Different implementation is available based on numerical type of
 *  programming environment.
 */
class TimeStepCriteria {
public:
  // default ctor
  //	TimeStepCriteria() : m_dtn(0.),m_bigNumber(1.) {}

  /** return the time step criteria according to ...*/
  static double timeStep(const dbpp::scalarField &aU1,
                         const dbpp::scalarField &aU2);

  /** return the time step criteria according to ...*/
  //		static double timeStep( const dbpp::RealNumArray<real>& aU1,
  //		const dbpp::RealNumArray<real>& aU2)
  //{
  // return 0.; // debugging purpose
  //}

  /** return the time step criteria according to ...*/
  static double timeStep(const std::vector<double> &aU1,
                         const std::vector<double> &aU2);

private:
  double m_bigNumber;
  double m_dtn;
};
} // namespace dbpp
