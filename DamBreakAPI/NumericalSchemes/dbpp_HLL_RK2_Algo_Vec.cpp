
// stl includes
#include <cassert>
#include <iostream>
#include <memory> // smart pointer
// STL numeric library
#include <numeric>
// WaveSimulator package includes
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "dbpp_HLL_RK2_Algo_Vec.h"

namespace dbpp {

// default implementation with E. MCNeil parameters
EMcNeil1D::EMcNeil1D() {
  // set pointer to numerical representation (Observer Design Pattern)
  dbpp::GlobalDiscretization::instance()->num_rep() = this;
}

//	Exécution de la phase intermédiaire de calcul:
//	schéma d'intégration Runge-Kutta (Nujic, 1995)
//
//	Au cours de cette phase, une valeur intermédiaire est déterminée
//	comme suit:
//
//	Up = U - dt/dx * (F[j+1/2] - F[j-1/2]) - (dt/dx)*P - dt*S
// ...
// stepping through time (pair of smart pointer)
// NOTE: in this version (single iteration, we have reproduced the result).
// But there is a problem, by using the std::vector and assigning values,
// work on copy (vector own it's element), we make change on vector element
// but this doesn't affect the StateVector element value, even if we pass it
// as reference and use reference on RealNumArray, because vector takes the
// ownership of the values. What we have to do, copy vector into RealNumArray.
// Or use vector of pointer (pointer vector, std::vector<double*> or
// boost::ptr_vector)
void EMcNeil1D::advance() {
  // advance the physics algorithm (overided by subclass)
  // Template Design Pattern
  timeStep();
}

void EMcNeil1D::setState() {
  // all copy that we are doing at the end of advance(...)
  // then call notify to all attached observer and update()
  // for global discretization

  using namespace std::placeholders;

  // notify section flow (notify all observers)
  // notify();  call observer update() method

  // update the global discretization
  dbpp::GlobalDiscretization::instance()->update();

  // update boundary condition (need to check for this one)
  dbpp::GlobalDiscretization::instance()->gamma().applyBC();
}
} // namespace dbpp
