// C++ include
#include <cassert>
#include <functional>
// package includes
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../Discretization/dbpp_HydroTerms.h"
#include "../Discretization/dbpp_ReconstrUtility.h"
#include "../Utility/dbpp_HydroCodeDef.h"
#include "dbpp_TestCalculFF.h"
//#include "EMcNeilUtils.h"
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../SfxTypes/dbpp_Simulation.h"

// deprecated
// static double dmaxarg1, dmaxarg2;
// #define DMAX(a,b) (dmaxarg1=(a), dmaxarg2=(b), (dmaxarg1) > (dmaxarg2) ?\
// 	(dmaxarg1) : (dmaxarg2))
//
// #define DMIN(a,b) (dmaxarg1=(a), dmaxarg2=(b), (dmaxarg1) < (dmaxarg2) ?\
// 	(dmaxarg1) : (dmaxarg2))

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
  double FR1, FL1, FR2, FL2;

  // HLL solver variables
  double uR, uL;
  double SR, SL;
  double CR, CL;
  double uS, CS;

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
    FL1 = aU2LR[i].first;  // UL2;
    FR1 = aU2LR[i].second; // UR2;

    // St-Venant flux term (conservative form) and not consider hydrostatic
    // pressure 				FL2 =
    // HydroTerms::EvaluationF2_I_1D (UL1, UL2, B); FR2 =
    // HydroTerms::EvaluationF2_I_1D (UR1, UR2, B);
    FL2 = HydroTerms::EvaluationF2_I_1D(aU1LR[i].first, aU2LR[i].first, 1.);
    FR2 = HydroTerms::EvaluationF2_I_1D(aU1LR[i].second, aU2LR[i].second, 1.);

    // ...
    // 				uL = UL2/UL1;
    // 				uR = UR2/UR1;
    uL = aU2LR[i].first / aU1LR[i].first;   // UL2/UL1;
    uR = aU2LR[i].second / aU1LR[i].second; // UR2/UR1;

    // celerity speed at shock face
    // 				CR = ::sqrt( grav*UR1/HydroUtils::T(B));
    // 				CL = ::sqrt( grav*UL1/HydroUtils::T(B));
    const double w_grav = PhysicalConstant::sGravity;
    CR = ::sqrt(w_grav * aU1LR[i].second / HydroUtils::T(1.));
    CL = ::sqrt(w_grav * aU1LR[i].first / HydroUtils::T(1.));
    //
    double w_CR = ::sqrt(w_grav * aU1LR[i].second / HydroUtils::T(1.));
    double w_CL = ::sqrt(w_grav * aU1LR[i].first / HydroUtils::T(1.));
    assert(CR == w_CR);
    assert(CL == w_CL);

    //	Une attention particulière devra être ici portée sur
    //	les signes (+/-) relativement au produit scalaire avec
    //	Si+1/2

    //	Calcul de SR et SL (shock speed at cell face)

    //	Étape intermédiaire: calcul de US et CS

    CS = 0.5 * (CL + CR) - 0.25 * (uL - uR);
    uS = 0.5 * (uL - uR) + CL - CR;

    // shall replace that by the standard algorithm,
    // this kind of home made functions are dangerous
    // and error prone since most of the time test all case not done.
    // 			SL = DMIN( uL - CL, uS - CS);	 deprecated use std::min
    // 			SR = DMAX( uR + CR, uS + CS);    deprecated use std::max

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
      // 				FF1[i] = (SR*FL1 - SL*FR1 +
      // SL*SR*(UR1-UL1))/(SR-SL); 				FF2[i] = (SR*FL2
      // - SL*FR2 + SL*SR*(UR2-UL2))/(SR-SL);
      FF1[i] =
          (SR * FL1 - SL * FR1 + SL * SR * (aU1LR[i].second - aU1LR[i].first)) /
          (SR - SL);
      FF2[i] =
          (SR * FL2 - SL * FR2 + SL * SR * (aU2LR[i].second - aU2LR[i].first)) /
          (SR - SL);
    }
  }
}

// Remark: this method doesn't make any sense!! i just don't get it?
// 	void TestCalculFF::calculFF( std::vector<double>& FF1,
// std::vector<double>& FF2, const StateVector& aU)
// 	{
// 		using namespace std;
// 		using namespace std::placeholders;
//
// 		// sanity check (debugging purpose)
// 		assert( aU.first->values().size()==100);
//
// 		// computational domain
// 		const dbpp::RealNumArray<real>& w_U1 = aU.first->values();
// 		const dbpp::RealNumArray<real>& w_U2 = aU.second->values();
//
// 		// When i do that i just resize or size the vector with default
// value (yeap!) 		vector<pair<real,real>> w_U1LR; // A
// w_U1LR.reserve(
// w_U1.size()); 		vector<pair<real,real>> w_U2LR; // Q
// w_U2LR.reserve( w_U2.size());
//
// 		// for passing to the right hand side numerical treatment
// 		vector<real> w_vU1( w_U1.getPtr(), w_U1.getPtr() + w_U1.size());
// 		vector<real> w_vU2( w_U2.getPtr(), w_U2.getPtr() + w_U2.size());

// apply B.C. upstream
// 		w_vU1[0]=get<0>( m_bcnodeAM); // A
// 		w_vU2[0]=get<1>( m_bcnodeAM); // Q

// need to do a push_back because the scalar field hold computational
// node only. In the reconstruction process, in the case under study,
// a ghost node is used (added at the end of the grid but not part of
// the computational domain, extrapolate)

// apply B.C. downstream
// 		w_vU1.push_back( get<0>( m_bcnodeAV));
// 		w_vU2.push_back( get<1>( m_bcnodeAV));

// reconstruct state or node variables at cell face (MUSCL-type extrapolation)
// Base class method used
// 		ReconstrUtil::reconstr_vec( w_vU1, w_vU2, w_U1LR, w_U2LR);
//
// 		// re-use
// 		TestCalculFF::calculFF( FF1, FF2, w_U1LR, w_U2LR);
//	}

// NOTE: cellFAce class has been re-factored so we should take a close look
// at the algorithm programmed below, i really doubt that it work properly.
//
void TestCalculFF::calculFF(const std::list<cellFace> &aListOface,
                            std::vector<double> &FF1,
                            std::vector<double> &FF2) {
  // an implementation base on cell face concept
  // cell face variable has been computed at given order
  // we are only concern about evaluating numerical
  // flux at cell face

  // hard coded for debugging purpose
  assert(aListOface.size() == 100);

  // shall use the simulation instance to retrieve discretization data
  DamBreakData w_dbData(DamBreakData::DiscrTypes::emcneil);
  double B = 0.; // default value
  if (w_dbData.isUnitSectionWidth()) {
    B = 1.; // overwrite
  }
  unsigned i = 0;
  const double w_grav = PhysicalConstant::sGravity;

  // initialize iterator (cell)
  std::list<cellFace>::const_iterator w_begCellF = aListOface.begin();
  while (w_begCellF != aListOface.end()) // loop on cell face
  {
    // left/right state variable at cell face
    const cellFace::pairvec w_lFace; // = (*w_begCellF).getUL(); // U1L,U2L
    const cellFace::pairvec w_rFace; //= (*w_begCellF).getUR(); // U1R,U2R

    const double FL1 = w_lFace.second; // aU2LR[i].first;  //UL2;
    const double FR1 = w_rFace.second; // aU2LR[i].second; //UR2;
    // HydroTerms::EvaluationF2_I_1D( aU1LR[i].first, aU2LR[i].first, 1.);
    const double FL2 =
        HydroTerms::EvaluationF2_I_1D(w_lFace.first, w_lFace.second, B);
    // HydroTerms::EvaluationF2_I_1D( aU1LR[i].second, aU2LR[i].second, 1.);
    const double FR2 =
        HydroTerms::EvaluationF2_I_1D(w_rFace.first, w_rFace.second, B);

    // compute flux at cell face
    const double uL = w_lFace.second / w_lFace.first; // UL2/UL1;
    const double uR = w_rFace.second / w_rFace.first; // UR2/UR1;

    // celerity speed at shock face
    const double CR = ::sqrt(
        w_grav * w_rFace.first /
        HydroUtils::T(B)); // ::sqrt( grav*aU1LR[i].second/HydroUtils::T(1.));
    const double CL = ::sqrt(
        w_grav * w_lFace.first /
        HydroUtils::T(B)); //::sqrt( grav*aU1LR[i].first/HydroUtils::T(1.));

    const double CS = 0.5 * (CL + CR) - 0.25 * (uL - uR);
    const double uS = 0.5 * (uL - uR) + CL - CR;
    const double SL = std::min(uL - CL, uS - CS);
    const double SR = std::max(uR + CR, uS + CS);

    //	Calcul de Fi+1/2
    if (SL > 0.) {
      FF1[i] = FL1;
      FF2[i] = FL2;
    } else if (SR < 0.) {
      FF1[i] = FR1;
      FF2[i] = FR2;
    } else {
      // 	FF1[i] = (SR*FL1 - SL*FR1 + SL*SR*(UR1-UL1))/(SR-SL);
      // 	FF2[i] = (SR*FL2 - SL*FR2 + SL*SR*(UR2-UL2))/(SR-SL);
      const double UR1 = w_rFace.first;
      const double UL1 = w_lFace.first;
      const double UR2 = w_rFace.second;
      const double UL2 = w_lFace.second;
      //
      FF1[i] = (SR * FL1 - SL * FR1 + SL * SR * (UR1 - UL1)) / (SR - SL);
      FF2[i] = (SR * FL2 - SL * FR2 + SL * SR * (UR2 - UL2)) / (SR - SL);
    }
  } // while-loop
}

// use the Reconstruction utility "vec" version
// still looping on the number of sections from list of section flow
// not sure if worth trying this version, because we eventually loop on cell
// face
//	void TestCalculFF::calculFF( std::vector<double>& FF1,
// std::vector<double>& FF2, const std::vector<double>& U1, 		const
// std::vector<double>& U2, const ListSectFlow* aListSectFlow)
//	{
//    throw "Not implemented yet";
//	}

// instead of copying the whole code i should re-use the base class method
// i mean the original (TraitementTermeSource2(all vector))
void TestCalculFF::TraitementTermeSource2(std::vector<double> &S,
                                          const std::vector<double> &Q,
                                          const std::vector<double> &A,
                                          const double aDx,
                                          ListSectFlow *aListSectFlow) {
  using namespace std;
  using namespace std::placeholders;

  // water level of each section
  vector<double> w_H;
  w_H.reserve(A.size());

  // retrieve water level of each section
  transform(aListSectFlow->getList().cbegin(), aListSectFlow->getList().cend(),
            std::back_inserter(w_H), std::bind(&SectFlow::H, _1));

  // retrieve active discretization data (Should be E. McNeil as default)
  DamBreakData w_dbData{
      dbpp::Simulation::instance()->getActiveDiscretization()};
  // const std::vector<double> &w_n = w_dbData.getManning();

  // check if all list of section are of unit width
  if (all_of(aListSectFlow->getList().begin(), aListSectFlow->getList().end(),
             mem_fn(&SectFlow::isUnitWidth))) {
    // this don't make sense, reason we inherit from BaseNumTreatmemt and this
    // method is virtual (TraitementTermeSource2) which it can be overriden.
    // Refactoring: BaseNumTreatment should be a static class that contains
    // all discretization for RHS terms. This way we could use it as an
    // attribute of this class and call this method.
    // we set the last argument as default to 1.
    BaseNumTreatmemt::TraitementTermeSource2(
        S, Q, A, w_H, w_dbData.getManning(), aDx,
        static_cast<const int>(aListSectFlow->size()));
  } else {
    // return 0 for every thing because we don't support
    fill(S.begin(), S.end(), 0.);
  }

  /*
                  double TermeS0;

      // physical constant
      const double w_grav = PhysicalConstant::sGravity;
      // spatial discretization
      //const double w_dx = emcil::GlobalDiscretization::instance()->getDx();

      for (unsigned j = 0; j < A.size()-1; j++)
      {
        // section width (at top)
        // Hydraulic radius
        const double Rh = HydroUtils::R( A[j],
     aListSectFlow->getList()[j]->B());

        // Manning coefficient of the section
        const double N = aListSectFlow->getList()[j]->N();

        // Manning formula (friction)
        const double TermeSf = w_grav*A[j]*N*N *Q[j]*fabs( Q[j])/(A[j]*A[j]*pow
     (Rh, 4./3.));

        //TermeSf = w_grav*A[j] * n[j]*n[j] * Q[j]*fabs(Q[j])/(A[j]*A[j]*pow
     (Rh, 4./3.));

        // bottom slop based on Nujic algorithm
        if (j == 0)
          TermeS0 = (0.5*w_grav*( A[j+1]+A[j]))*(w_H[j+1] - w_H[j])/aDx;

        else
          TermeS0 = (0.5 * w_grav * (A[j+1]+A[j-1]))*(w_H[j+1] -
     w_H[j-1])/(2.*aDx);

        S[j] = (TermeSf - TermeS0);
      }
                  */
}

// this implementation we will need to have the boundary condition, because the
// scalarField has only domain node, but to compute the derivative we need the
// ghost node
// void TestCalculFF::TraitementTermeSource2(std::vector<double> &S,
//                                         const dbpp::scalarField *Q,
//                                        const dbpp::scalarField *A,
//                                        const ListSectFlow *aListSectFlow) {
// throw "not implemented yet";
//}

// different using the state vector instead of using vector
// 	void TestCalculFF::calculFF(std::vector<double>& FF1,
// std::vector<double>& FF2, const StateVector& aU, 		const
// cellfaceVar& aU1LR, const cellfaceVar& aU2LR)
// 	{
// 		throw "not implemented yet";
// 	}

} // namespace dbpp
