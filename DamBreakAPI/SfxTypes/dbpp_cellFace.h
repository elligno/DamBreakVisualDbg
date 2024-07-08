#pragma once

// C++ includes
//#include <list>
//#include <tuple>
#include <Utility>
#include <iostream>
// boost include
#include <boost/operators.hpp>

namespace dbpp
{
	/**
	* j+1/2 cell face in the finite volume discretization
	* Usage:
	*   caculFF(const cellFace& aFace) compute the numerical flux
	*   with Godunov-type scheme for example F(UL,UR) with Approximate
	*   Riemann Solver (discontinuity or shock represented by cell face).
	*   Also we have a concept of cell or element of our discretization
	*   which is an aggregate of cell face (actually a pair).
	*   In this implementation we consider 2-components state vector
	*   that correspond to (A,Q) state variable (UL1,UL2) and (UR1,UR2).
	*
	*   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//   In our programming environment (EMcNeil discretization), the first 
	//   cell face has the following parameters:
	//   left index=0, right index=1 (global discretization node)
	//   cell face index=0 to 99 (there is 100 cell face) and the last one
	//   left index=99 (last node of the mesh), right index=100 (ghost node for BC)
	//   with cell face index = 99 (total cell face=100). Then the first cell face
	//   x_1/2 and x_3/2 coordinate and the last cell face
	//   cellface_0 = pair of cell face (0,1)/(1,2) because the first node is a tied node
	//   for now we assume this discretization. Create a cell which is made of a pair 
	//   of cell face. 
	//   
	//   Design Note:
	//      
	//      Inherit from boost operator library
	*/      
	class cellFace : public boost::equality_comparable<cellFace>, // strickly comparable
		                      boost::less_than_comparable<cellFace> // less than comparable
	{
	public:
    // shall it be public? may be, because client may need access
		typedef std::pair<double,double> pairvec;
		typedef std::pair<unsigned/*left node index*/,unsigned/*right node index*/> pairnodeI;
		typedef std::pair<double/*left node x*/,double/*right node x*/> pairnodeX;
	public:
		//enum class faceID {left=0,right=1};  what does that mean?
		// be careful with default ctor (read again Scott Meyers)
		// most of the time we need a default ctor when using array or stl container
		// yeap! it's right, since the cell face will stored in a container
		cellFace();
		
		// design note: this won't work because of the default argument value
		// if there is o left neighbor, user need to specify explicitly
		// but in this case we cannot set false to aLneighbour because 
		// the next parameter has to be fixed or something like this 
		cellFace( unsigned short aCellFaceIdx, // global index (global discretization) 
              unsigned short aLeftNodeIdx, unsigned short aRightNodeIdx,   // grid node index left/right on both side
			        /*double aLeftx, double aRightx,*/ bool aLneighbour=true, // x coord. on both side  
				      bool aRneighbour=true);                                   // boundary check at both end
		
//     cellFace(const std::pair<cellFace,cellFace>& aOther)
//     {
//       std::cout << "Just testing copy constructible\n";
//     }

    // compiler default ctor are probably fine
//     cellFace(const cellFace& aOther)
//     {
//       std::cout << "To be implemented\n";
//     }
// 
//     cellFace& operator= (const cellFace& aOther)
//     {
//       return *this;
//     }

		// i don't get it??
    //cellFace( unsigned aId, unsigned aLeftI, double aLeftx, double aDx);

		// default copy and assignment ctr will do
		~cellFace() { std::cout << "CellFace dtor\n";}

		void setLeftNodeI( const unsigned short aLnodeI)  { m_leftNodeI=aLnodeI; }
		void setRightNodeI( const unsigned short aRnodeI) { m_rightNodeI=aRnodeI;}
		void setCellFaceI( const unsigned short aCellFI)  { m_cellFaceI=aCellFI; }
		unsigned short getLeftNodeI()  const { return m_leftNodeI;  }
		unsigned short getRightNodeI() const { return m_rightNodeI; }
		unsigned short getCellFaceI()  const { return m_cellFaceI;  }

    //		double   x_i12() const         {return m_x12;}
		bool hasLeftNeighbour() const  { return m_hasLeftNeighbour;}
		bool hasRightNeighbour() const { return m_hasRightNeigbour;}

		// comparison operator (boost operator equality_comparable)
		// operator != is defined
		friend bool operator== (const cellFace& aFace1, const cellFace& aFace2)
		{
			return ( aFace1.m_cellFaceI==aFace2.getCellFaceI() && 
				     aFace1.getLeftNodeI()==aFace2.m_leftNodeI &&
					 aFace1.getRightNodeI()==aFace2.getRightNodeI());
		}
		
    // boost less than comparable semantic (not completed)
		friend bool operator< ( const cellFace& lhs, const cellFace& rhs)
		{
			return lhs.m_cellFaceI<rhs.m_cellFaceI && lhs.m_leftNodeI<rhs.m_leftNodeI;
		}
		
    // can be useful when debugging (printing information)
		friend std::ostream& operator<< ( std::ostream& os, const cellFace& aCface)
		{
			os <<"Cell face Id is: " <<  aCface.m_cellFaceI << "\n" 
			   << "Left Node Id is: " << aCface.m_leftNodeI << "\n"
			   << "Right Node Id is: " << aCface.m_rightNodeI << "\n";

			return os;
		}
	private:
		// i am not sure if we should have x-coord. as attribute?
// 		double m_x12;            // x_i+1/2=(x_i+x_i+1)/2.
// 		double m_leftX;          // x_i
// 		double m_rightX;         // x_i+1
//		unsigned m_cellIdx;      // 
		unsigned short m_cellFaceI;    // face index (i=1,100) in this case
		unsigned short m_leftNodeI;    // node index on left
		unsigned short m_rightNodeI;   // node index on right
// 		pairvec m_leftVec;       // UL1,UL2
// 		pairvec m_rightVec;      // UR1,UR2
		bool m_hasLeftNeighbour; // left node/index ???
		bool m_hasRightNeigbour; // right node/index ??? 
	}; // class

	// just a test about a concept of faces variables
// 	class CellFaceVariables
// 	{
// 	public:
// 		CellFaceVariables() {}
		// getter and setter
		// 		void setUL( pairvec aUL) {m_leftVec=aUL;} 
		// 		void setUL( unsigned aI) {aI==0 ? std::get<0>(m_leftVec)=aI : std::get<1>(m_leftVec)=aI;}
		// 		void setUR( pairvec aUR) {m_rightVec=aUR;}
		// 		void setUR( unsigned aI) {aI==0 ? std::get<0>(m_rightVec)=aI : std::get<1>(m_rightVec)=aI;}
		// 		pairvec getUL() const    {return m_leftVec;}
		// 		pairvec getUR() const    {return m_rightVec;}
		// 		double getUL( unsigned aI) const 
		// 		{
		// 			return aI==0 ? std::get<0>(m_leftVec) : std::get<1>(m_leftVec);
		// 		}
		// 		double getUR( unsigned aI) const 
		// 		{
		// 			return aI==0 ? std::get<0>(m_rightVec) : std::get<1>(m_rightVec);
		// 		}
		// read-only
		// 		StateVariables UL() const { return m_UL;}
		// 		StateVariables UR() const { return m_UR;}
		// 
		// 		// write access
		// 		StateVariables& UL() { return m_UL;}
		// 		StateVariables& UR() { return m_UR;}
		// 
		// 		// Set the state variable
		// 		void setUL( const StateVariables aUL) {m_UL=aUL;}
		// 		void setUR( const StateVariables aUR) {m_UR=aUR;}
// 	protected:
// 	private:
// 	};
} // End of namespace
