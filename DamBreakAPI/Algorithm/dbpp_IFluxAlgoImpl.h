#pragma once

// stl includes
#include <list>
#include <vector>
// package includes
#include "../Utility/dbpp_HydroCodeDef.h" // StateVector definition
#include "../SfxTypes/dbpp_cellFace.h"

namespace dbpp 
{
	/** Brief Interface that provides services to compute face flux
	 *  (numerical flux) at cell face.
	 *
	 *  Test implementation of the flux algorithm, provides the service
	 * for calculating flux face (numerical flux at cell face).
	 */
	class IFluxAlgoImpl
	{
	public:
		/**
		 *   Some typedef for code clarity
		 */
		typedef std::vector<std::pair<double/*UL*/,double/*UR*/>> cellfaceVar;
	public:

		/** dtor
		*/
		virtual ~IFluxAlgoImpl() {/*nothing to do*/}
		
		/** this is one of many implementation of this algorithm
		* this signature we have removed the reconstruction procedure (MUSCL)
		* it will be done outside the flux algorithm. 
		*/
		virtual void calculFF( std::vector<double>& FF1, std::vector<double>& FF2, 
			const cellfaceVar& aU1LR, const cellfaceVar& aU2LR)=0;
 		
 		/** Compute numerical flux at cell interface
		*/  
 		virtual void calculFF( const std::list<cellFace>& aListOfface, 
			std::vector<double>& FF1, std::vector<double>& FF2)=0;
	protected:
	private:
	};

} // End of namespace