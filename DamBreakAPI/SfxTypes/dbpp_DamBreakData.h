
#pragma once

// C++ includes
#include <string>
#include <utility>
#include <vector>

namespace dbpp {
// similar to the gridLattice concept developed before
/** Brief Hold different parameters of the DamBreak problem.
 *   These are used through the simulation
 *   spatial discretization parameters (it needs to be part of the dam-break
 * data?)
 *
 *   Design Note
 *     Inherit from boost utility to make copy/assignment ctor not accessible
 *     Hence user cannot copy or assign why that?
 *     Data is unique for each simulation (only one instance per simulation)
 *     It may change in the next version
 */
class DamBreakData //: boost::noncopyable
{
  typedef int int32;
  typedef double float64;

  /**
   * Structure that hold initial condition for teh simualtion
   */
  struct IC {
    std::vector<float64> m_U1; /**<A (wetted area) */
    std::vector<float64> m_U2; /**Q (discharge)*/
    std::vector<float64> m_H;  /**H (water level */
    std::vector<float64> m_Z;  /**Z (bathymetry)*/
    std::vector<float64> m_N;  /**  (Manning)*/
  } m_IC;

public:
  typedef std::pair<float64, float64> pairofloat64;

  /**
   * discretization type
   */
  enum class DiscrTypes {
    emcneil = 0, /**  (Manning)*/
    hudson = 1   /**  (Manning)*/
  };

public:
  // default ctor (default parameters)
  DamBreakData(DamBreakData::DiscrTypes aDiscrType =
                   DiscrTypes::hudson); // these are default parameters when
                                        // validating (Hudson)
  DamBreakData(int32 aNx, const std::pair<float64, float64>
                              aVal = // default value of the domain extent
                          std::make_pair(0., 1.));

  // spatial discretization parameters (it need to be part of the dam-break
  // data?) it is more the spatial discretization
  /**
   * @result x coordinate left boundary
   */
  float64 x_min() const { return m_Xmin; }
  /**
   * @result x coordinate right boundary
   */
  float64 x_max() const { return m_Xmax; }
  /**
   * @result x coordinate
   */
  std::vector<float64> X() const { return m_X; }
  /**
   * @result slope upstream
   */
  float64 getS0am() const { return m_S0am; }
  /**
   * @result slope downstream
   */
  float64 getS0av() const { return m_S0av; }
  /**
   * @result Courant-Friedrich-Levy
   */
  float64 getCFL() const { return m_cfl; }
  /**
   * @result spatial stepping
   */
  float64 dx() const { return m_dX; }
  /**
   * @result numeber of cross-sectional
   */
  int32 nbSections() const { return m_NbSects; }
  /**
   * @result initial water level downstream
   */
  float64 getPhi0() const { return m_Phi0; }
  /**
   * @result initial water level upstream
   */
  float64 getPhi1() const { return m_Phi1; }
  /**
   * @result cross-section width
   */
  float64 getWidth() const { return m_sectWidth; }
  /**
   * @result
   */
  pairofloat64 domainExtent() const { return m_domainExtent; }
  pairofloat64 dambreakExtent() const { return m_dambreakExtent; }
  DiscrTypes getDType() const { return m_discrType; }
  bool isUnitSectionWidth() const { return m_unitSectWidth; }
  bool isFrictionLess() const { return m_frictionLess; }
  bool isFlatBed() const { return m_flatBed; }
  std::vector<double> getManning() const { return m_IC.m_N; }
  std::vector<double> getBathy() const { return m_IC.m_Z; }
  const IC &getIC() const { return m_IC; }

  /** returning a string description of the discretization
   * Notation (for example E. McNeil):
   *    xmin=1
   *    xmax=1000
   *    i=1
   *    i=100
   */
  std::string toString(DiscrTypes aDisc) {
    if (aDisc == DiscrTypes::emcneil) {
      return std::string("d=1 [0,1000] [1:100]");
    } else {
      return std::string("d=1 [0,1] [1:10]");
    }
  }
  /** read parameters from a file (user defined)*/
  //  void readFromFile(std::string aFileName /* = */) { /*not implemented yet*/
  // }

private:
  std::pair<float64, float64> m_domainExtent;   // [min,max]
  std::pair<float64, float64> m_dambreakExtent; // []
  std::vector<float64> m_X;                     /**< ... */
  float64 m_S0am;                               /**< ...*/
  float64 m_S0av;                               /**< ...*/
  float64 m_dX;                                 /**< spatial step */
  float64 m_Phi0;                               /**< water level at upstream*/
  float64 m_Phi1;                               /**< water level at downstream*/
  float64 m_cfl;                /**< Courant-Friedrich-Levy number*/
  int32 m_NbSects;              /**< number of computational nodes*/
  float64 m_Xmin;               /**< x-coordinate of the left bound. node*/
  float64 m_Xmax;               /**< x-coordinate of the right bound. node*/
  float64 m_sectWidth;          /**< section flow width*/
  bool m_unitSectWidth;         /**< Section width*/
  bool m_frictionLess;          /**< no-friction (Manning coefficient is zero)*/
  std::string m_resultFilename; /**< result filename*/

  /** initialize discretization parameters */
  void discrInit();
  DiscrTypes m_discrType; /**< ...*/
  bool m_flatBed;         /**< bottom topography flag*/
  /**
   *  set initial condition data
   */
  void setIC();
};
} // namespace dbpp
