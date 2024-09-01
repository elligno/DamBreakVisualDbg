
#pragma once

#include "../SfxTypes/dbpp_scalarField.h"

namespace dbpp {

/** Brief Compute the time stepping according to stability criteria.
 *  Different implementation is available based on numerical type of
 *  programming environment.
 */
class TimeStepCriteria {
public:
  /** return the time step criteria according to ...*/
  static double timeStep(const dbpp::scalarField &aU1,
                         const dbpp::scalarField &aU2);

private:
  double m_bigNumber;
  double m_dtn;
};
} // namespace dbpp
