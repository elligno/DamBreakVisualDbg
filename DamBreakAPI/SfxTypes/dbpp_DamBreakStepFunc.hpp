#pragma once

// stl include
#include <functional>
// boost include
#include <tuple>
// project include
#include "../Discretization/dbpp_NodalVariables.h"

namespace dbpp
{
	/** Design Note (Jean Belanger june 28, 2015).
	* -----------
	*   this structure is redundant since we have the DammBreakData
	*   structure that is responsible to hold discretization data to
	*   simulate the dam-break problem (support 2 discretization).
	* retrieve the spatial discretization
	* similar to the gridLattice concept developed before
	*/

	/** Structure that hold discretization parameters. 
	*/
	typedef struct DiscrInfo 
	{
		/** 
		 */
		typedef int int32;
		typedef float float32;
		typedef double float64;

		/** default ctor (default parameters)*/
		DiscrInfo()
		{  
			/** configure the simulation property according 
			 * to Eric McNeil original source code. 
			 */
			m_DegOfFreedom = 4;             /**< (A,Q,H) and X as coordinate */ 
			m_Phi0 = 1.f;                   /**< water level at upstream */
			m_Phi1 = 10.f;                  /**< water level at downstream */
			m_NumOfGridPts = 101;           /**< number of computational nodes */
			m_Xmin = 0.f;                   /**< x-coordinate of the left bound. node*/
			m_Xmax = 1000.f;                /**< x-coordinate of the right bound. node*/
			m_domainExtent.first=m_Xmin;    /**< .. */
			m_domainExtent.second=m_Xmax;   /**< .. */
			m_dambreakExtent.first=m_Phi0;  /**< .. */
			m_dambreakExtent.second=m_Phi1; /**< .. */
			assert(m_Xmin < m_Xmax);        /**< .. */
			
			// spatial step (computational domain is 0,...,99 which is 100 points)
			m_dX= // NOTE: in his code Eric consider 101 points [0,100], last node is not part of the   
				(m_Xmax-m_Xmin)/boost::numeric_cast<float32>( m_NumOfGridPts-1); // domain but use as bnd. node
		//	assert(m_Phi0>m_Phi1);
		}

		/** Ctor from ... to be completed
		  *  
			*/
		DiscrInfo( int32 aNx, 
			const std::pair<float32,float32> aVal = 
			std::make_pair(0.f, 1.f))   // default value
		{
			m_domainExtent=aVal;
			m_NumOfGridPts=10;
			m_dX=
				(m_Xmax-m_Xmin)/boost::numeric_cast<float32>(m_NumOfGridPts-1);  // spatial step 
			m_DegOfFreedom=4;
			m_Phi0 = 1.f;          // water level at upstream
			m_Phi1 = 0.5f;         // water level at downstream
			m_dambreakExtent.first=m_Phi0;
			m_dambreakExtent.second=m_Phi1;
			m_Xmin=m_domainExtent.first;
			m_Xmax=m_domainExtent.second;
			assert(m_Xmin < m_Xmax);
			assert(m_Phi0 > m_Phi1);
		}
		/** 
		  * @result minimum coordinate
			*/
		float32 x_min() const { return m_Xmin;}
		/** 
		  * @result maximum coordinate  
			*/
		float32 x_max() const { return m_Xmax;}
		/** 
		  * @param input file (parameters)  
			*/
		void readFromFile( std::string aFileName/* = */)
		{/*not implemented yet*/}

		std::pair<float32,float32> m_domainExtent;   /**< grid coordinate [min,max]*/
		std::pair<float32,float32> m_dambreakExtent; /**< .. */
		float32 m_dX;                                /**< spatial step */
		int32   m_DegOfFreedom;                      /**< degree of freedom (A,Q,H)*/
		float32 m_Phi0;                              /**< water level at upstream*/
		float32 m_Phi1;                              /**< water level at downstream*/
		int32   m_NumOfGridPts;                      /**< number of computational nodes*/
		float32 m_Xmin;                              /**< x-coordinate of the left bound. node*/
		float32 m_Xmax;                              /**< x-coordinate of the right bound. node*/
	} DiscrInfo;                                   /**< discretization parameters*/

		/** Function or functor that initialize a nodal value.
		  * For example it could be the step function for 
		 * the dam-break problem.
		 *
		 * Code usage:
		 * > DiscrInfo w_Disrc; 
		 * >
		 * > //initializer (unary functor) which set initial values 
		 * > DamBreakStepFunc w_testForeachptr(w_Disrc);
		 * >
		 * > // initialization of nodal values (H,Q)
		 * > for_each( m_nval.begin(), m_nval.end(), w_testForeachptr);
		 * >
		 * > // section flow unit width and flat bed (then we can set A=H)
		 * > for_each( m_nval.begin(), m_nval.end(), // range
		 * > [](Nodal_Value& aNodeVal){aNodeVal[0]=aNodeVal[2];});
		*/
		class DamBreakStepFunc : 
			public std::unary_function< Nodal_Value, void>
		{
		public:
			// default ctor
			DamBreakStepFunc( DiscrInfo aDiscr) 
			: m_Discr( aDiscr) // at the beginning 
			{}

			// to be used with the for_each algorithm
			result_type operator() (argument_type& aNval) const
			{
				// retrieve the node attributes (i,x) 
				Nodal_Value::nodeIdX w_idx = aNval.getIdX();
				aNval[2] = m_Discr.m_Phi0;              // H
				if( w_idx.second >= m_Discr.x_max()/2.) // breaking point
					aNval[2] = m_Discr.m_Phi1;            // H
				aNval[1] = 0.;                          // Q
			}
		private:
			// discretization (shall we use the shared pointer)
			DiscrInfo m_Discr; /*< discretization parameter*/
		};
} // End of namespace