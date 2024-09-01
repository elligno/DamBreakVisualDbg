
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
void EMcNeil1D::advance(/*StateVector& aU*/) {
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
  notify(); // call observer update() method

  // update the global discretization
  dbpp::GlobalDiscretization::instance()->update();

  // update boundary condition (need to check for this one)
  dbpp::GlobalDiscretization::instance()->gamma().applyBC();

#if 0 // maybe in the next version??
    // Current version these values are set inside applyBC()
    // which doesn't make sense, why this method set b.c. to global?
    // Also, shall pass Uh as argument (for now ok) but keep in mind
    // the final version will take it as an arg GblDiscr{Uh,Omega,Gamma}
    // the the following: Gamma w_bcEMcNeil{Uh} hold a reference 
    // when GblDiscr is updated, Gamma get its value up-to-date, since
    // it use reference semantic ('Uh' ptr-to-Object).
    auto w_gDiscr = dbpp::GlobalDiscretization::instance();
    auto& w_frontVal = w_gDiscr->Uh().front();
    w_frontVal[0] = 1.3;
    auto& w_backVal = w_gDiscr->Uh().back();
    w_backVal[0] = 0.3;
#endif
}

void EMcNeil1D::attach(Observer *aObs) { m_listofObs.push_back(aObs); }

void EMcNeil1D::detach(Observer *aObs) {
  // stl find algorithm return an iterator and then delete it
  if (std::find(m_listofObs.begin(), m_listofObs.end(), aObs) !=
      m_listofObs.end()) {
    // return iterator at position if found it (dereference it to get the
    // object)
    std::list<Observer *>::iterator w_found =
        std::find(m_listofObs.begin(), m_listofObs.end(), aObs);

    // we found something, must delete it
    delete *w_found;
    *w_found = nullptr;
  }
}

void EMcNeil1D::notify() {
  // in this version we have only one observer
  // no need to loop through the list
  assert(1 == m_listofObs.size());
  Observer *w_obs = m_listofObs.front();
  // sanity check
  assert(w_obs != nullptr);
  // call update method (not implemented yet, s it completed)
  w_obs->update(); // list of section flow updated
}
} // namespace dbpp
