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
    cellFaceVariables() : m_UL(0.), m_UR(0.), m_faceIdx(0), m_order(2) {}

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
  typedef std::map<unsigned, std::pair<cellFaceVariables /*U1LR*/,
                                       cellFaceVariables /*U2LR*/>>
      mapofacesvar;

public:
  // 		 ReconstrUtil();
  // 		~ReconstrUtil();
  // Will be removed in the next version of this file, move in a
  // separate class which implement different version of the reconstruction
  // algorithm.
  // below 3 version of the same function but with different implementation
  // i would like to compare for efficiency between these functions. I suspect
  // that the valarray version will be more efficient since

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

  static void reconstr_rng(
      const dbpp::scalarField &aField, // field to reconstruct at cell face
      std::vector<std::pair<double, double>>
          &aVecULR); // face variable reconstructed

  // vector version of the reconstruction procedure
  static void reconstrv_sv(const StateVector &aStateV, vecULR &aULR1,
                           vecULR &aULR2);

  /** prototyping version of the reconstruction procedure which returns
   * state variables reconstructed at cell face at a given order by MUSCL.
   */
  static std::map<unsigned, vecULR>
  reconstr_j12(const GlobalDiscretization *aGdiscr,
               const dbpp::scalarField &aStateVec);
  static mapofacesvar reconstr_j12(const GlobalDiscretization *aGdiscr,
                                   const StateVector &aStateVector);

  /** this will be removed, because we are using the min function of the std.
   * MinMod function do more than taking the minimum, it test for some ...
   * slope limiter function to limit slope of the solution.
   */
  static void computeMinModDU(const std::vector<real> &aVecU,
                              std::vector<double> &aDU);

  /** gradient (this algorithm need to be verified and validated)
   * there is a lot of comment, but now it's clear in my mind.
   */
  template <typename Range> // compute first order, not sure but i think so
  static Range computeDU(const Range &aRng) {
    // NOTE: this algorithm is totally wrong, don't need to remove
    // the first element after adjacent_diffrence, i have made some
    // test in the "TestCpp11" project see file "TestSTLAlgorithm.cpp"
    // and the algorithm doesn't add anything to the container, first
    // element of the adjacent difference is the first element for which
    // we are taking or applying the algorithm. This first element need to
    // be overwrite with boundary value (dU[0]=U[1]-U[0]). For a test
    // implementation see file "TestScientificAlgo.cpp" of TestCpp11.cpp".
    // Don't need to use deque container because of pop_front() method.
    // Vector will do!!

    // compute the gradient at first-order using the stl numeric algorithm
    // computedU(Cont aCont,unsigned aOrder)
    Range w_tmpdUi(aRng.size()); // i am not sure about this one
    // w_vDU.resize(w_vecU.size()); // NbSections=101, but return 100
    std::adjacent_difference(aRng.begin(), aRng.end(),
                             std::back_inserter(w_tmpdUi));

    // this is not true, it's wrong!!
    //  because first element is set to 0 by the algo to compute the first diff.
    // 			typedef iterator_range<typename Range::iterator> cont_irange;
    // // value that iterator point to
    // 			// first and second element ()
    // 			cont_irange w_tmpRng = boost::make_iterator_range(
    // aRng.begin(), aRng.begin()+1);
    // boost::range_value<cont_irange>::type w_u0 =
    // *(w_tmpRng.begin()); 			boost::range_value<cont_irange>::type
    // w_u1
    // =
    // *(w_tmpRng.advance_begin(1)); 			w_tmpdUi[0]=w_u1-w_u0; //
    // overwrite the first element
    //
    // 			// sanity check
    // 			assert(aRng.size()==w_tmpdUi.size());

    // returning the range of gradient computed
    // skip first element because it correspond to first element
    // of the original range (adjacent algorithm work that way)
    return Range(w_tmpdUi.begin() + 1, w_tmpdUi.end());
  }

  template <typename Range>
  Range applyLimiter(const Range &aRofDU) /*, something that i want to try
         std::function<const typename Range::value_type& (typename const
         Range::value_type&, typename const Range::value_type&)> aLimiter) */
  {
    // when i do that, adjacent diff. will push element
    // and growth size?
    // IMPORTANT:
    Range w_minDU(aRofDU.size() + 1);

    // (just applying the minmod function to some range)
    std::adjacent_difference(
        aRofDU.begin(), aRofDU.end(), w_minDU.begin(),
        std::bind(&std::min<int>, std::placeholders::_1,
                  std::placeholders::_2)); // for testing purpose

    // treating the boundary node i=0 and i=100 in Eric McNeil code
    // we overwrite the first element since according to the adjacent
    // difference algorithm, it's ... to be completed
    w_minDU[0] =
        std::min<Range::value_type>(aRofDU[1] - aRofDU[0], 0); // overwrite
    // last element that ...
    //	w_minDU.push_back( 0, min(aRofDU[aR2apply.size()-1] -
    // aRofDU[aR2apply.size()-2]));
    w_minDU[aRofDU.size()] = std::min<Range::value_type>(
        0, aRofDU[aRofDU.size() - 1] - aRofDU[aRofDU.size() - 2]);

    // returns the new range
    return Range(w_minDU.begin(), w_minDU.end());
  }
};
} // namespace dbpp
