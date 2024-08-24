// project includes
#include "dbpp_HydroTerms.h"
//#include "../HydroCodeDef.h"
#include "../SfxTypes/dbpp_PhyConstant.h"

namespace dbpp 
{
	// ...
	HydroTerms::~HydroTerms()
	{

	}

	HydroTerms::HydroTerms()
	{

	}

	double HydroTerms::CalculTermePressionHydrostatique1D(double A, double B/*=1.*/)
	{
		//	Fonction spécifique à un canal rectangulaire de largeur B
		//	Pression hydrodynamique établi en fonction d'une superficie d'écoulement A

		double I;

		I = A*A/(2.*B);

		return (I);
	}

	double HydroTerms::EvaluationF2_C_1D(double U1, double U2, double B/*=1.*/)
	{
		//	Fonction de calcul de F2 complet (incluant le terme de pression hydrostatique)

		double F2, I;

		I = CalculTermePressionHydrostatique1D (U1, B);
        const double w_grav=PhysicalConstant::sGravity;
		F2 = U2*U2/U1 + w_grav*I;

		return (F2);
	}

	double HydroTerms::EvaluationF2_I_1D(double U1, double U2, double B/*=1.*/)
	{
		//	Fonction de calcul de F2 incomplet (excluant le terme de pression hydrostatique)

		double F2;

		F2 = U2*U2/U1;

		return (F2);
	}

} // end of namespace

