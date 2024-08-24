
#include "dbpp_cellFace.h"

namespace dbpp 
{
  // we really need this ctor?
	cellFace::cellFace() 
	: m_cellFaceI(0), 
	  m_leftNodeI(0), 
	  m_rightNodeI(0),

	  m_hasLeftNeighbour(false),
	  m_hasRightNeigbour(false)
	{
	}

	cellFace::cellFace( unsigned short aCellFaceI, unsigned short aLeftI, unsigned short aRightI,/* double aLeftx,*/ 
		/*double aRightx,*/ bool aLneighbour/*=true*/, bool aRneighbour/*=true*/) 
	: m_cellFaceI(aCellFaceI), 
	  m_leftNodeI(aLeftI), 
	  m_rightNodeI(aRightI),
	  m_hasLeftNeighbour(aLneighbour),
	  m_hasRightNeigbour(aRneighbour)
	{
		// compute the cell interface position along the grid
//		m_x12=(m_leftX+m_rightX)/2.;
	}

// 	cellFace::cellFace(unsigned aId, unsigned aLeftI, double aLeftx, double aDx) 
// 	: m_cellFaceI(aId),
// 	  m_leftNodeI(aLeftI)
//	  m_leftX(aLeftx)
//	{
		// compute ...
//		m_rightNodeI=m_leftNodeI+1;
// 		m_rightX=m_leftX+aDx;
// 		m_x12=(m_leftX+m_rightX)/2.;
//	}

} // End of namespace