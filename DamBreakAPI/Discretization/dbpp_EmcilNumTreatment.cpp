
// C++ inlcudee
#include <functional> // bind mechanism
#include <iostream>
// boost range library
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range.hpp>
// project includes
#include "../Discretization/dbpp_HydroTerms.h"
#include "../Numerics/dbpp_TimeStepCriteria.hpp"
#include "../Utility/dbpp_EMcNeilUtils.h"
#include "dbpp_EmcilNumTreatment.h"
// ...
#include "../Discretization/dbpp_ReconstrUtility.h"
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../SfxTypes/dbpp_PhyConstant.h"

namespace dbpp {
BaseNumTreatmemt::~BaseNumTreatmemt() {
  std::cout << "We are in the default BaseNumTreatment ctor\n";
}

BaseNumTreatmemt::BaseNumTreatmemt() {
  std::cout << "We are in the BaseNumTreatment dtor\n";
}

void BaseNumTreatmemt::TraitementTermeP(std::vector<double> &PF2,
                                        std::vector<double> &P2,
                                        const std::vector<double> &U1,
                                        const int NbSections,
                                        const double B /*=1*/) {
  int j;
  double I;
  // physical constant such as gravity
  const double w_grav = PhysicalConstant::sGravity;

  // loop over the range
  for (j = 0; j < NbSections; j++) {
    I = HydroTerms::CalculTermePressionHydrostatique1D(U1[j], B);
    P2[j] = w_grav * I;
  }

  // loop over cell face
  for (j = 0; j < NbSections - 1; j++)
    PF2[j] = 0.5 * (P2[j + 1] + P2[j]);
}

void BaseNumTreatmemt::CalculS0(std::vector<double> &S0,
                                const std::vector<double> &Z, const double dx,
                                const int NbSections) {
  int i;

  S0[0] = (Z[1] - Z[0]) / dx;

  for (i = 1; i < NbSections - 1; i++)
    S0[i] = (Z[i + 1] - Z[i - 1]) / (2. * dx);

  S0[NbSections - 1] = (Z[NbSections - 1] - Z[NbSections - 2]) / dx;
}

void BaseNumTreatmemt::TraitementTermeSource(
    std::vector<double> &S, const std::vector<double> &Q,
    const std::vector<double> &A, const std::vector<double> &Z,
    const std::vector<double> &n, const double dx, const int NbSections,
    const double B /*=1*/) {
  int j;
  double Rh, TermeSf, TermeS0;
  const double w_grav = PhysicalConstant::sGravity;

  for (j = 0; j < NbSections - 1; j++) {
    Rh = HydroUtils::R(A[j], B);

    TermeSf = w_grav * A[j] * n[j] * n[j] * Q[j] * fabs(Q[j]) /
              (A[j] * A[j] * pow(Rh, 4. / 3.));

    if (j == 0)
      TermeS0 = (0.5 * w_grav * (A[j + 1] + A[j])) * (Z[j + 1] - Z[j]) / dx;

    else
      TermeS0 = (0.5 * w_grav * (A[j + 1] + A[j - 1])) * (Z[j + 1] - Z[j - 1]) /
                (2. * dx);

    S[j] = (TermeSf - TermeS0);
  }
}

void BaseNumTreatmemt::TraitementTermeSource2(
    std::vector<double> &S, const std::vector<double> &Q,
    const std::vector<double> &A, const std::vector<double> &H,
    const std::vector<double> &n, const double dx, const int NbSections,
    double B /*=1*/) {
  // int j;
  double TermeS0{};
  const double w_grav = PhysicalConstant::sGravity;

  for (auto j = 0; j < NbSections - 1; j++) {
    auto Rh = HydroUtils::R(A[j], B);
    // check this formula, i think its not correct
    auto TermeSf = w_grav * A[j] * n[j] * n[j] * Q[j] * fabs(Q[j]) /
                   (A[j] * A[j] * pow(Rh, 4. / 3.));

    if (j == 0)
      TermeS0 = (0.5 * w_grav * (A[j + 1] + A[j])) * (H[j + 1] - H[j]) / dx;

    else
      TermeS0 = (0.5 * w_grav * (A[j + 1] + A[j - 1])) * (H[j + 1] - H[j - 1]) /
                (2. * dx);

    S[j] = (TermeSf - TermeS0);
  }
}

// crazy number of arguments (no choice to keep it for now)
void BaseNumTreatmemt::CalculFF(std::vector<double> &FF1,
                                std::vector<double> &FF2,
                                const std::vector<double> &U1,
                                const std::vector<double> &U2,
                                std::vector<double> &dU1,
                                std::vector<double> &dU2, const int NbSections,
                                double B /*=1.*/) {
  // interface variables (j+1/2)
  double UR1, UL1, UR2, UL2;
  double FR1, FL1, FR2, FL2;

  // HLL solver variables
  double uR, uL;
  double SR, SL;
  double CR, CL;
  double uS, CS;

  const double w_grav = PhysicalConstant::sGravity;
  // int i;

  // NOTE: Boundary condition (i=0,i=100)
  // below we do a special treatment for i=0
  // but for i=100 no special treatment, using pointer
  // doesn't really matter, it just use garbage, but with
  // vector we should have an error (runtime) out of range
  //
  //	Calculs préalables: évaluation des éléments du vecteur dU
  dU1[0] = HydroUtils::minmod(U1[1] - U1[0], 0.);
  dU2[0] = HydroUtils::minmod(U2[1] - U2[0], 0.);
  // change loop index because out-of-range error
  // i=1, i<NbSections-1 compare to Eric McNeil code ()
  // and added the boundary cnd. for the last node (ghost node i=100)
  for (int i = 1; i < NbSections - 1; i++) // last element index=100
  {                                        // i+1 -> 101 (doesn't exist)
    dU1[i] = HydroUtils::minmod(U1[i + 1] - U1[i], U1[i] - U1[i - 1]);
    dU2[i] = HydroUtils::minmod(U2[i + 1] - U2[i], U2[i] - U2[i - 1]);
  }

  // special treatment i=NbSections-1=100??
  // IMPORTANT: shall change the index looping above. There is 2 errors
  // 1: i=0 which is wrong because this case already considered
  // 2: i=100 go out of range (U[101] doesn't exist
  // i=1;i<NbSections-1 [1,...,99]
  // put these lines below in comment and run the algorithm and see what happens
  dU1[NbSections - 1] =
      HydroUtils::minmod(0., U1[NbSections - 1] - U1[NbSections - 2]);
  dU2[NbSections - 1] =
      HydroUtils::minmod(0., U2[NbSections - 1] - U2[NbSections - 2]);

  // actually we are looping over cell face (j+1/2) and not grid node
  // we first perform a reconstruction of state variable
  // at the interface by using a MUSCL interpolation type
  for (int i = 0; i < NbSections - 1; i++) {
    //	Calcul des éléments des vecteurs UR, UL, FR et FL
    UL1 = U1[i] + 0.5 * dU1[i];
    UR1 = U1[i + 1] - 0.5 * dU1[i + 1];

    UL2 = U2[i] + 0.5 * dU2[i];
    UR2 = U2[i + 1] - 0.5 * dU2[i + 1];

    // from here we compute flux parameters according to HLL
    // i am not sure (check paper of the algorithm)
    FL1 = UL2;
    FR1 = UR2;
    // St-Venant flux term (conservative form) and not consider hydrostatic
    // pressure
    FL2 = HydroTerms::EvaluationF2_I_1D(UL1, UL2, B);
    FR2 = HydroTerms::EvaluationF2_I_1D(UR1, UR2, B);

    uL = UL2 / UL1;
    uR = UR2 / UR1;

    CR = ::sqrt(w_grav * UR1 / HydroUtils::T(B));
    CL = ::sqrt(w_grav * UL1 / HydroUtils::T(B));

    //	Une attention particulière devra être ici portée sur
    //	les signes (+/-) relativement au produit scalaire avec
    //	Si+1/2

    //	Calcul de SR et SL

    //	Étape intermédiaire: calcul de US et CS

    CS = 0.5 * (CL + CR) - 0.25 * (uL - uR);
    uS = 0.5 * (uL - uR) + CL - CR;

    // 			SL = DMIN( uL - CL, uS - CS);
    // 			SR = DMAX( uR + CR, uS + CS);
    SL = std::min(uL - CL, uS - CS);
    SR = std::max(uR + CR, uS + CS);

    //	Calcul de Fi+1/2
    if (SL > 0.) {
      FF1[i] = FL1;
      FF2[i] = FL2;
    } else if (SR < 0.) {
      FF1[i] = FR1;
      FF2[i] = FR2;
    } else {
      FF1[i] = (SR * FL1 - SL * FR1 + SL * SR * (UR1 - UL1)) / (SR - SL);
      FF2[i] = (SR * FL2 - SL * FR2 + SL * SR * (UR2 - UL2)) / (SR - SL);
    }
  }
}

void BaseNumTreatmemt::CalculFF(std::vector<double> &FF1,
                                std::vector<double> &FF2,
                                const std::vector<double> &U1,
                                const std::vector<double> &U2,
                                double B /*=1.*/) {
  // variables at cell face (x_j+1/2)
  vecULR w_U1LR;
  w_U1LR.reserve(U1.size() - 1);
  vecULR w_U2LR;
  w_U2LR.reserve(U2.size() - 1);

  // MUSCL reconstruction procedure at second-order with minmod flux limiter
  ReconstrUtil::reconstr_vec(U1, U2, w_U1LR, w_U2LR);

  // sanity check
  assert(w_U1LR.size() != 0);
  assert(w_U2LR.size() != 0);

  // actually we are looping over cell face (j+1/2) and not grid node
  // we first perform a reconstruction of state variable
  // at the interface by using a MUSCL interpolation type
  for (size_t i = 0; i < w_U1LR.size() /*NbSections-1*/; i++) {
    //	Calcul des éléments des vecteurs UR, UL, FR et FL
    const double UL1 = w_U1LR[i].first;  // U1[i] + 0.5*dU1[i];
    const double UR1 = w_U1LR[i].second; // U1[i+1] - 0.5*dU1[i+1];

    const double UL2 = w_U2LR[i].first;  // U2[i] + 0.5*dU2[i];
    const double UR2 = w_U2LR[i].second; // U2[i+1] - 0.5*dU2[i+1];

    // from here we compute flux parameters according to HLL
    // i am not sure (check paper of the algorithm)
    const double FL1 = UL2;
    const double FR1 = UR2;

    // St-Venant flux term (conservative form) and not consider hydrostatic
    // pressure
    const double FL2 = HydroTerms::EvaluationF2_I_1D(UL1, UL2, B);
    const double FR2 = HydroTerms::EvaluationF2_I_1D(UR1, UR2, B);

    const double uL = UL2 / UL1;
    const double uR = UR2 / UR1;

    const double CR =
        ::sqrt(PhysicalConstant::sGravity * UR1 / HydroUtils::T(B));
    const double CL =
        ::sqrt(PhysicalConstant::sGravity * UL1 / HydroUtils::T(B));

    //	Une attention particulière devra être ici portée sur
    //	les signes (+/-) relativement au produit scalaire avec
    //	Si+1/2

    //	Calcul de SR et SL

    //	Étape intermédiaire: calcul de US et CS

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
      FF1[i] = (SR * FL1 - SL * FR1 + SL * SR * (UR1 - UL1)) / (SR - SL);
      FF2[i] = (SR * FL2 - SL * FR2 + SL * SR * (UR2 - UL2)) / (SR - SL);
    }
  }
}

// compute boundary condition according to characteristic equation
void BaseNumTreatmemt::computeBC(std::vector<double> &U1,
                                 std::vector<double> &U2,
                                 std::vector<double> &H)
// std::tuple<double,double,double>& aBcamont, std::tuple<double,double,double>&
// aBcaval)
{
  // set tuple with computed values
  setAmont(U1, U2, H);
  setAval(U1, U2, H);
}

void BaseNumTreatmemt::setAmont(std::vector<double> &U1,
                                std::vector<double> &U2,
                                std::vector<double> &H) {
  // physical constant such as gravity
  const double w_grav = PhysicalConstant::sGravity;

  // IMPORTANT
  //   we use as default E. McNeil data (but it should be set by the simulation
  //   class which is responsible to load config parameters).
  // dam-break data for the simulation
  DamBreakData w_dbData(DamBreakData::DiscrTypes::emcneil);
  //		const double S0am=w_dbData.getS0am();
  const double S0av = w_dbData.getS0av(); // some
  const double B = w_dbData.getWidth();   // section width

  // REMARK (Jean Belanger)
  // shall use the concept the GlobalDiscretization to retrieve
  // the nodal values (global) since it is updated at the end of
  // each time step, during a time step it still the previous values.
  // code sample
  //             boost::ptr_vector<Nodal_Value>&
  //             w_aqh=GlobalDiscretization::instance().Uh();
  // 			// need to retrieve the A,Q of the Nodal_Value
  // 			auto beg_iterA = boost::make_transform_iterator( w_aqh.begin(),
  // emcil::GetA()); 			auto end_iterA = boost::make_transform_iterator(
  // w_aqh.end(), emcil::GetA()); 			std::vector<double> w_U1; 			const double dt =
  // TimeStepCriteria::timeStep( std::vector<double>(beg_iterA,end_iterA),
  // 				                                          std::vector<double>(beg_iterQ,end_iterQ));
  // // A,Q

  // time step of the simulation (i am not sure about this one)
  // make sure that the U1,U2 are values from previous time step
  const double dt = TimeStepCriteria::timeStep(U1, U2);

  // temporary fix  (just want to validate the concept)
  // for this validation  we are using flat bed (again shall be provided
  // by the simulation)
  std::vector<double> Z(U1.size());
  std::vector<double> n(U1.size());

  //
  //	Amont, spécification de l'élévation de la surface libre
  //

  // variable that hold bc values
  double Ham, U1am, U2am;

  Ham = 10.;                           // water level
  U1am = HydroUtils::A(B, Ham - Z[0]); // wetted area

  //	Calcul des caractéristiques à la section située directement en aval
  const double n1 = n[1];                    // manning
  const double V1 = U2[1] / U1[1];           // velocity
  const double R1 = HydroUtils::R(U1[1], B); // hydraulic radius
  const double Y1 = H[1] - Z[1];             // water depth

  // because we are considering a rectangular channel
  // see reference (Modeling Of Open Channel Flow, chapter #2)
  const double CB = ::sqrt(w_grav * U1[1] / HydroUtils::T(B));

  // Manning formula (see documentation)
  const double Sf1 = n1 * n1 * V1 * ::fabs(V1) / ::pow(R1, 4. / 3.);
  // see again documentation (characteristics equation solution
  // i do not understand yet the *Y1, probably a unit
  const double CN = V1 - CB * Y1 +
                    w_grav * (S0av - Sf1) *
                        dt; // negative C characteristic (subcritical regime)
  // i do not understand this line (i don't get it)
  const double V0 =
      CN + CB * (Ham - Z[0]); // velocity at upstream along characteristic

  // set discharge Q at boundary node
  U2am = V0 * U1am; // discharge[m3/s]=velocity*surface

  printf("Condition amont: A: %lf  Q: %lf\n", U1am, U2am);

  //			return std::make_tuple(U1am,U2am,Ham);
}

void BaseNumTreatmemt::setAval(std::vector<double> &U1, std::vector<double> &U2,
                               std::vector<double> &H) {
  //
  //	Aval, spécification du débit
  //

  // physical constant such as gravity
  const double w_grav = PhysicalConstant::sGravity;

  // dam-break data for the simulation
  // Remark: we use E. McNeil dam break data as default
  // but it should provided by the simulation class
  DamBreakData w_dbData(DamBreakData::DiscrTypes::emcneil);
  // time step of the simulation (i am not sure about this one)
  // make sure that the U1,U2 are values from previous time step
  const double dt = TimeStepCriteria::timeStep(U1, U2);
  const double S0am = w_dbData.getS0am();
  //			const double S0av=w_dbData.getS0av();  // some
  const double B = w_dbData.getWidth(); // section width

  // temporary fix  (just want to validate the concept)
  std::vector<double> Z(U1.size());
  std::vector<double> n(U1.size());

  // variable that hold bc values
  double Hav, U1av, U2av;
  const int NbSections = w_dbData.nbSections();

  U2av = 0.;

  const double n1 = n[NbSections - 2];
  const double V1 = U2[NbSections - 2] / U1[NbSections - 2];
  const double R1 = HydroUtils::R(U1[NbSections - 2], B);
  const double Y1 = H[NbSections - 2] - Z[NbSections - 2];

  const double CA = ::sqrt(w_grav * U1[NbSections - 2] / HydroUtils::T(B));
  const double Sf1 = n1 * n1 * V1 * ::fabs(V1) / ::pow(R1, 4. / 3.);

  const double CP = CA * Y1 + w_grav * (S0am - Sf1) * dt + V1;

  Hav = (CP / CA) + Z[NbSections - 1];
  U1av = HydroUtils::Evaluation_A_fonction_H(Hav, Z[NbSections - 1], B);

  printf("Condition aval: A: %lf  Q: %lf\n", U1av, U2av);

  //		return std::make_tuple(U1av,U2av,Hav);
}
} // namespace dbpp
