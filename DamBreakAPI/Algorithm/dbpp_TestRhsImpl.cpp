// C++ includes
#include <algorithm>
#include <cassert>
#include <functional>
// Package includes
#include "../Numerics/dbpp_TestCalculFF.h"
#include "../Numerics/dbpp_TwoStepsIntegrator.h"
#include "dbpp_TestRhsImpl.h"
// lib API package include
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../Discretization/dbpp_ReconstrUtility.h"
#include "../Numerics/dbpp_SourceTermsAlgorithms.h"
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../Utility/dbpp_AppConstant.hpp"

namespace dbpp {
// This version will help in the transition from taking all these steps
// (algorithm) out of the HLL_RK2_Algo advance method. Reconstruction
// procedure is done by a separate function, this way we isolate the
// extrapolation algorithm from the algorithm.
//
void TestRhsImpl::calculate(const StateVector &aU) {
  using namespace std;
  using namespace std::placeholders;

  // sanity check (debugging purpose)
  assert(aU.first->values().size() == 100);
  assert(aU.first->values().size() == static_cast<int>(m_listSectFlow->size()));

  // computational domain (retrieve values)
  const auto &w_U1 = aU.first->values();
  const auto &w_U2 = aU.second->values();

  // +++++++++++++++++++++++++++++++++++++++++
  // When i do that i just resize or size the vector with default value (yeap!)
  // reserve memory to store data, size=0 since no elements
  vector<pair<double, double>>
      w_U1LR; // A (reconstructed variable at cell face)
  w_U1LR.reserve(w_U1.size());
  vector<pair<double, double>> w_U2LR; // Q (ditto)
  w_U2LR.reserve(w_U2.size());

  // for passing to the right hand side numerical treatment
  // for now we use legacy code which take as argument vector
  // we need to create temporary vector and set ...
  vector<double> w_vU1(w_U1.getPtr(), w_U1.getPtr() + w_U1.size());
  vector<double> w_vU2(w_U2.getPtr(), w_U2.getPtr() + w_U2.size());

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

  // for debugging purpose (temporary fix) shall be set by the simulation
  DamBreakData w_dbDat(DamBreakData::DiscrTypes::emcneil);

  std::vector<double> m_vH; /**< water level*/
  m_vH.reserve(m_listSectFlow->size());

  // now compute the water level (suppose to be updated at the end of the time
  // loop) ready for the next time step. Predictor and corrector use the same H

  //  std::transform(w_vU1.cbegin(), w_vU1.cend(), w_dbDat.getIC().m_Z.cbegin(),
  //                 std::back_inserter(m_vH),
  //                 std::bind(&HydroUtils::Evaluation_H_fonction_A, _1, 1.,
  //                 _2));

  // retireve H (water level from section flow)
  using value_type = ListSectFlow::vec_valtype;
  std::transform(m_listSectFlow->begin(), m_listSectFlow->end(),
                 std::back_inserter(m_vH),
                 [](const value_type aSectFlow) { return aSectFlow->H(); });

  // reconstruct state variables at cell face (MUSCL-type extrapolation)
  // Base class method used instead of the static class ReconstrUtil
  ReconstrUtil::reconstr_vec(w_vU1, w_vU2, w_U1LR, w_U2LR);

  // Jean B (August 2024)
  // Encapsulate reponsibilities avoid cross dependencies
  // MusclReonstruction w_muslcl;
  // call ReconstrUtil::reconstr_vec if user has decided to implement
  // return extrapolated (reconstructed) variables at cell face j+1/2
  // cellFaceVar = w_muslcl.reconstr(aU.first,aU.second,GlobalDiscretization);
  // +++++++++++++++++++++++++++++++++++++++++++++++++

  // compute cell face flux (HLL algorithm)
  TestCalculFF w_fluxFaceAlgo;

  // no choice, refactoring is needed ()
  std::vector<double> w_FF1(aU.first->values().size());
  std::vector<double> w_FF2(aU.second->values().size());

  // Riemann solver to compute the flux at cell face
  w_fluxFaceAlgo.calculFF(w_FF1, w_FF2, w_U1LR, w_U2LR);

  // force reset to zero (valarray)
  m_swerhs.m_FF1.resize(w_FF1.size());
  m_swerhs.m_FF2.resize(w_FF2.size());

  // for now leave it like that, backward compatibility
  std::copy(w_FF1.cbegin(), w_FF1.cend(), std::begin(m_swerhs.m_FF1));
  std::copy(w_FF2.cbegin(), w_FF2.cend(), std::begin(m_swerhs.m_FF2));

  // good practice no memory leak
  std::unique_ptr<BaseNumTreatmemt> w_Bnum(new BaseNumTreatmemt);
  // source term discretization (this is the original signature of the function)
  std::vector<double> w_S(aU.first->values().size());
  w_Bnum->TraitementTermeSource2(
      w_S, w_vU2, w_vU1, m_vH, // ranges
      w_dbDat.getManning(), aU.first->grid().Delta(1),
      static_cast<const int>(m_listSectFlow->size()), w_dbDat.getWidth());
  // force reset to zero (valarray)
  m_swerhs.m_S.resize(w_S.size());

  std::copy(w_S.cbegin(), w_S.cend(), std::begin(m_swerhs.m_S));

  cout << "Exiting of the SweRhsAlgorithm::calculate method\n";
}

// use implementation of the IFluxAlgoImpl (no more, using BaseNumTreatment)
// actually BaseNumTreatment is just a component ... to be completed
// Also using the new implementation of the reconstruction algorithm based
// on scalar field instead of std::vector
void TestRhsImpl::calculate(const StateVector &aU,
                            const GlobalDiscretization *aGblDiscr) {
  using namespace std;
  using namespace std::placeholders;

  // sanity check (debugging purpose)
  assert(aU.first->values().size() == 100);

  auto w_vU1 = aU.first->values().to_stdVector();  // A
  auto w_vU2 = aU.second->values().to_stdVector(); // Q

  // boundary condition to applied at both end
  const Gamma &w_bc = aGblDiscr->gamma();
  w_bc.applyBC(); // not sure??
  const auto [Am, Qm, Hm] = w_bc.getBCNodeAmont().Values();
  const auto [Av, Qv, Hv] = w_bc.getBCNodeAval().Values();

  // apply B.C. upstream
  w_vU1[0] = Am; // get<0>(w_bcUpstream.Values()); // A
  w_vU2[0] = Qm; // get<1>(w_bcUpstream.Values()); // Q

  // need to do a push_back because the scalar field hold computational
  // node only. In the reconstruction process, in the case under study,
  // a ghost node is used (added at the end of the grid but not part of
  // the computational domain, extrapolate)

  // apply B.C. downstream
  w_vU1.push_back(/*get<0>(w_bcDownstream.Values())*/ Av); // A
  w_vU2.push_back(/*get<1>(w_bcDownstream.Values())*/ Qv); // Q

  // for debugging purpose (temporary fix) shall be set by the simulation
  // i am no sure about that or read from an xml file with the property
  // tree from boost
  DamBreakData w_dbDat(DamBreakData::DiscrTypes::emcneil);

  // use the default implementation of the Base class
  std::vector<double> w_vDU1(EMCNEILNbSections::value);
  std::vector<double> w_vDU2(EMCNEILNbSections::value);

  // set memory allocation
  // m_swerhs.resize(w_dbDat.nbSections());
  // m_swerhs.resize(w_dbDat.nbSections());

  // temporary fix (computational domain)
  std::vector<double> w_FF1(DIM::value);
  std::vector<double> w_FF2(DIM::value);

  BaseNumTreatmemt w_baseTreatment;
  // compute flux at cell face according to Riemann problem
  w_baseTreatment.CalculFF(w_FF1, w_FF2, w_vU1, w_vU2, w_vDU1, w_vDU2,
                           w_dbDat.nbSections(), w_dbDat.getWidth());

  // bathymetry
  const std::vector<double> &w_Z = w_dbDat.getIC().m_Z;
  std::vector<double> w_vH;
  w_vH.reserve(w_vU1.size());

  // now compute the water level
  std::transform(w_vU1.cbegin(), w_vU1.cend(), w_Z.cbegin(),
                 std::back_inserter(w_vH),
                 std::bind(&HydroUtils::Evaluation_H_fonction_A, _1, 1., _2));

  // Will be deprecated in the near future
  std::vector<double> w_S(DIM::value); // computational domain
  // source term discretization (this is the original signature of the function)
  w_baseTreatment.TraitementTermeSource2(
      w_S, w_vU2, w_vU1, w_vH, // ranges
      w_dbDat.getManning(), aU.first->grid().Delta(1), w_dbDat.nbSections(),
      w_dbDat.getWidth());

  // use the new signature by returning valarray (want to check)
  SourceTermsTreatment w_testImpl;
  auto w_srcArray = w_testImpl.TraitementTermeSource2(aU, w_bc, m_listSectFlow);

  // auto w_msg = std::string{"Exiting of the SweRhsAlgorithm::calculate()
  // method\n"};
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Exiting of the TestRhsImpl::calculate() method\n"}.data());

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
} // namespace dbpp
