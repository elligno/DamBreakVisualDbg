
#pragma once

#include "../SfxTypes/dbpp_ListSectFlow.h"

namespace dbpp 
{
	/** Brief Hydrodynamic utility. 
	*   Provides services to compute section geometry properties 
	*/
	class HydroUtils 
	{
	public:
		//	Fonction spécifique à un canal rectangulaire de largeur B

		/** Rectangular cross-section
		* @param section width
		* @result cross-section ... 
		*/
		static double T (double B);
		
		/** Rectangular cross-section
		* @param section width
		* @param water depth
		* @result cross-section wetted area  
		*/
		static double A (double B, double Y);
		//	Fonction spécifique à un canal rectangulaire de largeur B
		//	Périmètre établi en fonction d'une superficie d'écoulement A
		
		/** Rectangular cross-section
		*  @param section wetted area
		*  @param section width
		* @result wetted perimeter 
		*/
		static double P (double A, double B);
		//	Fonction spécifique à un canal rectangulaire de largeur B
		//	Périmètre établi en fonction d'une superficie d'écoulement A
		/** Rectangular cross-section
		* @param section width
		* @result  
		*/
		static double R (double A, double B);
		/** Rectangular cross-section
		* @param first value
		* @result  
		*/
		static double minmod (double a, double b);
		// some utility 
//    static double Evaluation_H_fonction_A (const SectFlow* aSectionF);
		
		/** Rectangular cross-section
		* @param water height 
		* @param bathymetry
		* @param section width
		* @result wetted area  		*/
		static double Evaluation_H_fonction_A (double A, double B, double Z);
  
		//  static double Evaluation_A_fonction_H (const SectFlow* aSectionF);
		/** Rectangular cross-section
		* @param water height 
		* @param bathymetry
		* @param section width
		* @result wetted area  
		*/
		static double Evaluation_A_fonction_H (double H, double Z, double B);
	};
} // End of namespace