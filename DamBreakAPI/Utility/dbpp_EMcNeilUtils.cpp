// C includes
#include <cmath>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
// STL include
#include <algorithm>
// project include
#include "dbpp_EMcNeilUtils.h"
#include "dbpp_TestLogger.h"

namespace dbpp {
/**
 * Utility function to evaluate state variable wetted area
 * of a cross-section from water level)
 */
double HydroUtils::Evaluation_A_fonction_H(double H, double Z, double B) {
  double Resultat;

  Resultat = (H - Z) * B;

  return (Resultat);
}

// Usage: vector<SectFloow*> want to compute wetted area
// compute geometric property of the section flow
//   double  HydroUtils::Evaluation_A_fonction_H( const SectFlow* aSectFlow)
//   {
//     const double H = aSectFlow->H(); // water level
//     const double Z = aSectFlow->Z(); // bathymetry
//     const double B = aSectFlow->B(); // section width
//
//     return (H-Z)*B;
//   }

/**
 * Utility function to evaluate state variable wetted area
 * of a cross-section from water level)
 */
double HydroUtils::Evaluation_H_fonction_A(double A, double B, double Z) {
  double Resultat;

  Resultat = A / B + Z;

  return (Resultat);
}

//   double HydroUtils::Evaluation_H_fonction_A (const SectFlow* aSectionF)
//   {
//     const double A = Evaluation_A_fonction_H(aSectionF);
//     const double B = aSectionF->B();
//     const double Z = aSectionF->Z();
//
//     return A/B + Z;
//   }

/**
 * Utility function to evaluate state variable wetted area
 * of a cross-section from water level)
 */
double HydroUtils::minmod(double a, double b) {
  double Resultat;

  if (a * b <= 0.)
    Resultat = 0.;

  else if ((fabs(a) < fabs(b)) && (a * b > 0.))
    Resultat = a;

  else if ((fabs(b) < fabs(a)) && (a * b > 0.))
    Resultat = b;

  else if (fabs(b) == fabs(a)) //	ATTENTION, mis pour vérification !!!!
    Resultat = a;
  else {
    //	printf("Fonction MINMOD: situation ne correspondant pas a celle
    // attendue\n");
    auto *w_msg = "Fonction MINMOD: situation ne correspondant pas a celle "
                  "attendue, leaving program\n";

    dbpp::Logger::instance()->OutputError(const_cast<char *>(w_msg));

    //  "Fonction MINMOD: situation ne correspondant pas a celle "
    //            "attendue, leaving program\n");
    // throw std::exception("Fonction MINMOD: situation ne correspondant pas a
    // celle attendue");
    exit(101); // don't do that! at least throw an exception!!
  }

  return (Resultat);
}

/**
 * @param
 * @param
 */
double HydroUtils::R(double A, double B) {
  double Resultat;

  Resultat = A / P(A, B);

  return (Resultat);
}

/**
 * @param
 * @param
 */
double HydroUtils::P(double A, double B) {
  double Resultat;

  Resultat = 2. * A / B + B;

  return (Resultat);
}

/**
 * @param
 * @param
 */
double HydroUtils::A(double B, double Y) {
  double Resultat;

  Resultat = B * Y;

  return (Resultat);
}

/**
 * @param
 * @param
 */
double HydroUtils::T(double B) {
  double Resultat;

  Resultat = B;

  return (Resultat);
}
} // namespace dbpp
