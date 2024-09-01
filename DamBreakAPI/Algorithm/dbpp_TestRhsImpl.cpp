// C++ includes
#include <algorithm>
#include <cassert>
#include <functional>
// Package includes
#include "../Numerics/dbpp_TestCalculFF.h"
#include "dbpp_TestRhsImpl.h"
// src package include
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../Discretization/dbpp_ReconstrUtility.h"
#include "../SfxTypes/dbpp_DamBreakData.h"

namespace dbpp {
// This version will help in the transition from taking all these steps
// (algorithm) out of the HLL_RK2_Algo advance method. Reconstruction
// procedure is done by a separate function, this way we isolate the
// extrapolation algorithm from the algorithm.
//
//  Question: boundary condition has been applied on StateVector?
//  in this version we still use EMcNeil1D class in which with std
//  vector and boundary applied to U1,U2 and H. "StateVector"
//
//  Usage:
//    Create algorithm
//    SweRhsAlgorithm* w_rhs = new TestRhsImpl();
//    w_rhs->setH(aScalarField); // set water level H for this step
//    w_rhs->calculate(aStateVec);
//    predictor();
//    w_rhs->calculate(aStateVec);
//    corrector();
//
//    Implement the E.McNeil calculFF() version
//
void TestRhsImpl::calculate(const StateVector &aU) {
  using namespace std;
  using namespace std::placeholders;

  // sanity check (debugging purpose)
  assert(aU.first->values().size() == 100);

  // computational domain
  const auto &w_U1 = aU.first->values();
  const auto &w_U2 = aU.second->values();

  // +++++++++++++++++++++++++++++++++++++++++
  // When i do that i just resize or size the vector with default value (yeap!)
  vector<pair<real, real>> w_U1LR; // A
  w_U1LR.reserve(w_U1.size());
  vector<pair<real, real>> w_U2LR; // Q
  w_U2LR.reserve(w_U2.size());

  // for passing to the right hand side numerical treatment
  vector<real> w_vU1(w_U1.getPtr(), w_U1.getPtr() + w_U1.size());
  vector<real> w_vU2(w_U2.getPtr(), w_U2.getPtr() + w_U2.size());

  // apply B.C. upstream
  w_vU1[0] = get<0>(m_bcnodeAM); // A
  w_vU2[0] = get<1>(m_bcnodeAM); // Q

  // need to do a push_back because the scalar field hold computational
  // node only. In the reconstruction process, in the case under study,
  // a ghost node is used (added at the end of the grid but not part of
  // the computational domain, extrapolate)

  // apply B.C. downstream
  w_vU1.push_back(get<0>(m_bcnodeAV)); // A
  w_vU2.push_back(get<1>(m_bcnodeAV)); // Q

  // reconstruct state or node variables at cell face (MUSCL-type extrapolation)
  // Base class method used instead of the static class ReconstrUtil
  // Can be a unit test since it is essentially
  ReconstrUtil::reconstr_vec(w_vU1, w_vU2, w_U1LR, w_U2LR);

  // Jean B (August 2024)
  // All the lines above coul be replaced by these
  // b.c. and all other should be handled by the reconstruction algo
  // i mean its user responsibilities to implement the way he wants it
  // API expose signature with scalarField and other types, not stl!!
  // Encapsulate reponsibilities avoid cross dependencies
  // MusclReonstruction w_muslcl;
  // call ReconstrUtil::reconstr_vec if user has decided to implement
  // w_muslcl.reconstr(aU.first,aU.second,GlobalDiscretization, cellFaceVar);
  // +++++++++++++++++++++++++++++++++++++++++++++++++

  // good practice no memory leak
  std::unique_ptr<BaseNumTreatmemt> w_Bnum(new BaseNumTreatmemt);

  // for debugging purpose (temporary fix) shall be set by the simulation
  // i am no sure about that
  DamBreakData w_dbDat(DamBreakData::DiscrTypes::emcneil);
  // bathymetry
  // const std::vector<double>& w_Z = w_dbDat.getIC().m_Z;
  m_vH.reserve(w_vU1.size());

  // because of the signature of the method i don't have much choice
  std::vector<double> w_vDU1(w_dbDat.nbSections());
  std::vector<double> w_vDU2(w_dbDat.nbSections());

  // in the current implementation we suppose that
  // the size is the number of grid node (memory allocation)
  if (m_swerhs.size() == 0) {
    // this is very confusing about size, because
    // flux act on cell face and source on grid node.
    // Since size is not identical make it confusing.
    m_swerhs.resize(w_U1.size());
  }

  // Riemann solver to compute the flux at cell face
  w_Bnum->CalculFF(m_swerhs.m_FF1, m_swerhs.m_FF2, w_vU1, w_vU2, w_vDU1, w_vDU2,
                   w_dbDat.nbSections(), w_dbDat.getWidth());

  // now compute the water level (suppose to be updated at the end of the time
  // loop) ready for the next time step. Predictor and corrector use the same H
  // 		std::transform( w_vU1.cbegin(), w_vU1.cend(), w_Z.cbegin(),
  // std::back_inserter(m_vH), 			std::bind(
  // &HydroUtils::Evaluation_H_fonction_A,_1,1.,_2));

  // source term discretization (this is the original signature of the function)
  w_Bnum->TraitementTermeSource2(m_swerhs.m_S, w_vU2, w_vU1, m_vH, // ranges
                                 w_dbDat.getManning(), w_dbDat.dx(),
                                 w_dbDat.nbSections(), w_dbDat.getWidth());

  cout << "Exiting of the SweRhsAlgorithm::calculate method\n";
}

// use implementation of the IFluxAlgoImpl (no more, using BaseNumTreatment)
// actually BaseNumTreatment is just a component ... to be completed
// Also using the new implementation of the reconstruction algorithm based
// on scalar field instead of std::vector
void TestRhsImpl::calculate(const StateVector &aU,
                            BaseNumTreatmemt *aBaseTreatment) {
  using namespace std;
  using namespace std::placeholders;

  // sanity check (debugging purpose)
  assert(aU.first->values().size() == 100);

  auto w_vU1 = aU.first->values().to_stdVector();  // A
  auto w_vU2 = aU.second->values().to_stdVector(); // Q

  // boundary condition to applied at both end
  Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  w_bc.applyBC();
  const auto &w_bcUpstream = w_bc.getBCNodeAmont();
  const auto &w_bcDownstream = w_bc.getBCNodeAval();

  // apply B.C. upstream
  w_vU1[0] = get<0>(w_bcUpstream.Values()); // A
  w_vU2[0] = get<1>(w_bcUpstream.Values()); // Q

  // need to do a push_back because the scalar field hold computational
  // node only. In the reconstruction process, in the case under study,
  // a ghost node is used (added at the end of the grid but not part of
  // the computational domain, extrapolate)

  // apply B.C. downstream
  w_vU1.push_back(get<0>(w_bcDownstream.Values())); // A
  w_vU2.push_back(get<1>(w_bcDownstream.Values())); // Q

  // for debugging purpose (temporary fix) shall be set by the simulation
  // i am no sure about that or read from an xml file with the property
  // tree from boost
  DamBreakData w_dbDat(DamBreakData::DiscrTypes::emcneil);

  // use the default implementation of the Base class
  std::vector<double> w_vDU1(w_dbDat.nbSections());
  std::vector<double> w_vDU2(w_dbDat.nbSections());
  // set memory allocation
  m_swerhs.resize(w_dbDat.nbSections());
  m_swerhs.resize(w_dbDat.nbSections());
  // compute flux at cell face according to Riemann problem
  aBaseTreatment->CalculFF(m_swerhs.m_FF1, m_swerhs.m_FF2, w_vU1, w_vU2, w_vDU1,
                           w_vDU2, w_dbDat.nbSections(), w_dbDat.getWidth());

  // bathymetry
  const std::vector<double> &w_Z = w_dbDat.getIC().m_Z;
  m_vH.reserve(w_vU1.size());

  // now compute the water level
  std::transform(w_vU1.cbegin(), w_vU1.cend(), w_Z.cbegin(),
                 std::back_inserter(m_vH),
                 std::bind(&HydroUtils::Evaluation_H_fonction_A, _1, 1., _2));

  // source term discretization (this is the original signature of the function)
  aBaseTreatment->TraitementTermeSource2(
      m_swerhs.m_S, w_vU2, w_vU1, m_vH, // ranges
      w_dbDat.getManning(), w_dbDat.dx(), w_dbDat.nbSections(),
      w_dbDat.getWidth());

  auto w_msg = "Exiting of the SweRhsAlgorithm::calculate() method\n";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));

  cout << "Exiting of the SweRhsAlgorithm::calculate method\n";
}

// (A,Q,H) format
void TestRhsImpl::setBCNodes(
    const std::tuple<double, double, double> &aBcnodeAM,
    const std::tuple<double, double, double> &aBcnodeAV) {
  // setting b.c. node upstream and downstream
  m_bcnodeAM = aBcnodeAM;
  m_bcnodeAV = aBcnodeAV;
}

// Remark:
//   Water level is set at the beginning of the time step,
//   not updated at mid state level, use same value
//   updated at the end of the time step to be ready for
//   next time step.
//
void TestRhsImpl::setH(const dbpp::scalarField &aA) {
  using namespace std;
  using namespace std::placeholders;

  // create vector of A values
  vector<double> w_vU1(aA.values().getPtr(),
                       aA.values().getPtr() + aA.values().size());

  // (initialize with default value 0)
  vector<real> z(aA.values().size() + 1); // NbSections in original code

  if (!all_of(z.cbegin(), z.cend(), // range
              bind(equal_to<real>(), _1, 0.)) == true) {
    // fill it with zero value (make sure it's zero initialized)
    fill(z.begin(), z.end(), 0.);
  }

  // retrieve current discretization parameters
  DamBreakData w_McnilDat(DamBreakData::DiscrTypes::emcneil);
  double w_sectionWidth = 1.; // as default value
  if (!w_McnilDat.isUnitSectionWidth()) {
    // force to be unit width
    w_sectionWidth = 1.;
  }

  // what we do here?
  if (m_vH.size() == 0) {
    // apply B.C. upstream
    w_vU1[0] = get<0>(m_bcnodeAM); // A
    // apply B.C. downstream (actually we add the ghost node)
    w_vU1.push_back(get<0>(m_bcnodeAV)); // A

    // just to make sure
    assert(w_vU1.size() == z.size());

    // size the vector and apply boundary cnd at the end
    m_vH.resize(aA.values().size() + 1);
  }

  // H=A-Z (water depth) computation
  std::transform(
      w_vU1.begin(), w_vU1.end(), z.begin(), m_vH.begin(),
      bind(&HydroUtils::Evaluation_H_fonction_A, _1, w_sectionWidth, _2));

  // apply bc to H make sure we have fix
  m_vH[0] = get<2>(m_bcnodeAM);
  m_vH[aA.values().size()] = get<2>(m_bcnodeAV); // supposed to be at i=100
}
} // namespace dbpp
