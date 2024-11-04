// Lib includes
#include "dbpp_SourceTermsAlgorithms.h"
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_AppConstant.hpp"
#include "../Utility/dbpp_EMcNeilUtils.h"

namespace dbpp {

// instead of copying the whole code i should re-use the base class method
// i mean the original (TraitementTermeSource2(all vector))
void SourceTermsTreatment::TraitementTermeSource2(
    std::vector<double> &S, const std::vector<double> &Q,
    const std::vector<double> &A, const double aDx,
    const ListSectFlow *aListSectFlow) {
  // using namespace std;
  using namespace std::placeholders;

  // water level of each section
  std::vector<double> w_H;
  w_H.reserve(A.size());

  // avoid calling cbegin/cend on temporary
  const auto begList = aListSectFlow->getList();

  // retrieve water level of each section
  std::transform(begList.cbegin(), begList.cend(), std::back_inserter(w_H),
                 std::bind(&SectFlow::H, _1));

  // retrieve active discretization data (Should be E. McNeil as default)
  DamBreakData w_dbData{
      dbpp::Simulation::instance()->getActiveDiscretization()};
  // const std::vector<double> &w_n = w_dbData.getManning();

  // check if all list of section are of unit width
  if (aListSectFlow->isUnitWidth()) {
    // this don't make sense, reason we inherit from BaseNumTreatmemt and this
    // method is virtual (TraitementTermeSource2) which it can be overriden.
    // Refactoring: BaseNumTreatment should be a static class that contains
    // all discretization for RHS terms. This way we could use it as an
    // attribute of this class and call this method.
    // we set the last argument as default to 1.
    BaseNumTreatmemt w_baseRhsDiscr;
    w_baseRhsDiscr.TraitementTermeSource2(
        S, Q, A, w_H, w_dbData.getManning(), aDx,
        static_cast<const int>(aListSectFlow->size()));
  } else {
    // return 0 for every thing because we don't support
    fill(S.begin(), S.end(), 0.);
  }
}

std::valarray<double> SourceTermsTreatment::TraitementTermeSource2(
    const StateVector &aStateVec, const ListSectFlow *aListSectFlow) {

  // ghost node at right far end (size+1)
  std::vector<double> w_S(EMCNEILNbSections::value);
  TraitementTermeSource2(w_S, aStateVec.first->values().to_stdVector(),
                         aStateVec.second->values().to_stdVector(),
                         aStateVec.first->grid().Delta(1), aListSectFlow);

  return std::valarray<double>(w_S.data(), w_S.size());
}

std::valarray<double> SourceTermsTreatment::TraitementTermeSource2(
    const StateVector &aStateVec, const Gamma &aBC,
    const ListSectFlow *aListSectFlow) {
  // need to pass b.c.
  auto w_Avec = aStateVec.first->values().to_stdVector();
  auto w_Qvec = aStateVec.second->values().to_stdVector();
  double Aval{};
  double Qval{};
  double Hval{};
  std::tie(Aval, Qval, Hval) = aBC.getBCNodeAval().Values();
  w_Avec.push_back(Aval);
  w_Qvec.push_back(Qval);
  assert(EMCNEILNbSections::value == w_Avec.size());
  // what about list section flow?? b.c.?
  if (aListSectFlow->getList().size() == EMCNEILNbSections::value) {
    // auto decay, no choice to return a reference
    auto ptrSect = aListSectFlow->getList();
    auto &w_lastSection = ptrSect.back();
    w_lastSection->setH(Hval); // set b.c. ghost section
  }
  // compute source term
  std::vector<double> w_S(DIM::value); // computational domain
  TraitementTermeSource2(w_S, w_Qvec, w_Avec, aStateVec.first->grid().Delta(1),
                         aListSectFlow);

  return std::valarray<double>(w_S.data(), w_S.size());
}

std::valarray<double> SourceTermsTreatment::TraitementTermeS0(
    const std::vector<double> &Q, const std::vector<double> &A,
    const double aDx, const ListSectFlow *aListSectFlow) {
  // sanity check
  assert(EMCNEILNbSections::value == A.size());

  // water level of each section
  std::vector<double> w_H;
  w_H.reserve(A.size());

  using namespace std::placeholders;

  // avoid calling cbegin/cend on temporary
  const auto begList = aListSectFlow->getList();

  // retrieve water level of each section
  std::transform(begList.cbegin(), begList.cend(), std::back_inserter(w_H),
                 std::bind(&SectFlow::H, _1));

  // physical constant
  const auto w_grav = PhysicalConstant::sGravity;

  std::valarray<double> S(EMCNEILNbSections::value);
  double TermeS0{};

  for (unsigned j = 0; j < A.size() - 1; j++) {
    // section width (at top)
    // Hydraulic radius
    const auto Rh = HydroUtils::R(A[j], aListSectFlow->getList()[j]->B());

    // Manning coefficient of the section
    const auto N = aListSectFlow->getList()[j]->N();

    // Manning formula (friction)
    const auto TermeSf = w_grav * N * N * Q[j] * std::fabs(Q[j]) /
                         (A[j] * A[j] * std::pow(Rh, 4. / 3.));

    // TermeSf = w_grav*A[j] * n[j]*n[j] * Q[j]*fabs(Q[j])/(A[j]*A[j]*pow
    // (Rh, 4./3.));

    // bottom slop based on Nujic algorithm
    if (j == 0)
      TermeS0 =
          (0.5 * w_grav * (A[j + 1] + A[j])) * (w_H[j + 1] - w_H[j]) / aDx;

    else
      TermeS0 = (0.5 * w_grav * (A[j + 1] + A[j - 1])) *
                (w_H[j + 1] - w_H[j - 1]) / (2. * aDx);

    S[j] = (TermeSf - TermeS0);
  }
  return S; // source term
}
} // namespace dbpp
