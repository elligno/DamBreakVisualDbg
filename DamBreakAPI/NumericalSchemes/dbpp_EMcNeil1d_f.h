
#pragma once

// C++ includes
#include <exception>
#include <memory>
// package includes
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_GlobalDiscretization.h"
// Numerical schemes package
#include "dbpp_HLL_RK2_Algo_Vec.h"

namespace dbpp {
/**
 * Brief Implementation that use the scalar field to implement the
 *predictor/corrector algorithm. Numerical scheme representation that inherit
 *from base class We still use the U1/U2 variables of the base class, because we
 *still using deprecated algorithm which we need to pass as argument
 *std::vector. BaseNumTreatment is deprecated.
 *
 *   Design Note inherit from boost noncopyable to denied access
 *   of copying and assignment. Also is final, cannot be inherited or
 *sub-classed.
 */
class EMcNeil1d_f final : public EMcNeil1D {
  using vecdbl = std::vector<double>;
  /**< make code readable*/
  using fluxVector = std::pair<vecdbl, vecdbl>;
  /**< Face flux of first-variable*/
  using BaseNumPtr = std::unique_ptr<BaseNumTreatmemt>;

public:
  /**
   *  default constructor
   */
  EMcNeil1d_f();
  /**
   * destructor
   */
  ~EMcNeil1d_f() = default;

  // copy and assignment not accessible from client
  /**
   *   copy ctor
   */
  EMcNeil1d_f(const EMcNeil1d_f &) = delete;
  /**
   *   assignment ctor
   */
  EMcNeil1d_f &operator=(const EMcNeil1d_f &) = delete;

  // update all nodes
  void mainLoop(const GlobalDiscretization *aGblDiscr,
                const double aTimeTo) override final {}
  void initialize(const GlobalDiscretization *aGblDiscr,
                  double aTime) override final {}

protected:
  /**
   *   time stepping algorithm
   */
  void timeStep() override;

  /** @Brief algorithm implementation.
   *
   */
  void predictor();
  /** @Brief algorithm implementation.
   *
   */
  void corrector();

  /** @Brief ...
   *
   */
  void
  setInitSln(const StateVector &aU /*, ListSectFlow *aListofSect*/) override;

  /** @Brief set boundary node (vector)
* methods, it's left to the user how to implement
* the time stepping. That's the purpose of the
advance() method.*/
  void setAmont(std::vector<double> &aU1, std::vector<double> &aU2);
  void setAval(std::vector<double> &aU1, std::vector<double> &aU2);

  /** @Brief Set the boundary condition (deprecated).
   *    Originally
   */
  void setBC(/*std::vector<double>& aU1,std::vector<double>& aU2*/) {
    GlobalDiscretization *w_EmcNilBC = GlobalDiscretization::instance();
    Gamma &w_bc = w_EmcNilBC->gamma();

    m_amontBC = w_bc.getBCNodeAmont();
    m_avalBC = w_bc.getBCNodeAval();
  }

private:
  /** time stepping algorithm based on two-steps*/
  void predictor(const fluxVector &aFluxVec);
  void corrector(const fluxVector &aFluxVec);

  /** B.C. condition*/
  Nodal_Value m_amontBC;
  Nodal_Value m_avalBC;
  /** deprecated but want to remove them from base class*/
  //	std::vector<double> m_dU1;  /**< gradient of state variable*/
  //	std::vector<double> m_dU2;  /**< gradient of state variable*/
  //	std::vector<double> m_U1p;  /**< intermediate state variable*/
  //	std::vector<double> m_U2p;  /**< intermediate state variable*/
  //	std::vector<double> m_FF1;  /**< Face flux of first-variable*/
  //	std::vector<double> m_FF2;  /**< Face flux of second-variable*/
  //		size_t m_NbSections;	      /**< Number of cross-sectional*/
  std::vector<double> m_S; /**< Source term*/

  //			std::vector<double>  U1, U2; /**< state variables*/
  //		std::vector<double> H;      /**< water level*/
  // temporary variable (not sure about that)
  StateVector m_U12;  /**< state vector that hold sate variables (U1,U2)*/
  StateVector m_U12p; /**< state vector for mid time step*/
  ListSectFlow *m_listSections; /**<List of sections flow*/
  void setH(vecdbl &aVecofH);
};
} // namespace dbpp
