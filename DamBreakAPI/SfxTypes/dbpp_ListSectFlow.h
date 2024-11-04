#pragma once

// C++ includes
#include <iostream>
#include <memory> // shared_ptr
// stl includes
#include <algorithm>
#include <vector>
// package includes
#include "../SfxTypes/dbpp_Observer.h"

namespace dbpp {
/** Brief Model of hydraulic cross-section.
 *    Represent the cross section (solve the st-Venant equations).
 *    It has the hydraulic properties such radius, wetted perimeter
 *    that define ... to be completed
 *
 *    Design Note
 */
class SectFlow // boost 'operator' library such as
{
  // equality_comparable, less_than_comparable
public:
  // for now support only these 2 types
  enum class SectionType { unit = 0, rectangular = 1 };

  // default ctor (if we want to use it with STL container)
  SectFlow();
  // ...
  SectFlow(unsigned aId, double aX, SectionType aType = SectionType::unit);
  SectFlow(unsigned aId, double aX, double aH,
           SectionType aType = SectionType::unit);
  // section parameters or hydraulic parameters*/
  unsigned m_id;      /**< section number id*/
  double m_x;         /**< x-coordinate*/
  double m_Z;         /**< bottom thalweg or topography*/
  double m_H;         /**< unit width, otherwise an array*/
  double m_N;         /**< Manning coefficient*/
  double m_B;         /**< section width*/
  double m_S0am;      /**< some parameters from E. McNeil code (slope)*/
  double m_S0av;      /**< some parameters from E. McNeil code (slope)*/
  SectionType m_type; /**< section geometry type*/

  /**
   *  @return type of cross-section (geometry)
   */
  SectionType getSectionType() const { return SectionType::unit; }
  /**
   * @return true if given cross-section is unit width
   */
  bool isUnitWidth() const { return true; }
  /**
   * @return true if given cross-section has no Manning coefficient
   */
  bool isFrictionLess() const { return true; }
  /**
   * @return true if given cross-section is flat (unheaven bottom)
   */
  bool isFlatBottom() const { return true; }
  /**
   * @return x-coordinate of the section (along flow direction)
   */
  double X() const { return m_x; }
  /**
   * @return z-coordinate up direction
   */
  double Z() const { return m_Z; }
  /**
   * @return water level
   */
  double H() const { return m_H; }
  /**
   * @return Manning coefficient
   */
  double N() const { return m_N; }
  /**
   * @return cross-section width
   */
  double B() const { return m_B; }
  /**
   * @return slope upstream
   */
  double getS0am() const { return m_S0am; }
  /**
   * @return slope downstream
   */
  double getS0av() const { return m_S0av; }
  unsigned getId() const { return m_id; }
  /**
   * @return section type (unit, rectangular, parabolic)
   */
  SectionType getType() const { return m_type; }

  /**  water level for this section
   * @param z-coordinate up direction
   */
  void setH(double aH) { m_H = aH; }

  /** print some of the section parameters for debugging purpose
   *
   */
  void printSectionInfo() const {
    // print some of the section parameters for debugging purpose
    std::cout << "Section id :" << m_id << " "
              << "X: " << m_x << " "
              << "Z: " << m_Z << " "
              << "H: " << m_H << " "
              << "N: " << m_N << " "
              << "B: " << m_B << " "
              << "S0am: " << m_S0am << " "
              << "S0av: " << m_S0av << "\n";
  }
};

// forward declaration
class EMcNeil1D;
// class SemiDiscreteModel;

// don't need an const_iterator, because it is read-only, need to re-work
// this need to be inside the class (more test to do)
//	typedef boost::iterator_range<std::list<SectFlow*>::const_iterator>
// ListIterator;
// some test types (just experimenting some concept)
// class IITereable { virtual ListIterator getListIterator() const = 0;};
// class Observer { virtual void update()=0;};

// Usage:
//   This concept of forward list implement traversing from top to bottom
//   Global instance or sole instance of this class since
//   it can't be any more than one list of section flow ON.
//  Iterate through the collection with out knowing
//   ListSectFlow w_test;
// 	BOOST_FOREACH(ListSectFlow::ListIterator::reference w_sect,
// w_test.getEnumerator())
// 	{
// 		w_sect.setH(12);
// 		//std::cout << str << "\n";
// 	}
// 	Design Note:

// TODO: this class needs a serious refactoring
//  Observer pattern i a big mess

/** Brief List of all section flow for the simulation.
 *   The list of section is an observer (design pattern) and implemented
 *   as a vector STL-like (can be used with most of algorithm) by providing
 *   methods that returns iterator.
 *
 *   Implemented as a pointer-container for reference semantic.
 *   Design Note:
 *      Shall use the shared_ptr type to make sure the clean-up is done
 *      (it will be implemented in the next version of the application)
 *
 * Usage:
 *   This concept of forward list implement traversing from top to bottom
 *   Global instance or sole instance of this class since
 *   it can't be any more than one list of section flow ON.
 *
 *   Below an example of how to use it (iterate through the collection)
 *
 *   Iterate through the collection with out knowing
 *   ListSectFlow w_test;
 * 	BOOST_FOREACH( ListSectFlow::ListIterator::reference w_sect,
 * w_test.getEnumerator())
 * 	{
 * 		w_sect.setH(12);
 * 	}
 */
class ListSectFlow : public Observer {
public:
  // incomplete type (forward declaration)
  /**
   * some typedef for code clarity
   */
  using vec_sizetype =
      std::vector<SectFlow *>::size_type; /**< container size type*/
  using vec_valtype = std::vector<SectFlow *>::value_type; /**< container type*/
  using vec_iter = std::vector<SectFlow *>::iterator; /**< container iterator*/
  using vec_citer =
      std::vector<SectFlow *>::const_iterator; /**< container const iterator*/

  /** some utility method (iterator)
   *
   *  @return iterator (begin/end) of the collection
   */
  vec_iter begin() { return m_listOfSect.begin(); }
  vec_iter end() { return m_listOfSect.end(); }

  // constant
  vec_citer begin() const { return m_listOfSect.cbegin(); }
  vec_citer end() const { return m_listOfSect.cend(); }

public:
  /**
   * default constructor (Ctor from subject)
   */
  ListSectFlow(std::shared_ptr<EMcNeil1D> aSubj,
               size_t aNbSections = 101 /*jb::Wave1DSimulator* aSubj*/);
  /**
   *   destructor
   */
  virtual ~ListSectFlow();

  /** Copy ctor
   *
   */
  ListSectFlow(const ListSectFlow &aOther);

  /** Assignment ctor
   *
   */
  ListSectFlow &operator=(const ListSectFlow &aOther);

  //
  // Move semantic
  //
  // 		ListSectFlow( const ListSectFlow&& aOther)
  // 		{
  // 			// what to do (not completed)
  // 		}

  /** Add the section to the list.
   *  pass by pointer since we want a reference semantic
   * we are not using the boost concept pointer container
   * or even the smart pointer (shared pointer C++11) ??
   * @param aSectF a cross-section to be added
   */
  void add(SectFlow *aSectF);

  /** Look for a given section in the list
   * @return true if given cross-section is found
   */
  bool contains(SectFlow *aSectF2Find) {
    // use the find algorithm, of the stl ()
    if (std::find(m_listOfSect.begin(), m_listOfSect.end(), aSectF2Find) !=
        m_listOfSect.end()) {
      // found something
      return true;
    }
    return false;
  }

  /**
   * default ctor (Ctor from subject)
   */
  SectFlow *operator[](vec_sizetype aIdx) { return m_listOfSect[aIdx]; }

  /**
   * @param aIdx index element
   * @return element at given index
   */
  const SectFlow *operator[](vec_sizetype aIdx) const {
    return m_listOfSect[aIdx];
  }

  /**
   *  @return true if container has no section added
   */
  [[nodiscard]] bool isEmpty() const noexcept { return m_listOfSect.empty(); }
  /**
   *  @return true/false if all sections are unit width
   */
  bool isUnitWidth() const {
    if (std::all_of(m_listOfSect.cbegin(), m_listOfSect.cend(),
                    [](const SectFlow *aSectFlow) {
                      return aSectFlow->isUnitWidth();
                    })) {
      return true;
    }
    return false;
  }
  /**
   *  @return rue/false if all sections are no friction
   */
  bool isFrictionLess() const {
    if (std::all_of(m_listOfSect.cbegin(), m_listOfSect.cend(),
                    [](const SectFlow *aSectFlow) {
                      return aSectFlow->isFrictionLess();
                    })) {
      return true;
    }
    return false;
  }
  /**
   *  @return rue/false if all sections are no friction
   */
  bool isFlatBed() const {
    if (std::all_of(m_listOfSect.cbegin(), m_listOfSect.cend(),
                    [](const SectFlow *aSectFlow) {
                      return aSectFlow->isFlatBottom();
                    })) {
      return true;
    }
    return false;
  }
  /**
   *  @return number of sections
   */
  vec_sizetype size() const { return m_listOfSect.size(); }

  /**
   *  @return vector<SectFlow*> vector of section (read only)
   */
  std::vector<SectFlow *> getList() const { return m_listOfSect; }

  /**
   * @return vector<SectFlow*> reference to vector of section
   */
  std::vector<SectFlow *> &getList() { return m_listOfSect; }

  /** Observer implementation
   *  Update section parameter (water level) in Observer design pattern
   *  should call getState() from Subject
   */
  void update() override;

private:
  // shall use the shared_ptr type to make sure the clean-up is done
  // std::vector<SectFlow*>::size_type m_NbSections; /**< number of sections*/
  std::vector<SectFlow *> m_listOfSect; /**< list of sections*/
  std::shared_ptr<EMcNeil1D> m_subj;    /**< */

  bool deleteIt(); // helper function to clean-up the list
                   //		SemiDiscreteModel* m_modelSubj;
};
// global variable (initialized at the beginning
// of the simulation in the main function).
//	extern ListSectFlow* g_ListSectFlow;
} // namespace dbpp
