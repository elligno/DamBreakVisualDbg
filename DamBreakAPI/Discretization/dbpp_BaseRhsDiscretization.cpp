// C++ include
#include <iostream>
// stl include
#include <algorithm> // min and max
// BaseNumTypes library
#include "../SfxTypes/dbpp_scalarField.h"

#include "../SfxTypes/dbpp_PhyConstant.h" //need to rename this file, important to name things propferly
#include "../Utility/dbpp_EMcNeilUtils.h"
#include "dbpp_HydroTerms.h"
// package includes
// #include "Access.h"
// #include "IConservative.h"
#include "../SfxTypes/dbpp_IRhsDiscretization.h"

namespace dbpp {
// Design Note
// ------------
//    This a new version of the RHS (Right-Hand-Side discretization)
//    More prototyping at this stage, i am just trying to express
//    the signature of the methods in terms of the programming env. type.
//
// An implementation of the RHS(Right-Hand-Side) terms discretization.
// This discretization is based on Eric McNeil HLL solver.
//
// 13-feb-17
//  Implement a POM mechanism (Physics Object Model)
//
class TestRhsDiscretization : public Access,
                              public IRhsDiscretization,
                              public IUpdateable,
                              public IDisplayable {
public:
  TestRhsDiscretization();

  //
  // Access
  //
  void *getInterface(const std::string &aStr2Interface) override final;

  //
  // IUpdateable
  //
  void update() override final;

  //
  // IDisplayable
  //
  void display() override final;

  // Temp fix
  void getNumericalTreatment() override {}

  // pressure term discretization
  virtual void TraitementTermeP(vecdbl &PF2, vecdbl &P2, const vecdbl &U1,
                                const int NbSections,
                                const double B = 1.) override final;

  // bathymetry source term discretization
  virtual void CalculS0(vecdbl &S0, const vecdbl &Z, const double dx,
                        const int NbSections) override final;

  // bottom source term discretization
  virtual void TraitementTermeSource(vecdbl &S, const vecdbl &Q,
                                     const vecdbl &A, const vecdbl &Z,
                                     const vecdbl &n, const double dx,
                                     const int NbSections,
                                     const double B = 1.) override final;

  // discretization of the friction and bottom source term
  virtual void TraitementTermeSource2(vecdbl &S, const vecdbl &Q,
                                      const vecdbl &A, const vecdbl &H,
                                      const vecdbl &n, const double dx,
                                      const int NbSections,
                                      double B = 1.) override final;

  // numerical flux at cell interface (x+1/2)
  virtual void CalculFF(vecdbl &FF1, vecdbl &FF2, const vecdbl &U1,
                        const vecdbl &U2, vecdbl &dU1, vecdbl &dU2,
                        const int NbSections, double B = 1.) override final;

  // resource
  virtual void Release() override final;

private:
  // not allowed by client
  TestRhsDiscretization(const TestRhsDiscretization &) = delete;
  TestRhsDiscretization &operator=(const TestRhsDiscretization &) = delete;
};

void TestRhsDiscretization::TraitementTermeP(vecdbl &PF2, vecdbl &P2,
                                             const vecdbl &U1,
                                             const int NbSections,
                                             const double B /*=1*/) {
  //    using namespace basenum;

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

void TestRhsDiscretization::CalculS0(vecdbl &S0, const vecdbl &Z,
                                     const double dx, const int NbSections) {
  int i;

  S0[0] = (Z[1] - Z[0]) / dx;

  for (i = 1; i < NbSections - 1; i++)
    S0[i] = (Z[i + 1] - Z[i - 1]) / (2. * dx);

  S0[NbSections - 1] = (Z[NbSections - 1] - Z[NbSections - 2]) / dx;
}

void TestRhsDiscretization::TraitementTermeSource(
    vecdbl &S, const vecdbl &Q, const vecdbl &A, const vecdbl &Z,
    const vecdbl &n, const double dx, const int NbSections,
    const double B /*=1*/) {
  //    using namespace basenum;

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

void TestRhsDiscretization::TraitementTermeSource2(
    vecdbl &S, const vecdbl &Q, const vecdbl &A, const vecdbl &H,
    const vecdbl &n, const double dx, const int NbSections, double B /*=1*/) {
  //    using namespace basenum;

  int j;
  double Rh, TermeSf, TermeS0;
  const double w_grav = PhysicalConstant::sGravity;

  for (j = 0; j < NbSections - 1; j++) {
    Rh = HydroUtils::R(A[j], B);

    TermeSf = w_grav * A[j] * n[j] * n[j] * Q[j] * fabs(Q[j]) /
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
void TestRhsDiscretization::CalculFF(vecdbl &FF1, vecdbl &FF2, const vecdbl &U1,
                                     const vecdbl &U2, vecdbl &dU1, vecdbl &dU2,
                                     const int NbSections, double B /*=1.*/) {
  // using namespace basenum;

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
    SL = std::min<double>(uL - CL, uS - CS);
    SR = std::max<double>(uR + CR, uS + CS);

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

void TestRhsDiscretization::Release() { delete this; }

TestRhsDiscretization::TestRhsDiscretization()
    : Access(
          "IDisplayable IUpdateable") // supported interface for this component
{
  std::cout << "TestRhsDiscretization::TestRhsDiscretization(string) ctor\n";
}

// inherited from Access mechanism interface
void *TestRhsDiscretization::getInterface(const std::string &aStr2Interface) {
  std::cout << "getInterface() of inherit\n";

  if (hasInterface()) // support interface
  {
    // 			if ( ::strcmp("IDisplayable",aStr2Interface.data())==0)
    // 			{
    // 				return static_cast<aa::IDisplayable*>(this);
    // 			}
    if (::strcmp("IUpdateable", aStr2Interface.data()) == 0) {
      return static_cast<IUpdateable *>(this);
    }
    // 			else if(
    // ::strcmp("IConservative",aStr2Interface.data())==0)
    // 			{
    // 				return (IConservative*)this;
    // 			}
    else if (::strcmp("IDisplayable", aStr2Interface.data()) == 0) {
      return static_cast<IDisplayable *>(this);
    } else {
      return nullptr;
    }
  } else {
    return nullptr;
  }
}

void TestRhsDiscretization::update() {
  std::cout << "Updating BC values at each iteration\n";
}

void TestRhsDiscretization::display() {
  std::cout << "Display values of the BC upstream/downstream\n";
}

} // namespace dbpp

////////////////////////////////////////////////////////////////////////////////
// Factory function that creates instances if the Xyz object.

// Export both decorated and undecorated names.
//   GetEMCNEIL1D    - Undecorated name, which can be easily used with
//   GetProcAddress
//                      Win32 API function.
//   _GetEMCNEIL1D@0 - Common name decoration for __stdcall functions in C
//   language.
//
// For more information on name decoration see here:
// "Format of a C Decorated Name"
// http://msdn.microsoft.com/en-us/library/x7kb4e2f.aspx

#if !defined(_WIN64)
// This pragma is required only for 32-bit builds. In a 64-bit environment,
// C functions are not decorated.
#pragma comment(linker, "/export:GetRhsDiscretization=_GetRhsDiscretization@0")
#pragma comment(linker, "/export:RhsRelease=_RhsRelease@4")
#endif // _WIN64

RHSHANDLE APIENTRY GetRhsDiscretization() {
  // created with default argument
  return new dbpp::TestRhsDiscretization();
}

VOID APIENTRY RhsRelease(RHSHANDLE handle) {
  if (handle) {
    handle->Release();
  }
}
