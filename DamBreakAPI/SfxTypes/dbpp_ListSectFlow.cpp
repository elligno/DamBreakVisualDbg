
// Application includes
#include "dbpp_ListSectFlow.h"
#include "dbpp_Wave1DSimulator.h" // that strange
// Util package includes
#include "../Utility/dbpp_TestLogger.h"

namespace dbpp {
SectFlow::SectFlow()
    : m_id(1), // section number id 1 as default, can be confusing
      m_x(0.), m_Z(0.), m_H(0.), m_N(0.), m_B(1.), // unit section width
      m_S0am(0.), m_S0av(0.), m_type(SectionType::unit)
//  m_state()
{
  // something to do
}

// DEPRECATED
SectFlow::SectFlow(unsigned aId, double aX, SectionType aType /*=unit*/)
    : m_id(aId), m_x(aX), m_Z(0.), // default value
      m_H(0.), m_N(0.),            // default value
      m_B(1.),                     // unit section width
      m_S0am(0.), m_S0av(0.), m_type(aType)
//	m_state()
{
  // something to do
}

SectFlow::SectFlow(unsigned aId, double aX, double aH,
                   SectionType aType /*=unit*/)
    : m_id(aId), m_x(aX), m_Z(0.), // default value (flat bed as default)
      m_H(aH), m_N(0.),            // default value (frictionless as default)
      m_B(1.),                     // default value (unit section width)
      m_S0am(0.),                  // default value
      m_S0av(0.),                  // default value
      m_type(aType)
//	m_state()
{
  // something to do
}
// =================================
//       ListSectionFlow
// =================================

ListSectFlow::ListSectFlow(size_t aNbSections /*=101*/) {
  m_listOfSect.reserve(aNbSections);
}

ListSectFlow::ListSectFlow(const ListSectFlow &aOther) {

  m_listOfSect.reserve(aOther.size()); // list of sections
  // create the list
  std::copy(aOther.begin(), aOther.end(), m_listOfSect.begin());
}

dbpp::ListSectFlow &ListSectFlow::operator=(const ListSectFlow &aOther) {
  // check for self-assignment
  if (this == &aOther) {
    return *this;
  }
  //	 m_NbSections=0;
  if (!m_listOfSect.empty()) {
    deleteIt();
  }
  m_listOfSect.reserve(aOther.size()); // list of sections
  // create the list
  std::copy(aOther.begin(), aOther.end(), m_listOfSect.begin());

  return *this;
}

// not completed, case where size!=0 i call clear,
// i don't know if i should do it
bool ListSectFlow::deleteIt() {
  if (!m_listOfSect.empty()) {
    // might as well to delete it
    auto w_begList = m_listOfSect.begin();
    while (w_begList != m_listOfSect.end()) {
      SectFlow *w_sect2del = *w_begList;
      delete w_sect2del;
      w_sect2del = nullptr;
      ++w_begList;
    }

    // sanity check (check size, actually size still the same
    // i just deleted the pointer but did not remove them from the list)
    std::vector<SectFlow *>::size_type w_size = m_listOfSect.size();
    if (w_size != 0) {
      m_listOfSect.clear(); // ???
      // check if size is zero
      if (m_listOfSect.size() != 0) {
        return false;
      }
    } // if
  }   // if

  return true;
}

void ListSectFlow::add(SectFlow *aSectF) {
  // check for capacity and and size
  m_listOfSect.push_back(aSectF);
}

ListSectFlow::~ListSectFlow() {
  if (!m_listOfSect.empty()) {
    // remove element from the list
    deleteIt();
  }
}
} // namespace dbpp
