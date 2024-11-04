
// library include
#include "../SfxTypes/dbpp_scalarField.h"

// typedef's from BaseNumTypes library used through this project 
//typedef elgo_ptr<jb::scalarField> fieldptr; deprecated
/** 
*  Model of scalar field
*/
typedef std::shared_ptr<dbpp::scalarField> fieldptr; 
/** 
*  Model of a grid lattice
*/
typedef std::shared_ptr<dbpp::gridLattice> Mesh1D;  //one-dimensional; grid x=0,...,1000
/** 
*  Model of a flux tensor
*/
typedef std::pair<dbpp::RealNumArray<real>,dbpp::RealNumArray<real>> fluxtensor;

/** 
*   state vector that represent the physical variables (A,Q)
*/
typedef std::pair<fieldptr,fieldptr> StateVector; // U=(A,Q)
/** 
*  pair of double
*/
typedef std::vector<std::pair<double,double>> vecpaird;
/** 
*  variable for reconstruction  procedure at cell face
*/
typedef std::vector<std::pair<double,double>> vecULR; // vector that hold the cell face variable