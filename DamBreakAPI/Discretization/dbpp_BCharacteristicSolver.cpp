#include "dbpp_BCharacteristicSolver.h"

namespace dbpp 
{
  double BCharacteristicSolver::computeDownstreamValues(const Nodal_Value& aNval, /*Uh[99],last node and section computational domain*/ 
    const SectFlow aLastSection/*99*/, double aDt)
  {
    const auto w_nval = aNval.Values(); // tuple values
    const auto grav = PhysicalConstant::sGravity;

    const auto n1 = aLastSection.N(); //n[NbSections-2];
    const auto V1 = std::get<1>(w_nval) / std::get<0>(w_nval);  //U2[NbSections - 2] / U1[NbSections - 2];
    const auto R1 = HydroUtils::R(std::get<0>(w_nval), aLastSection.B());   //HydroUtils::R(U1[NbSections - 2], 1./*w_sectDown->B()*/);
    const auto Y1 = std::get<2>(w_nval) - aLastSection.Z();  //H[NbSections - 2] - 0./*w_sectDown->Z()*/;  //Z[NbSections-2];
//       const double CA =
//         ::sqrt(PhysicalConstant::sGravity* U1[NbSections - 2] / HydroUtils::T(1./*w_sectDown->B()*/));
    const auto CA =
      ::sqrt(PhysicalConstant::sGravity*std::get<0>(w_nval) / HydroUtils::T(aLastSection.B()));

    // Manning formula
    const auto Sf1 = n1*n1*V1*::fabs(V1) / ::pow(R1, 4. / 3.);
    // retrieve current simulation time step
   // const double dt = Simulation::instance()->simulationTimeStep();

    // Characteristic #1
    const auto CP = V1 + CA*Y1 + PhysicalConstant::sGravity*(aLastSection.getS0am() - Sf1)*aDt;

    // because we reserve i=0,...,99 need to add i=100
    //H[NbSections - 1] = (CP / CA) + 0./*w_lastSect->Z()*/; //Z[NbSections-1];
    return (CP / CA);
  }

  double BCharacteristicSolver::computeUpstreamValues(const Nodal_Value& aNval/*next downstream*/, const SectFlow aSectFlow0/*firstt downstream*/, const SectFlow aSectFlow /*next downstream*/, double aDt)
  {

    const auto w_nval = aNval.Values(); //tuple values
    const auto grav = PhysicalConstant::sGravity;

    // Calcul des caractéristiques à la section située directement en aval
    const auto n1 = aSectFlow.N(); //n[1];  manning
    const auto V1 = std::get<1>(w_nval) / std::get<0>(w_nval);  //U2[1] / U1[1]; // velocity
    const auto R1 = HydroUtils::R(std::get<0>(w_nval), aSectFlow.B());   //HydroUtils::R(U1[1], B);  hydraulic radius
    const auto Y1 = aSectFlow.H() - aSectFlow.Z();    //H[1] - Z[1];  water depth

    // because we are considering a rectangular channel
    // see reference (Modeling Of Open Channel Flow, chapter #2)
    const auto CB = ::sqrt(grav*std::get<0>(w_nval) / HydroUtils::T(aSectFlow.B()));   //::sqrt(grav*U1[1] / HydroUtils::T(B));

    // Manning formula (see documentation)
    const auto Sf1 = n1*n1*V1*::fabs(V1) / ::pow(R1, 4. / 3.);

    // simulation current time step
    //const double dt = Simulation::instance()->simulationTimeStep();

    // see again documentation (characteristics equation solution
    // i do not understand yet the *Y1, probably a unit  
    const auto CN = V1 - CB*Y1 + grav*(aSectFlow.getS0av() - Sf1)*aDt; // negative C characteristic (subcritical regime) 

    // i do not understand this line (i don't get it)
    const auto V0 = CN + CB*(aSectFlow0.H() - aSectFlow0.Z()); //CB*(H[0] - Z[0]);  velocity at upstream along characteristic

    return V0;
  }

} // End of namespace

