
// STL include
#include <algorithm>
// App include
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

#if 0
	// Not completed
	double TimeStepCriteria::timeStep( const std::vector<double>& aU1, const std::vector<double>& aU2)
	{
		double dt = std::numeric_limits<double>::max(); //BIG_dt;
		const double w_grav = PhysicalConstant::sGravity;
		
		// need to think about the design, for now dambreak data i noncopyable 
	//	DamBreakData::DiscrTypes w_dbType = Simulation::instance()->getActiveDBdata();
// 		const double CFL = Simulation::instance()->getCFLNumber();
// 		DamBreakData w_dbData(Simulation::instance()->getDiscrType());
// 		const double B=w_dbData.getWidth(); 
// 		const double dx=w_dbData.dx();
// 
// 		//	Calcul des valeurs des variables d'état
// 		for( std::vector<double>::size_type j = 1; 
// 			j < aU1.size()/*NbSections*/; j++)
// 		{
// 			//	Analyse des conditions de stabilité numérique
// 			const double V = aU2[j]/aU1[j];
// 			const double c = ::sqrt( w_grav*aU1[j]/HydroUtils::T(B));	// T à calculer
// 			const double dtc = dx/(::fabs(V) + c);
// 
// 			dt = std::min(dtc, dt); //DMIN (dtc, dt);
// 		}

		//	Calcul de dt
//		dt = CFL*dt;
		return dt;

		//
		// new version (Dec. 09)
		//
		// 			typedef std::binder2nd<std::multiplies<float64> > fmult;
		// 
		// 			std::vector<float64> w_V; // compute velocity
		// 			
		// 			std::transform( w_U1.begin(), w_U1.end(), w_U2.begin(),
		// 				std::back_inserter(w_V), std::divides<float64>());
		// 			
		// 			std::vector<float64> w_gh; // compute celerity
		// 			
		// 			std::transform( w_U1.begin(), w_U1.end(), // unit width 
		// 				std::back_inserter(w_gh), 
		// 				std::bind2nd( std::multiplies<float64>(), g));
		// 			
		// 			std::transform( w_gh.begin(), w_gh.end(), 
		// 				w_gh.begin(), sqroot_d()); // w_C
		// 			std::for_each( w_V.begin(), w_V.end(), fabs_d());
		// 			
		// 			std::vector<float64> w_denominator; // v+(gh)^12
		// 			std::transform( w_V.begin(), w_V.end(), w_gh.begin(),
		// 				std::back_inserter( w_denominator), std::plus<float64>());
		// 			
		// 			BOOST_FOREACH( float64 aDen, w_denominator)
		// 			{
		// 				float64 w_Dtc = w_Dx/aDen;
		// 				w_Dt          = std::min( w_Dtc, w_Dt);
		// 			}
		// 		std::vector<float64> w_Dtc;
		// 		std::transform();
		//	Calcul des valeurs des variables d'état 	
		// NOTE: why do we start looping at j = 1, it should it be j=0?
		//		for( int j = 1; j < mNbSections; j++) 
		//		{
		//	Analyse des conditions de stabilité numérique
		//float64 wV   = mU2[j]/mU1[j];
		// Compute velocity
		//			float64 w_V   = wState.second[j]/wState.first[j];
		//	float64 wC   = ::sqrt( g * mU1[j]/wT());
		//			float64 w_C   = ::sqrt( g * wState.first[j]);
		//			float64 w_Dtc = m_Dx/( ::fabs(w_V) + w_C);
		//			w_Dt          = std::min( w_Dtc, w_Dt);
		//		}

		// return the time stepping
		//return w_Dt;  debugging purpose
	}
#endif // 0
} // namespace dbpp
