
#pragma once

// package includes
#include "../Algorithm/dbpp_SweRhsAlgorithm.h"
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_AppConstant.hpp"

namespace dbpp {

/** @brief to be completed
 */
class BaseRhsImpl : public SweRhsAlgorithm {

  using vecdbl = std::vector<double>;

public:
  BaseRhsImpl(); // to be defined

  /** @brief Boundary condition values at upstream and downstream nodes
   *          A,Q,H variable order in tuple argument
   *  Usage:
   *   setBC(std::make_tuple(A,Q,H),
   *         std::make_tuple());
   */
  virtual void setBCNodes(const bcvalues &aBcnodeAM,
                          const bcvalues &aBcnodeAV) override {
    // no implementation
  }

  /** @brief Algorithm for the spatial term (discretization)
   * @param aU state vector (state var)
   */
  virtual void calculate(const StateVector &aU) override { /*no impleentation*/
  }

  /** @brief Algorithm for the spatial term (discretization)
   * @param state vector
   * @param Global discretization
   */
  virtual void calculate(const StateVector &aU,
                         const GlobalDiscretization *aGblDiscr) override {

    //    std::vector<double> w_Avec;
    //    w_Avec.reserve(DIM::value);
    //    aGblDiscr->to_stdVector(w_Avec); // default
    //    std::vector<double> w_Qvec;
    //    w_Qvec.reserve(DIM::value);
    //    aGblDiscr->to_stdVector(w_Avec,
    //    GlobalDiscretization::NodalValComp::Q);

    // auto w_discrType = Simulation::instance()->getActiveDiscretization();
    auto w_dbData = Simulation::instance()->getActiveDBdata();

    //    scalarField w_A{
    //        std::make_shared<gridLattice>(w_dbData.toString(w_discrType)),
    //        w_Avec, std::string{"A-state var"}};

    //    scalarField w_Q{
    //        std::make_shared<gridLattice>(w_dbData.toString(w_discrType)),
    //        w_Qvec, std::string{"Q-state var"}};

    auto w_Avec = aU.first->values().to_stdVector();
    auto w_Qvec = aU.second->values().to_stdVector();

    const auto &w_bc = aGblDiscr->gamma();
    // upstream b.c.values
    auto [A0, Q0, H0] = w_bc.getBCNodeAmont().Values(); // tuple-like
    auto [AN, QN, HN] = w_bc.getBCNodeAval().Values();  // tuple-like
    w_Avec.push_back(AN);
    w_Qvec.push_back(QN);

    // not sure about this one (node indexed from 1 to N)
    // const int nx = m_grid->getMaxI(1); number of points in first dimension
    auto w_lastNodeIdx = aU.first->grid().getMaxI(1);
    auto firstIdx = aU.first->grid().getBase(1); // yes, and its 0
    assert(0 == firstIdx); // scalar field use valarray store data, base = 0

    // Uh are global nodal values
    const auto &w_Uval = GlobalDiscretization::instance()->Uh();

    // setting boundary condition
    if (w_Uval[firstIdx].isTiedNode()) // check first node
    {
      // set first node
      w_Avec[firstIdx] = A0;
      w_Qvec[firstIdx] = Q0;
    }

    // global discr include the ghost node,
    if (w_Uval[w_lastNodeIdx + 1].isTiedNode() &&
        w_Uval[w_lastNodeIdx + 1].isGhostNode()) {
      // adding ghost node
      w_Avec.push_back(AN);
      w_Qvec.push_back(QN);
    }

    // Algorithm component based E. McNeil (Physical Algorithm)
    // POM (Physics Object Model)
    std::unique_ptr<BaseNumTreatmemt> w_numTermsAlgo(new BaseNumTreatmemt);

    // flux component at cell face
    vecdbl w_FF1;
    vecdbl w_FF2;
    w_FF1.reserve(aU.first->values().size());
    w_FF2.reserve(aU.second->values().size());

    // Riemann solver of type HLL (use MUSCL reconstruction order 2)
    // Compute face flux (cell face)
    w_numTermsAlgo->CalculFF(w_FF1, w_FF2, w_Avec, w_Qvec);
    //
    std::copy(w_FF1.cbegin(), w_FF1.cend(), std::begin(m_swerhs.m_FF1));
    std::copy(w_FF2.cbegin(), w_FF2.cend(), std::begin(m_swerhs.m_FF2));

    // retrieve vector of H values of the global discretization
    vecdbl w_H;
    w_H.reserve(GlobalDiscretization::instance()->Hvalues().size());
    GlobalDiscretization::instance()->to_stdVector(
        w_H, GlobalDiscretization::NodalValComp::H);

    w_H[firstIdx] = H0;
    w_H[DIM::value + 1] = HN;

    // Physical algorithm (source term are discretized by a ... to be completed)
    vecdbl w_n(DIM::value); // initialize to 0 as default (frictionless case)
    vecdbl w_S(DIM::value); // ditto (Sf=0 and S0 = derivative)

    // Source terms algorithm (friction and pressure)
    w_numTermsAlgo->TraitementTermeSource2(w_S, w_Qvec, w_Avec, w_H, w_n,
                                           aU.first->grid().Delta(1),
                                           DIM::value); // computational domain

    std::copy(w_S.cbegin(), w_S.cend(), std::begin(m_swerhs.m_S));
  }
};
} // namespace dbpp
