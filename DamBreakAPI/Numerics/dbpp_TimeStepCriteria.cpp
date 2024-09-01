
// STL include
#include <algorithm>
// App includes
#include "../SfxTypes//dbpp_PhyConstant.h"
#include "dbpp_TimeStepCriteria.hpp"

namespace dbpp {
double TimeStepCriteria::timeStep(const dbpp::scalarField &aU1,
                                  const dbpp::scalarField &aU2) {
  // return time step computed
  using namespace std;

  const dbpp::RealNumArray<real> &w_U1 = aU1.values(); // (A)
  const dbpp::RealNumArray<real> &w_U2 = aU2.values(); // (Q)
  // const int w_nx = aU1.grid().getBase(1); // index of the first grid node
  const int w_nbPts = aU1.grid().getNoPoints();
  const real dx = aU1.grid().Delta(1); // in x-direction (shall be 10)

  // dt = BIG_dt;  huge double value
  real dt = std::numeric_limits<real>::max();
  const double w_grav = PhysicalConstant::sGravity;

  for (int j = 2; j <= w_nbPts; j++) // start j=2, i think j=1 is tied node
  {
    double V = w_U2(j) / w_U1(j);        // ...
    double c = ::sqrt(w_grav * w_U1(j)); // unit width
    double dtc = dx / (::fabs(V) + c);   // ...
    dt = std::min(dtc, dt);              // ...
  }

  // debugging purpose don't really need to do that, actually this is not
  // as important as the simulation time, since we are comparing with E. McNeil
  // source code (see file 'debug de la fonction de controle')
  cout << "\nAutomatic variable adjustment of time step to " << dt << endl;

  // Calcul de dt (shall be done outside the method, this method
  // compute time step, multiplication by cfl number not here, when
  // call the increase time step in the main simulation time loop)
  return dt;
}
} // namespace dbpp
