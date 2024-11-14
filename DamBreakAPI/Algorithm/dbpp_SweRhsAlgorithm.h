#pragma once

// STL includes
#include <list>
#include <tuple>
#include <valarray>
// project includes
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../Utility/dbpp_HydroCodeDef.h"
#include "../Utility/dbpp_TestLogger.h"

// forward declaration
namespace dbpp {
class BaseNumTreatmemt;
}

namespace dbpp {
/** Brief Model of the right-hand-side discretization.
 * Class that represent the spatial terms
 * numerical evaluation (depending if we consider)
 * all terms of the st-Venant equations
 * ... to be completed
 * Usage
 *   ----reconstr algorithm
 *   ----compute flux algorithm
 *   ----compute source terms discretization
 *   Can have a vector of algorithm that we could want
 *   to test and compare between each other
 *   std::vector<>
 *   make this class with private copy and assignment ctor
 */
class SweRhsAlgorithm {
public:
  /**
   * some typedef for clarity
   */
  using vecdbl = std::vector<double>; /**< vector of double*/
  using bcvalues =
      std::tuple<double, double, double>; /**< tuple hold bc values*/
public:
  // struct that hold ... numerical of each term
  /** Brief struct that hold ... numerical of each term.
   *   Based on STL container and provides similar interface
   *   Container variables for the rhs terms.
   */
#if 0
    struct SWERHS {
    using tensorField =
        std::vector<std::pair<double, double>>;  /**< represent a tensor field*/
    using value_type = double;                   /**< double value*/
    using vector_type = std::vector<value_type>; /**< vector of value*/
    using iter_type = std::vector<value_type>::iterator; /**< iterator */

    // ... to be completed
    // 			SWERHS() // default is E. McNeil validation
    // 			: m_Pterm(false),
    // 			  m_Sterm(true),
    // 			  m_FcTerm(true)
    // 			{}

    /**
     * default is E. McNeil validation
     */
    explicit SWERHS(size_t aSize = 101) // default value
        : m_Pterm(false), m_Sterm(true), m_FcTerm(true), m_capacity(0),
          m_Size(static_cast<unsigned int>(aSize)) {
      // 				m_FF1.reserve(aSize-1);  cell face
      // (NbSections-1) 				m_FF2.reserve(aSize-1);
      // cell face (NbSections-1)
      // m_S.reserve(aSize);      global grid node (NbSections)
    }

    /**
     * @param aNb number of grid point
     */
    void reserve(const unsigned aNb) {
      if (m_capacity != 0) {
        m_capacity = 0; // force to zero
      }
      m_capacity = aNb;

      m_FF1.reserve(m_capacity);   // cell face (NbSections-1)
      m_FF2.reserve(m_capacity);   // cell face (NbSections-1)
      m_S.reserve(m_capacity + 1); // global grid node (NbSections)
    }

    /**
     * @param aNb number of grid point
     */
    void resize(const unsigned aNb) {
      if (m_Size != 0) {
        m_Size = 0; // force to zero
      }
      m_Size = aNb;
      m_FF1.resize(m_Size);   // cell face (NbSections-1)
      m_FF2.resize(m_Size);   // cell face (NbSections-1)
      m_S.resize(m_Size + 1); // global grid node (NbSections)
    }
    /**
     * @return aNb number of grid point
     */
    unsigned size() const { return m_Size; }
    /**
     * @return memory allocation
     */
    unsigned capacity() const { return m_capacity; }

    // some methods (not completed but main idea is that)
    /**
     * @return pressure term status
     */
    bool hasPressureTerm() const { return m_Pterm; } // default value
    /**
     * @return source term status
     */
    bool hasSourceTerm() const { return m_Sterm; } // default value
    /**
     * @return convective term status
     */
    bool hasConvectiveTerm() const { return m_FcTerm; } // default value

    // data store
    std::vector<double> m_FF1; /**< first component of the flux */
    std::vector<double> m_FF2; /**< second component of the flux */
    tensorField m_tensorFlux;  /**< tensor flux */
    std::vector<double> m_S;   /**< source term */
    bool m_Pterm;              /**< pressure term*/
    bool m_Sterm;              /**< source term*/
    bool m_FcTerm;             /**< convective flux term */
    unsigned m_capacity;       /**< containers capacity*/
    unsigned m_Size;           /**< containers size*/
  };
#endif
  /** Brief struct that hold ... numerical of each term.
   *   Based on STL container and provides similar interface
   *   Container variables for the rhs terms.
   */
  struct SWERHS {
    std::valarray<double> m_FF1;
    std::valarray<double> m_FF2;
    std::valarray<double> m_S;
  };

protected:
  // structure to hold computed information
  SWERHS m_swerhs; /**< right-hand-side term*/
public:
  // do we need a default ctor? if we want to  use it with stl container we
  // surely want a default ctor
  /** default ctor
   */
  SweRhsAlgorithm() : m_name{"SweRhsAlgorithm"} {
    dbpp::Logger::instance()->OutputSuccess(
        std::string{"SweRhsAlgorithm default ctor"}.data());
  }

  /** algorithm for the spatial term (discretization)
   * @param state vector
   */
  virtual void calculate(const StateVector &aU) = 0;
  virtual void calculate(const StateVector &aU,
                         const GlobalDiscretization *aGblDiscr) = 0;

  /** read/write access
   */
  SWERHS getRHS() const { return m_swerhs; }
  // const SWERHS& getRHS() const { return m_swerhs;} compile-time error, don't
  // understand!!

  /** method returns info about this algorithm
   */
  std::string name() const { return m_name; }

  /** set water level at the beginning
   */
  // virtual void setH(const dbpp::scalarField &aA) = 0;

  /** NOTE: 'H' is computed once at the beginning of the time step
   *       in the intermediate state H is not updated
   * boundary condition values at upstream and downstream nodes
   * A,Q,H variable order in tuple argument
   *Usage:
   *   setBC(std::make_tuple(A,Q,H),
   *         std::make_tuple());
   */
  virtual void setBCNodes(const bcvalues &aBcnodeAM,
                          const bcvalues &aBcnodeAV) = 0;

protected:
  ListSectFlow *m_listSectionFlow{nullptr};

private:
  std::string m_name; /*< rhs algorithm name*/
};
} // namespace dbpp
