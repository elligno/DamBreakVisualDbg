
#include "dbpp_EMcNeilBCimpl.h"

namespace dbpp 
{

  dbpp::Nodal_Value EMcNeilBCimpl::getDownstreamValues(const Nodal_Value& aNval, /*Uh[99],last node and section computational domain*/ 
    const SectFlow* aLastSection/*99*/, const SectFlow* aGhostSection/*100*/, double aDt)
  {
    // if( Gamma BCtypes Hfixed/Qzero) open right exit
    // { set discharge to zero (at rest at t=0)
       //U2[NbSections - 1] = 0.; Discharge (fluid at rest)
    // }

     // because we reserve i=0,...,99 need to add i=100
     // // Aval, spécification du débit
     //U2[NbSections - 1] = 0.; // because we reserve and set i=0,...,99

  //   H[NbSections - 1] = (CP / CA) + 0./*w_lastSect->Z()*/; //Z[NbSections-1];
    const auto H = BCharacteristicSolver::computeDownstreamValues(aNval, *aLastSection, aDt) + aGhostSection->Z();

    //   U1[NbSections - 1] =
    //     HydroUtils::Evaluation_A_fonction_H(H[NbSections - 1], 0./*w_lastSect->Z()Z[NbSections-1]*/, 1./*w_lastSect->B()*/);
    const auto U1 = HydroUtils::Evaluation_A_fonction_H(H, aGhostSection->Z(), aGhostSection->B());

    // Consider fluid at rest, means that Q=0. (U2 variable)
    return Nodal_Value{ {aGhostSection->getId(), aGhostSection->X()},3,std::make_tuple(U1/*A*/,0./*discharge*/,H),-1 }; // U1,U2,H
  }

  dbpp::Nodal_Value EMcNeilBCimpl::getUpstreamValues( const Nodal_Value& aNval, /*Uh[99],last node and section computational domain*/ 
    const SectFlow* aFirstSection, const SectFlow* aNextSection, double aDt)
  {

    // H=Phi1
    //	Amont, spécification de l'élévation de la surface libre
    //H[0] = 10.;  water level
    const double w_H = 10.; //phi1

    //U1[0] = HydroUtils::A(B, H[0] - Z[0]); // wetted area (=A*T) where T is top width of the section
    const auto w_A = HydroUtils::A(aFirstSection->B(), w_H - aFirstSection->Z()); // wetted area

    // // set discharge Q at boundary node (characteristic)
    const auto w_Q = w_A*BCharacteristicSolver::computeUpstreamValues(aNval, *aFirstSection, *aNextSection, aDt);

    return Nodal_Value{ { aFirstSection->getId(), aFirstSection->X() },3,std::make_tuple(w_A/*A*/,w_Q/*discharge*/,w_H),-1 };
  }

} // End of namespace

