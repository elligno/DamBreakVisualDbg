#pragma once

// App includes
#include "../Discretization/dbpp_EMcnilGamma.h"
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../Utility/dbpp_EMcNeilUtils.h" //HydroUtils
#include "dbpp_BCharacteristicSolver.h"

namespace dbpp {
// DESIGN NOTE
//   This is the first draft of the B.C. refactoring. Separate
//   the characteristic computation from setting boundary node value.
//
class EMcNeilBCimpl {
public:
  // default is E. McNeil b.c. value (phi1=10/phi0=1)
  // Created at the initialization phase just after
  // list of sections have been created (reference semantic with smart-ptr)
  EMcNeilBCimpl(std::shared_ptr<ListSectFlow> aListSectFlow)
      : m_bcType{Gamma::eBCtypes::Hten_Qzero}, m_listSectFlow{aListSectFlow} {}

  // Usage
  //
  EMcNeilBCimpl(ListSectFlow *aListSectFlow)
      : m_bcType{Gamma::eBCtypes::Hten_Qzero}, m_listSectFlow{aListSectFlow} {}

  // Amont
  void setUpstream(const Nodal_Value &aNval, double aDt) {
    auto w_nextSectFront = m_listSectFlow->getList()[1];
    auto w_amontValues = getUpstreamValues(
        aNval, m_listSectFlow->getList().front(), w_nextSectFront, aDt);
  }

  // Aval
  void setDownstream(/*const Nodal_Value& aNval, double aDt*/) {
    // create ghost section from last section (computational domain)
    SectFlow w_ghostSectFlow{*m_listSectFlow->getList().back()}; // copy ctor
    if (w_ghostSectFlow.X() == m_listSectFlow->getList().back()->X()) {
      // do something
      w_ghostSectFlow.m_id = m_listSectFlow->getList().back()->m_id + 1;
      const auto SectXcoord =
          m_listSectFlow->getList()[m_listSectFlow->getList().back()->m_id - 1];
      const auto w_dx = m_listSectFlow->getList().back()->X() - SectXcoord->X();
      if (m_bcType == Gamma::eBCtypes::Hten_Qzero) {
        assert(10. == w_dx); // E. McNeil values
      }
      w_ghostSectFlow.m_x =
          m_listSectFlow->getList().back()->X() + w_dx; // debugging purpose
      // shall be same since we did a copy ctor (sanity check)
      if (w_ghostSectFlow.m_H != m_listSectFlow->getList().back()->H()) {
        w_ghostSectFlow.m_H = m_listSectFlow->getList().back()->H();
      }
      if (w_ghostSectFlow.m_N != m_listSectFlow->getList().back()->N()) {
        w_ghostSectFlow.m_N = m_listSectFlow->getList().back()->N();
      }
      if (w_ghostSectFlow.m_Z != m_listSectFlow->getList().back()->Z()) {
        w_ghostSectFlow.m_Z = m_listSectFlow->getList().back()->Z();
      }
    }
  }

protected:
  // Usage
  //  Create a ghost section
  Nodal_Value getDownstreamValues(
      const Nodal_Value
          &aNval, /*Uh[99],last node and section computational domain*/
      const SectFlow *aLastSection /*99*/,
      const SectFlow *aGhostSection /*100*/, double aDt);

  Nodal_Value getUpstreamValues(
      const Nodal_Value
          &aNval, /*Uh[99],last node and section computational domain*/
      const SectFlow *aFirstSection, const SectFlow *aNextSection, double aDt);

private:
  Gamma::eBCtypes m_bcType;
  std::shared_ptr<ListSectFlow> m_listSectFlow; // unique_ptr??
};
} // namespace dbpp
