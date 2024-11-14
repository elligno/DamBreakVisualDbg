// C++ includes
#include <cassert>
#include <functional>
// package includes
#include "../Discretization/dbpp_HydroTerms.h"
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../Utility/dbpp_EMcNeilUtils.h"
#include "dbpp_TestCalculFF.h"

namespace dbpp {
// still have a loop on the number of sections, this should desappear
// probably in the next version.
// Important: variable U1LR and U2LR are reconstructed at a given order
// in this case we have used the MUSCL reconstruction procedure
// Usage:
//   version with std::vector argument (nodal variable U1,U2)
//   and vecULR which is a vector of pair (left and right state)
//   ReconstrUtil::reconstr_vec(aU1,aU2, U1LR,U2LR); // static class for now
//   ....
//   calculFF(...); compute flux at cell face
//
void TestCalculFF::calculFF(std::vector<double> &FF1, std::vector<double> &FF2,
                            const cellfaceVar &aU1LR,
                            const cellfaceVar &aU2LR) {
  // interface variables (j+1/2)
  //		double UR1, UL1, UR2, UL2;
  // double FR1, FL1, FR2, FL2;

  // HLL solver variables
  // double uR, uL;
  double SR, SL;
  // double CR, CL;
  // double uS, CS;

  // sanity check, otherwise it wouldn't make any sense
  assert(aU1LR.size() == aU2LR.size());

  // actually we are looping over cell face (j+1/2) and not grid node
  // we first perform a reconstruction of state variable
  // at the interface by using a MUSCL interpolation type
  for (unsigned i = 0; i < aU1LR.size();
       /*aNbSections-1;*/ i++) // loop cell face variable
  {                            // actually we loop on cell face
    // from here we compute flux parameters according to HLL
    // i am not sure (check paper of the algorithm)
    auto FL1 = aU2LR[i].first;  // UL2;
    auto FR1 = aU2LR[i].second; // UR2;

    // St-Venant flux term (conservative form) and not consider hydrostatic
    // pressure 				FL2 =
    // HydroTerms::EvaluationF2_I_1D (UL1, UL2, B); FR2 =
    // HydroTerms::EvaluationF2_I_1D (UR1, UR2, B);
    auto FL2 =
        HydroTerms::EvaluationF2_I_1D(aU1LR[i].first, aU2LR[i].first, 1.);
    auto FR2 =
        HydroTerms::EvaluationF2_I_1D(aU1LR[i].second, aU2LR[i].second, 1.);

    // ...
    // 				uL = UL2/UL1;
    // 				uR = UR2/UR1;
    auto uL = aU2LR[i].first / aU1LR[i].first;   // UL2/UL1;
    auto uR = aU2LR[i].second / aU1LR[i].second; // UR2/UR1;

    // celerity speed at shock face
    // 				CR = ::sqrt( grav*UR1/HydroUtils::T(B));
    // 				CL = ::sqrt( grav*UL1/HydroUtils::T(B));
    const auto w_grav = PhysicalConstant::sGravity;
    auto CR = std::sqrt(w_grav * aU1LR[i].second / HydroUtils::T(1.));
    auto CL = std::sqrt(w_grav * aU1LR[i].first / HydroUtils::T(1.));
    //
    auto w_CR = std::sqrt(w_grav * aU1LR[i].second / HydroUtils::T(1.));
    auto w_CL = std::sqrt(w_grav * aU1LR[i].first / HydroUtils::T(1.));
    assert(CR == w_CR); // what is that??
    assert(CL == w_CL); // ditto

    //	Une attention particulière devra être ici portée sur
    //	les signes (+/-) relativement au produit scalaire avec
    //	Si+1/2

    //	Calcul de SR et SL (shock speed at cell face)

    //	Étape intermédiaire: calcul de US et CS

    auto CS = 0.5 * (CL + CR) - 0.25 * (uL - uR);
    auto uS = 0.5 * (uL - uR) + CL - CR;

    // testing the standard function
    SL = std::min(uL - CL, uS - CS);
    SR = std::max(uR + CR, uS + CS);
    // 			assert(SL==stdSL);
    // 			assert(SR==stdSR);

    //	Calcul de Fi+1/2
    if (SL > 0.) {
      FF1[i] = FL1;
      FF2[i] = FL2;
    } else if (SR < 0.) {
      FF1[i] = FR1;
      FF2[i] = FR2;
    } else {
      FF1[i] =
          (SR * FL1 - SL * FR1 + SL * SR * (aU1LR[i].second - aU1LR[i].first)) /
          (SR - SL);
      FF2[i] =
          (SR * FL2 - SL * FR2 + SL * SR * (aU2LR[i].second - aU2LR[i].first)) /
          (SR - SL);
    }
  }
}
} // namespace dbpp
