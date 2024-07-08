
// C++ include
#include <iostream>
// boost include
#include <boost/range/iterator_range.hpp>
// Package includes
#include "../Numerics/dbpp_TestCalculFF.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
#include "../Utility/dbpp_TestLogger.h"
#include "dbpp_TestEMcNeilVec.h"

namespace dbpp {
TestEMcNeilVec::TestEMcNeilVec()
    : EMcNeil1D(),    // base class implementation
      m_NbSections(0) // default value
{
  // is empty, if so give a default value
  const unsigned NbSection = dbpp::Simulation::instance()->getNbSections();
  m_NbSections = NbSection;

  // Again, we need to do basic check, especially if U,U2
  // check size of the U1 and U2
  if (U1.empty()) {
    m_U1p.resize(m_NbSections);
  } else {
    m_U1p.resize(U1.size());
  }
  if (m_U2p.empty()) {
    m_U2p.resize(m_NbSections);
  } else {
    m_U2p.resize(U2.size());
  }

  //  dbpp::Logger::instance()->OutputSuccess(
  //      "TestEMcNeilVec::TestEMcNeilVec() ctor");

  //  dbpp::Logger::instance()->OutputSuccess(
  //      "Using RHS Algorithm: TestRhsImpl ...");

  auto w_msg =
      "Active RHS Discretization: BaseNumTreatment with TestCalculFF flux ";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));
}

// TestEMcNeilVec::~TestEMcNeilVec() {
//  dbpp::Logger::instance()->OutputSuccess(
//      "TestEMcNeilVec::TestEMcNeilVec() dtor");
//  // must clear but i don't think it's needed
//}

// Design Note:
// ------------
//  In this implementation we are using the 'predictor' and 'corrector'
//  implementation of the base class (use std::vector). But another
//  implementation could make use of of an integrator (explicit)
//  in which the rhs is pass as argument.
// some implementation of advance (i am just exploring some design
// and testing).
// We are using the new concept of the RHS algorithm (step of the algo)
// wrapped inside of the algo, this way we can make different algorithm
// base on the numerical representation (treatment of the rhs).
//
// 		std::cout << "Upstream bc values: " << U1[0] << " " << U2[0] <<
// "
// "
// << H[0] << "\n"; 		std::cout << "Downstream bc values: " << U1[100]
// << " " << U2[100] << " " << H[100] << "\n";
//
// DESIGN NOTE: this algorithm need to be re-factored, there is a lot of crapt.
// Basically the skeleton of the way it should be is there, in the future we
// manipulate the SweRhsAlgorithm that is responsible for applying the physical
// algorithm (for example Nujic take into ... and introduce a new concept of
// compatibility relationship). All of this is under construction, but we are
// on the right track.
void TestEMcNeilVec::advance() {
  using namespace std;
  using namespace boost;

  // create a Rhs algorithm (actually it's more an implementation)
  // in this algorithm a reconstruction procedure of type MUSCL
  // (freconstr_vec() is version in use)
  unique_ptr<SweRhsAlgorithm> w_ptr2SweRhs(new TestRhsImpl);

  // create numerical discretization for this algorithm
  unique_ptr<BaseNumTreatmemt> w_rhsDiscr(new TestCalculFF);

  // apply algorithm (rhs numerical treatment of convective flux, source terms)
  // fill the SWERHS structure (to be used in our time stepper from base class)
  w_ptr2SweRhs->calculate(m_U12, w_rhsDiscr.get());

  // retrieve the rhs computed value, because we need it
  // in the predictor step ()
  //  typedef SweRhsAlgorithm::SWERHS swe_rhs;
  m_rhs = w_ptr2SweRhs->getRHS();

  // U1p, U2p set bc values
  setBC();

  // step through time (n+1/2) by using Runge-Kutta family integrator
  // don't have much choice, because advance is call only once. If 2-steps
  // algorithm is needed we have toi do it inside.
  predictor(); // U1p and U2p up-to-date, will be used for next step

  // write to debug file
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(m_NbSections, m_U1p, m_U2p));

  // ++++++ final time step +++++++++++++

  // state vector for intermediate state (need to be updated)
  auto &w_rU1p = m_U12p.first->values();
  auto &w_rU2p = m_U12p.second->values();

  // Jean Belanger test (April 2019)
  // ifd this works, could replace std::copy, is it more efficient??
  // 		jb::RealNumArray<real> w_TestCpy(m_U1p.size()-1,m_U1p.data());
  // 		w_rU1p = w_TestCpy; // copy constructible?? not sure about this

  // next step is to do the final step (n+1)
  // it's not efficient, but we are not concern with efficiency for now
  copy(m_U1p.cbegin(), m_U1p.cend() - 1,
       w_rU1p.getPtr()); // last node not part of
  copy(m_U2p.cbegin(), m_U2p.cend() - 1,
       w_rU2p.getPtr()); // the computational domain

  // apply algorithm (rhs numerical treatment of convective flux, source terms)
  w_ptr2SweRhs->calculate(m_U12p, w_rhsDiscr.get()); // updated values

  // rhs computed values
  m_rhs = w_ptr2SweRhs->getRHS();

  // n+1 time step
  corrector();

  // notify all observers
  setState();
}

// those equations are evaluated over each cell
// then for each cell we compute the cell face
// numerical flux
void TestEMcNeilVec::predictor() {
  // predictor step of the Runge-Kutta family
  // deprecated, in future version use a separate class
  // that is responsible for time stepping algorithm
  // time step at ...
  const auto dt = dbpp::Simulation::instance()->simulationTimeStep();
  // const int NbSections = m_rhs.m_FF1.size(); // debugging purpose
  const auto dx = m_U12.first->grid().Delta(1);

  // predictor-step (this loop doesn't make sense)
  // so confusing, actually we are looping on grid node
  // and at each grid node there is a section flow
  // associated with it (that what it is)
  for (unsigned j = 1; j < m_rhs.m_FF1.size(); j++) {
    m_U1p[j] = U1[j] - dt / dx * (m_rhs.m_FF1[j] - m_rhs.m_FF1[j - 1]);
    m_U2p[j] = U2[j] - dt / dx * (m_rhs.m_FF2[j] - m_rhs.m_FF2[j - 1]) +
               dt * m_rhs.m_S[j];
  }
}

void TestEMcNeilVec::corrector() {
  // time step at ...
  const auto dt = dbpp::Simulation::instance()->simulationTimeStep();
  // const int NbSections = m_rhs.m_FF1.size(); // debugging purpose
  const auto dx = m_U12.first->grid().Delta(1);

  //	Calcul des valeurs des variables d'état (compute U1, U2)
  for (unsigned j = 1; j < m_rhs.m_FF1.size();
       j++) // computational node except i=0 which is set by B.C.
  {
    U1[j] = 0.5 * (U1[j] + m_U1p[j] -
                   dt / dx * (m_rhs.m_FF1[j] - m_rhs.m_FF1[j - 1]));
    U2[j] = 0.5 * (U2[j] + m_U2p[j] -
                   dt / dx * (m_rhs.m_FF2[j] - m_rhs.m_FF2[j - 1])
                   //- dt/dx * (PF2[j] - PF2[j-1]) not considering pressure for
                   // this simulation
                   + dt * m_rhs.m_S[j]);
  }
}

void TestEMcNeilVec::setBC() {
  //	Calcul des valeurs intermédiaires des variables d'état
  m_U1p[0] = U1[0]; // w_amVal[0]
  m_U2p[0] = U2[0]; // w_amVal[1]

  m_U1p[m_NbSections - 1] = U1[m_NbSections - 1]; // w_avVal[0]
  m_U2p[m_NbSections - 1] = U2[m_NbSections - 1]; // w_avVal[1]
}
} // namespace dbpp
