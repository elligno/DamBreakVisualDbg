
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

ListSectFlow::ListSectFlow(
    std::shared_ptr<EMcNeil1D> aSubj,
    size_t aNbSections /*=101jb::Wave1DSimulator* aSubj*/)
// : m_NbSections(aNbSections)
{
  // will be removed in future version
  m_subj = aSubj; // equivalent to reset? need to check this
  m_subj->attach(this);

  m_listOfSect.reserve(aNbSections);
}

ListSectFlow::ListSectFlow(const ListSectFlow &aOther) {
  // construct new type from existing one
  // m_NbSections = aOther.m_NbSections;           number of sections

  m_listOfSect.reserve(aOther.size()); // list of sections
  // create the list
  std::copy(aOther.begin(), aOther.end(), m_listOfSect.begin());
  m_subj = aOther.m_subj;
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
  if (m_subj != nullptr) //?? why?
  {
    m_subj = nullptr; // give-up ownership!!!
  }

  m_listOfSect.reserve(aOther.size()); // list of sections
  // create the list
  std::copy(aOther.begin(), aOther.end(), m_listOfSect.begin());

  return *this;
}

//    ListSectFlow::ListSectFlow( SemiDiscreteModel* aSubj, size_t
//    aNbSections/*=101jb::Wave1DSimulator* aSubj*/)
// 	: m_NbSections(aNbSections)
//    {
// 	   m_modelSubj=aSubj;
// 	   m_modelSubj->attach(this);
//
// 	   m_listOfSect.reserve(m_NbSections);
//    }
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
    std::list<SectFlow *>::size_type w_size = m_listOfSect.size();
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
// is it completed (i think what we do here,
// we update sections from 0 to 99, domain of 100 nodes)
// 101 node is the ghost node set by BC
void ListSectFlow::update() {
  // working variable to store A,Q values
// 	   std::vector<double> w_U1;
// 	   w_U1.reserve(m_NbSections); // why -1?
// 	   std::vector<double> w_U2;
// 	   w_U2.reserve(m_NbSections);
#if 0 // EMcNeil1D version in use
     // m_subj->getFinalState(w_U1, w_U2);
#endif

  // A,B,Z
  auto w_beg = m_listOfSect.begin();
  auto w_end = m_listOfSect.cend() - 1;
  unsigned i{};
  const auto w_stateVecAQ = m_subj->getState();
  const auto w_U1 = w_stateVecAQ.first->values().to_stdVector();

  while (w_beg != w_end) // computational domain
  {                      // but we need also the extra section
    SectFlow *w_currSect = *w_beg++;
    w_currSect->setH(HydroUtils::Evaluation_H_fonction_A(
        w_U1[i++], w_currSect->B(), w_currSect->Z()));
  }
  // auto w_lastSect = m_listOfSect.back(); // 101 not part of computational
  // domain
  auto w_msg = "Finished updating Section flow water depth";
  Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));

  // 	   for (SectFlow* w_hVal : m_listOfSect)
  // 	   {
  // 		   std::cout << w_hVal->H() << "\n";
  // 	   }

#if 0
	   // debugging purpose (print all information about a section)
	   std::for_each( m_listOfSect.cbegin(),m_listOfSect.cend(),
		   std::mem_fn( &dbpp::SectFlow::H));

     std::cout << "Finished updating Section flow water depth\n";
#endif
}

} // namespace dbpp
