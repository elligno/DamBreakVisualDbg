#pragma once

// stl includes
#include <map>
#include <numeric>
#include <vector>
// package includes
#include "../SfxTypes/dbpp_scalarField.h"
#include "../Utility/dbpp_HydroCodeDef.h"

namespace dbpp {
class GlobalDiscretization;
}
namespace dbpp {
/** Brief Static class provides reconstruction algorithm at cell face.
 *   This class is more a prototype (support for the second-order of
 *   of the state variables at cell face according to MUSCL).
 *   Variables are extrapolated to second-order by means of minmod
 *   procedure (flux limiter)
 *
 *   Reconstruction procedure consist of extrapolation state variables
 *   at cell face at given order. A flux limiter is used to select
 *   or to control gradient to avoid sharp function that can lead to spurious
 *   oscillation that result in non-convergence or stability problem.
 *
 *   In the current implementation we are experimenting different implementation
 *   of this concept (for example using STL algorithm to compute gradient).
 *
 *   Main steps of the algorithm
 *     - need to add an extra node (ghost node)
 *     - gradient ...
 *     - compute face variables given order
 *
 *   4 static methods that takes care of the reconstruction (prototyping)
 *   different implementation of the MUSCL reconstruction but with
 *     - reconstr_vec(...) implementation based on a vector to represent state
 * variable
 *     - reconstr_val(...) implementation based on ...
 *     - reconstr_rng(...) implementation based on ...
 *     - reconstrv_sv(...) implementation based on ...
 */
class ReconstrUtil {
public:
  /** Brief cell face (j+1/2) variables
   *  (shall take a look at stateVariables which is very similar)
   *  Not sure of the concept but i think that i am trying to encapsulate
   *  ... to be completed
   */
  struct cellFaceVariables {
    /**
     * default ctor
     */
    cellFaceVariables(double aUL, double aUR, unsigned aFaceIdx = 1)
        : m_UL{aUL}, m_UR{aUR}, m_faceIdx{aFaceIdx}, m_order{2} {}

    /** default copy and assignment ctor will be fine*/

    double m_UL;        /**< left state cell face reconstructed variable*/
    double m_UR;        /**< right state*/
    double m_FL;        /**< left face flux */
    double m_FR;        /**< right face flux*/
    unsigned m_faceIdx; /**< global face index of the discretization (j+1/2)*/
    unsigned m_order;   /**< reconstruction order */

    /**
     * setter (set cell variable and flux value) is it necessary??
     */
    void setULR(const double aUL, const double aUR) {
      m_UL = aUL;
      m_UR = aUR;
    }
    void setULR(const std::pair<double, double> aULR) {
      m_UL = aULR.first;
      m_UR = aULR.second;
    }
    void setFLR(const double aFL, const double aFR) {
      m_FL = aFL;
      m_FR = aFR;
    }
    void setAsStateVar() { /*not implemented yet*/
    }                      // not sure yet

    /**
     * getter return pair of cell variable (flux and state variable)
     */
    std::pair<double, double> getFLR() const {
      return std::make_pair(m_FL, m_FR);
    }
    std::pair<double, double> getULR() const {
      return std::make_pair(m_UL, m_UR);
    }
  };

  /**
   *  typedef for code clarity
   */
  using mapofacesvar = // A(U1),Q(U2)
      std::map<unsigned, std::pair<cellFaceVariables /*U1LR*/,
                                   cellFaceVariables /*U2LR*/>>;

public:
  /**
   *  Implementation of the reconstruction procedure algorithm. Use different
   *  container such as std::vector as argument,
   */
  static void reconstr_vec(
      const std::vector<double> &aU1,
      const std::vector<double> &aU2,      // field to reconstruct at cell face
      vecULR &aVecU1LR, vecULR &aVecU2LR); // face variable reconstructed

  static void reconstr_val(
      const dbpp::scalarField &aField, // field to reconstruct at cell face
      std::vector<std::pair<double, double>>
          &aVecULR); // face variable reconstructed (UL,UR)

  // vector version of the reconstruction procedure
  static void reconstrv_sv(const StateVector &aStateV, vecULR &aULR1,
                           vecULR &aULR2);

  /** prototyping version of the reconstruction procedure which returns
   * state variables reconstructed at cell face at a given order by MUSCL.
   */
  static mapofacesvar reconstr_j12(const GlobalDiscretization *aGdiscr,
                                   const StateVector &aStateVector);
};
} // namespace dbpp
