#pragma once

// App includes
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../Utility/dbpp_EMcNeilUtils.h"
#include "dbpp_NodalVariables.h"

// namespace dbpp { class ListSectFlow; }
namespace dbpp {
class BCharacteristicSolver {
public:
  // temporary impl. for debug purpose (pass the last section/next section)
  static double computeDownstreamCPCA(
      const Nodal_Value
          &aNval, /*Uh[99],last node and section computational domain*/
      const SectFlow aLastSection /*99*/, double aDt);

  /** Compute velocity along upstream characteristic */
  static double
  computeUpstreamVelocities(const Nodal_Value &aNval /*next downstream*/,
                            const SectFlow aSectFlow0 /*firstt downstream*/,
                            const SectFlow aSectFlow /*next downstream*/,
                            double aDt);

  // that's what we need, to be implemented in the near future
  // void solve(/*upstream,downstream, state variable (A,Q)*/);
};
} // namespace dbpp
