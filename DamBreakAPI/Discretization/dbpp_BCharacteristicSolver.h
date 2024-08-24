#pragma once

// App includes
#include "dbpp_NodalVariables.h"
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../Utility/dbpp_EMcNeilUtils.h"

//namespace dbpp { class ListSectFlow; }
namespace dbpp 
{
  class BCharacteristicSolver 
  {
  public:
    // temporary impl. for debug purpose (pass the last section/next section)
    static double computeDownstreamValues( const Nodal_Value& aNval, /*Uh[99],last node and section computational domain*/
      const SectFlow aLastSection/*99*/, double aDt);

    static double computeUpstreamValues( const Nodal_Value& aNval/*next downstream*/, 
      const SectFlow aSectFlow0/*firstt downstream*/, const SectFlow aSectFlow /*next downstream*/, double aDt);

    // that's what we need, to be implemented in the near future
    //void solve(/*upstream,downstream, state variable (A,Q)*/);
  };
} // End of namespace
