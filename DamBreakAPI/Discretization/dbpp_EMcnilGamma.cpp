
// project includes
#include "../SfxTypes/dbpp_DamBreakData.h"
#include "../SfxTypes/dbpp_ListSectFlow.h"
// ...
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "dbpp_GlobalDiscretization.h"

namespace dbpp {
// IMPORTANT:
// ----------
// we are using the characteristic method to solve St-Venant for
// boundary condition upstream/downstream (this method is explained
// in the following document from springer.com).
// Reference: Modeling Of The Open Channel Flow (chapter #2)
// google it with "an introduction to st-venant one-dimensional model"
// Reminder
//   Curve in the x/t plane (phase space) ... to be completed
//   Technical report ....
//   Another, we are considering the supercritical flow (Fr>1)
//   it will be nice if we could measure during the simulation
//   the Froude number.
//   (A,Q,H)
// void Gamma::getBCAmont(Nodal_Value &aNvalbnd) const {
//  GlobalDiscretization *w_gDiscr = GlobalDiscretization::instance();
//}

// void Gamma::getBCAval(Nodal_Value &aNvalbnd) const {
//  GlobalDiscretization *w_gDiscr = GlobalDiscretization::instance();
//  DamBreakData w_discrActive(DamBreakData::DiscrTypes::emcneil);
//  aNvalbnd[1] = 0.;
//}

// Design Note
// pas as args the GlobalDiscr and ListofSect (ptr)
// Separate class (static) to compute b.c.
// return nodal values computed
// Gamma manage the b.c. independently from the implementation
void Gamma::setAmont() {
  using namespace std;
  using namespace dbpp;

  // retrieve values from active global discretization
  auto U1 = GlobalDiscretization::instance()->U1values();
  auto U2 = GlobalDiscretization::instance()->U2values();
  auto H = GlobalDiscretization::instance()->Hvalues();
  // auto Z = GlobalDiscretization::instance()->Zvalues();
  std::vector<double> Z(U1.size());
  const auto w_n = 0.;  // w_sectUp->N();
  const auto S0av = 0.; // w_sectUp->getS0av();
  const auto B = 1.;    // w_sectUp->B();

  // auto w_activeDiscr = Simulation::instance()->getActiveDiscretization();
  DamBreakData w_discrData(Simulation::instance()->getActiveDiscretization());

  //	Amont, spécification de l'élévation de la surface libre
  H[0] = w_discrData.getPhi1();          // 10.;  water level
  U1[0] = HydroUtils::A(B, H[0] - Z[0]); // wetted area

  //	Calcul des caractéristiques à la section située directement en aval
  const auto n1 = w_n;                     // n[1];  manning
  const auto V1 = U2[1] / U1[1];           // velocity
  const auto R1 = HydroUtils::R(U1[1], B); // hydraulic radius
  const auto Y1 = H[1] - Z[1];             // water depth

  // because we are considering a rectangular channel of unit width
  // see reference (Modeling Of Open Channel Flow, chapter #2)
  const auto CB = ::sqrt(PhysicalConstant::sGravity * U1[1] / HydroUtils::T(B));

  // Manning formula (see documentation)
  const auto Sf1 = n1 * n1 * V1 * std::fabs(V1) / std::pow(R1, 4. / 3.);

  // simulation current time step
  const auto dt = Simulation::instance()->simulationTimeStep();

  // see again documentation (characteristics equation solution
  // i do not understand yet the *Y1, probably a unit
  const auto CN = V1 - CB * Y1 +
                  PhysicalConstant::sGravity * (S0av - Sf1) *
                      dt; // negative C characteristic (subcritical regime)

  // i do not understand this line (i don't get it)
  const auto V0 =
      CN + CB * (H[0] - Z[0]); // velocity at upstream along characteristic

  // set discharge Q at boundary node
  U2[0] = V0 * U1[0]; // discharge[m3/s]=velocity*surface

  printf("Gamma implementation\n");
  printf("Condition amont: A: %lf  Q: %lf\n", U1[0], U2[0]);

  // set Nodal_Value b.c.
  m_amont[0] = U1[0]; // A
  m_amont[1] = U2[0]; // Q
  m_amont[2] = H[0];  // H
#if 0
		int w_check=m_amont.no_of_dim();

		// debugging purpose
		const double w_A=m_amont[0]; // A
		const double w_Q=m_amont[1]; // Q
		const double w_H=m_amont[2];  // H
#endif // 0
       //	std::cout << "checking bc values\n";
}

// DESIGN NOTE (new signature)
//  void Gamma::setAval(Nodal_Value, last section, dt)
//  Extract characteristic computation and create a static
//  class with the following methods:
//    -- tuple<double> computeDownstreamCharacteristic()
//    -- tuple<double> computeUpstreamCharacteristic()
//
void Gamma::setAval() {
  using namespace std;
  using namespace dbpp;

  auto U1 = GlobalDiscretization::instance()->U1values();
  auto U2 = GlobalDiscretization::instance()->U2values();
  auto H = GlobalDiscretization::instance()->Hvalues();
  const auto NbSections = GlobalDiscretization::instance()->getNbSections();

  // section downstream b.c. node (g_ListSectFlow is deprecated)
  // 		SectFlow* w_lastSect = emcil::g_ListSectFlow->getList().back();
  // 		SectFlow* w_sectDown =
  // g_ListSectFlow->getList()[w_lastSect->getId()-1];

  // take the implementation of EMcNeil1D
  const auto S0am = 0.; // w_sectDown->getS0am();
  const auto w_n = 0.;  // w_sectDown->N();

  // Aval, spécification du débit
  U2[NbSections - 1] = 0.; // because we reserve and set i=0,...,99

  //  printf("Condition aval: A: %lf  Q: %lf\n", U1[NbSections - 2],
  //         U2[NbSections - 2]);

  const auto n1 = w_n; // n[NbSections-2];
  const auto V1 = U2[NbSections - 2] / U1[NbSections - 2];
  const auto R1 = HydroUtils::R(U1[NbSections - 2], 1. /*w_sectDown->B()*/);
  const auto Y1 =
      H[NbSections - 2] - 0. /*w_sectDown->Z()*/; // Z[NbSections-2];
  const auto CA = ::sqrt(PhysicalConstant::sGravity * U1[NbSections - 2] /
                         HydroUtils::T(1. /*w_sectDown->B()*/));

  const auto Sf1 = n1 * n1 * V1 * ::fabs(V1) / ::pow(R1, 4. / 3.);
  // retrieve current simulation time step
  const auto dt = Simulation::instance()->simulationTimeStep();
  const auto CP = CA * Y1 + PhysicalConstant::sGravity * (S0am - Sf1) * dt + V1;

  // because we reserve i=0,...,99 need to add i=100
  H[NbSections - 1] = (CP / CA) + 0. /*w_lastSect->Z()*/; // Z[NbSections-1];
  U1[NbSections - 1] = HydroUtils::Evaluation_A_fonction_H(
      H[NbSections - 1], 0. /*w_lastSect->Z()Z[NbSections-1]*/,
      1. /*w_lastSect->B()*/);

  printf("Condition aval: A: %lf  Q: %lf\n", U1[NbSections - 1],
         U2[NbSections - 1]);

  // set Nodal_Value m_aval
  m_aval[0] = U1[NbSections - 1];
  m_aval[1] = U2[NbSections - 1];
  m_aval[2] = H[NbSections - 1];
}

void Gamma::applyBC() {
  // call setAmont() and setAval() in a row
  setAmont();
  setAval();

  //		typedef GlobalDiscretization::instance() g_discr;
  // set the global values bc

  // amont
  GlobalDiscretization::instance()->U1values()[0] = m_amont[0];
  GlobalDiscretization::instance()->U2values()[0] = m_amont[1];
  GlobalDiscretization::instance()->Hvalues()[0] = m_amont[2];

  // ...
  unsigned w_nbsect = GlobalDiscretization::instance()->getNbSections() - 1;

  // aval
  GlobalDiscretization::instance()->U1values()[w_nbsect] = m_aval[0];
  GlobalDiscretization::instance()->U2values()[w_nbsect] = m_aval[1];
  GlobalDiscretization::instance()->Hvalues()[w_nbsect] = m_aval[2];

  // Nodal_Value& w_nval2 = GlobalDiscretization::instance()->Uh()[100];
}

// Nov 24
// NOTE this is not complete!! not sure if i'm going to use it in the future
//  There is a new implementation (refactoring) that i want to test
void Gamma::applyBC() const {
  // call setAmont() and setAval() in a row
  // setAmont();
  // setAval();
  // amont
  GlobalDiscretization::instance()->U1values()[0] = m_amont[0];
  GlobalDiscretization::instance()->U2values()[0] = m_amont[1];
  GlobalDiscretization::instance()->Hvalues()[0] = m_amont[2];

  // ...
  unsigned w_nbsect = GlobalDiscretization::instance()->getNbSections() - 1;

  // aval
  GlobalDiscretization::instance()->U1values()[w_nbsect] = m_aval[0];
  GlobalDiscretization::instance()->U2values()[w_nbsect] = m_aval[1];
  GlobalDiscretization::instance()->Hvalues()[w_nbsect] = m_aval[2];
}
} // namespace dbpp
