#pragma once

namespace dbpp 
{
	/** Brief Class that hold st-venant equations terms (flux and source).
	*   These terms are evaluated according to physical algorithm. For example
	*   flux can be splitted in two parts: pressure and convective flux where each  
	*   can be treated with numerical discretization such as finite difference scheme 
	*   and Riemann algorithm (shock-capturing scheme to model discontinuity accurately).
	*/
	class HydroTerms
	{
	public:
		/** default ctor. 
		*/
		HydroTerms();
		/** Destructor.
		*/
		~HydroTerms();
		//	Fonction spécifique à un canal rectangulaire de largeur B
		//	Pression hydrodynamique établi en fonction d'une superficie d'écoulement A
		/** Compute hydrodynamic pressure for a rectangular cross-section of width B
		  * in term of wetted area A.
		*/
		static double CalculTermePressionHydrostatique1D (double A, double B=1.);
		//	Fonction de calcul de F2 complet (incluant le terme de pression hydrostatique)
		/** 
		*   Compute face flux considering pressure and convective parts
		*/
		static double EvaluationF2_C_1D (double U1, double U2, double B=1.);
		//	Fonction de calcul de F2 incomplet (excluant le terme de pression hydrostatique)
		/** 
		*  Compute face flux without pressure term
		*/
		static double EvaluationF2_I_1D (double U1, double U2, double B=1.);
	private:
	};
} // End of namespace