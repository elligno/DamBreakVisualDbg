
#include <numeric> // UpwindScheme1st algorithm
// package includes
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../Numerics/dbpp_UpwindScheme1st.hpp"
#include "dbpp_EMcNeil1d_f.h"
//#include "TestLogger.h"
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../SfxTypes/dbpp_Simulation.h"
// ...
#include "../Utility/dbpp_Hydro1DLogger.h"
#include "../Utility/dbpp_TestLogger.h"

namespace dbpp {
EMcNeil1d_f::EMcNeil1d_f()
    : EMcNeil1D() // base class (i think its done by default)
{
  auto w_msg1 = "EMcNeil1d_f::EMcNeil1d_f() ctor";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg1));
  auto w_msg2 = "Active RHS Discretization: BaseNumTreatment (EMcNeil)";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg2));

  // create a grid with E. mcNeil discretization
  auto w_grid = // E McNeil discretization as default
      std::make_shared<dbpp::gridLattice>(std::string("d=1 [0,1000] [1:100]"));

  // Create scalar field for the A-variable
  m_U12.first.reset(new dbpp::scalarField(w_grid, std::string("A")));
  // Create scalar field for the Q-variable
  m_U12.second.reset(new dbpp::scalarField(w_grid, std::string("Q")));

  // Create a scalar field for mid-time step
  m_U12p.first.reset(new dbpp::scalarField(w_grid, std::string("A1p")));
  m_U12p.second.reset(new dbpp::scalarField(w_grid, std::string("A2p")));
}

void EMcNeil1d_f::timeStep() {
  using namespace std;
  using namespace std::placeholders;

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"EMcNeil1d_f::timeStep() algorithm"}.data());

  // set boundary value at both end (downstream and upstream)
  setBC();

  // set boundary value at both end (downstream and upstream)
  assert(100 == m_listSections->getList().size());
  assert(100 == m_U12.first->values().size());

  m_listSections->getList()[0]->setH(m_amontBC[2]);
  m_listSections->getList()[m_U12.first->values().size()]->setH(
      m_avalBC[2]); // ghost node

  //
  // Use a two steps algorithm (more stability when )
  //

  // compute intermediate state (n+1/2)
  predictor();

  // compute final state (n+1)
  corrector();

  // notify all observers (for now list of sections and Global Discretization)
  setState();

  // lines above replaced by this line
  auto NbIterations = dbpp::Simulation::instance()->getIterationNumber();
  dbpp::DbgLogger::instance()->write2file(std::string(""), ++NbIterations);

  // Write to log file for visualizing data
#if 0 // deprecated (older version)
		std::vector<double> w_U1; w_U1.reserve(m_U12.first->values().size());
		std::vector<double> w_U2; w_U2.reserve(m_U12.first->values().size());
    m_U12.first->values().to_stdVector(w_U1);
		m_U12.first->values().to_stdVector(w_U2);
#endif

  auto w_U1 = m_U12.first->values().to_stdVector();
  auto w_U2 = m_U12.first->values().to_stdVector();

  dbpp::DbgLogger::instance()->write2file(
      std::make_tuple(m_U12.first->values().size(), w_U1, w_U2));
}

void EMcNeil1d_f::predictor(const fluxVector &aFluxVec) {
  // not sure about this line
  const auto w_dt = dbpp::Simulation::instance()->simulationTimeStep();

  // retrieve grid of the domain
  const auto &w_grid1d = m_U12p.first->grid();

  // compute intermediate time step scalar field values
  auto &w_U1p = m_U12p.first->values();
  auto &w_U2p = m_U12p.second->values();

  // retrieve values from initial or previous time step
  const auto &w_U1 = m_U12.first->values();
  const auto &w_U2 = m_U12.second->values();

  // spatial grid spacing or step (finite difference discretization)
  const auto w_dx = w_grid1d.Delta(1);

  // dF_i = (F_i - F_i-1)/dx first-order derivative with backward upwind stencil
  auto w_dFF1 = UpwindDerivatr1st(aFluxVec.first);  // first component
  auto w_dFF2 = UpwindDerivatr1st(aFluxVec.second); // second component

  // set BC at upstream (downstream left open) for the computational domain
  w_U1p(1) = m_amontBC[0];
  w_U2p(1) = m_amontBC[1];

  // computational domain (don't consider first node (tied))
  for (int l = 2; l <= w_grid1d.getDivisions(1); ++l) {
    // mass equations
    w_U1p(l) = w_U1(l) - w_dt / w_dx * (w_dFF1[l - 1]);

    // momentum equation (with source term)
    w_U2p(l) = w_U2(l) - w_dt / w_dx * (w_dFF2[l - 1]) + w_dt * m_S[l - 1];
  }
}

void EMcNeil1d_f::corrector(const fluxVector &aFluxVec) {
  // not sure about this line
  const double w_dt = dbpp::Simulation::instance()->simulationTimeStep();

  // retrieve grid of the domain
  const auto &w_grid1d = m_U12p.first->grid();

  // scalar field values over the grid
  const auto &w_U1p = m_U12p.first->values();
  const auto &w_U2p = m_U12p.second->values();

  // does it change values of aU in the advance(...) method??
  auto &w_U1 = m_U12.first->values();
  auto &w_U2 = m_U12.second->values();

  // spatial grid spacing or step (finite difference discretization)
  const double w_dx = w_grid1d.Delta(1);

  // dF_i = (F_i - F_i-1)/dx first-order derivative with backward upwind stencil
  auto w_dFF1 = UpwindDerivatr1st(aFluxVec.first);
  auto w_dFF2 = UpwindDerivatr1st(aFluxVec.second);

  // set BC
  w_U1(1) = m_amontBC[0];
  w_U2(1) = m_amontBC[1];

  // computational node except i=0 which is set by B.C.
  // update field and ready to notify everyone interested
  // by this update, particularly the global discretization
  for (int l = 2; l <= w_grid1d.getDivisions(1); ++l) {
    // mass equation
    w_U1(l) = 0.5 * (w_U1(l) + w_U1p(l) - w_dt / w_dx * (w_dFF1[l - 1]));

    // momentum equation (with source)
    w_U2(l) = 0.5 * (w_U2(l) + w_U2p(l) - w_dt / w_dx * (w_dFF2[l - 1]) +
                     w_dt * m_S[l - 1]);
  }
}

void EMcNeil1d_f::predictor() {
  // same algorithm as the base class but using new approach
  // basic E. McNeil algorithm (Physical Algorithm)
  std::unique_ptr<BaseNumTreatmemt> w_numTermsAlgo(new BaseNumTreatmemt);

  // design note build from initial or previous time step
  // All those vector will be removed in the next refactoring
  // we are on step away to get rid of that old dependency architecture
#if 0 // deprecated (older version)
		vecdbl U1;
		U1.reserve(m_U12.first->values().size());
		vecdbl U2; 
		U2.reserve(m_U12.second->values().size());
		
		// set values 
		m_U12.first->values().to_stdVector(U1);   // computational node
		m_U12.second->values().to_stdVector(U2);
#endif

  auto U1 = m_U12.first->values().to_stdVector(); // computational node;
  auto U2 = m_U12.second->values().to_stdVector();

  // not sure about this one (node indexed from 1 to N)
  // const int nx = m_grid->getMaxI(1); number of points in first dimension
  auto w_lastNodeIdx = m_U12.first->grid().getMaxI(1);
  auto firstIdx = m_U12.first->grid().getBase(1); // return first node index??
  assert(1 == firstIdx); // since std and vector are indexed from 0 to N-1
                         // we need to subtract by 1

  // Uh are global nodal values
  const auto &w_Uval = GlobalDiscretization::instance()->Uh();

  // setting boundary condition
  if (w_Uval[firstIdx - 1].isTiedNode()) // check first node
  {
    setAmont(U1, U2); // set first node
  }

  // REMINDER current version of the physical algorithms are based on global
  // nodes and passed as vector argument. These properties are inherited
  // from old architecture (E. McNeil code). In the next re-factoring these
  // algorithm signature will replaced by more appropriate type such as
  // scalar field.

  // need to check this (actually we check global discretization)
  // global discr include the ghost node,
  if (w_Uval[w_lastNodeIdx - 1].isTiedNode() &&
      w_Uval[w_lastNodeIdx - 1].isGhostNode()) {
    setAval(U1, U2); // adding ghost node
  }

  // flux component at cell face
  fluxVector w_FF12;
  w_FF12.first.reserve(m_U12.first->values().size());
  w_FF12.second.reserve(m_U12.second->values().size());

  // set boundary condition
  // 		U1.push_back(m_avalBC[0]); // downstream (A)
  // 		U2.push_back(m_avalBC[1]); // downstream (Q)

  // Solve by a Riemann solver of type HLL
  // Compute face flux (cell face)
  w_numTermsAlgo->CalculFF(w_FF12.first, w_FF12.second, U1, U2);

  // retrieve vector of H values of the global discretization
  auto w_NbH = GlobalDiscretization::instance()->Hvalues().size();
  vecdbl w_H;
  w_H.reserve(w_NbH);

  //   need to validate ... to be completed
  setH(w_H);

  // Physical algorithm (source term are discretized by a ... to be completed)
  vecdbl w_n(m_listSections->size()); // initialize to 0 as default
  if (!m_S.empty()) {
    m_S.clear(); // size is now zero
  }
  m_S.resize(m_listSections->size() - 1);

  // m_numTermsAlgo->TraitementTermeSource2( S, U2, U1, w_H, n, dx,
  // NbSections/*, B*/);
  const auto dx = m_U12.first->grid().Delta(1);
  w_numTermsAlgo->TraitementTermeSource2(
      m_S, U2, U1, w_H, w_n, dx,
      static_cast<const int>(m_listSections->size()));

  // first step of two-phase algorithm (computational domain: 1,...,99 with 0
  // tied) now we are using the scalarField attribute m_U12p to step through
  // time but we still have algorithm that use vector as argument ()
  predictor(std::make_pair(
      w_FF12.first, w_FF12.second)); // compute m_U12p (computational node
                                     // except i=1 which is set by B.C.)
                                     // Refactoring in the next version
  // predictor(SWERHS); from SweRhsAlgorithm : SWERHS getRHS()
}

void EMcNeil1d_f::corrector() {
  // same algorithm as the base class but using new approach
  // basic E. McNeil algorithm (Physical Algorithm)
  std::unique_ptr<BaseNumTreatmemt> w_numTermsAlgo(new BaseNumTreatmemt);

#if 0 // deprecated (older version)
		vecdbl w_U1p;  // intermediate state variable
		w_U1p.reserve(m_U12p.first->values().size());
		vecdbl w_U2p;  // intermediate state variable
		w_U2p.reserve(m_U12p.second->values().size());

		// retrieve values from predictor update
		m_U12p.first->values().to_stdVector(w_U1p);   // only computational node (100 nodes)
		m_U12p.second->values().to_stdVector(w_U2p);
#endif

  // intermediate state variable (only computational node (100 nodes))
  auto w_U1p = m_U12p.first->values().to_stdVector();
  auto w_U2p = m_U12p.second->values().to_stdVector();

  // no choice to push_back, when assign (to_stdVector) we reset
  // begin and end iterator and we don't have 101 element but the
  // begin-end element (then we have to push the last node)
  // 		w_U1p.push_back(U1[100]);  downstream (A)
  // 		w_U2p.push_back(U2[100]);  downstream (Q)

  // 		w_U1p.push_back(m_avalBC[0]); // downstream (A)
  // 		w_U2p.push_back(m_avalBC[1]); // downstream (Q)

  // call setAmont() and set Aval()
  // NOTE don't call setAmont() since we retrieve intermediate
  // state where amont bc have been set in the predictor step
  setAval(w_U1p, w_U2p);

  // write to debug file
  auto w_NbSections = dbpp::Simulation::instance()->getNbSections();
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(w_NbSections, w_U1p, w_U2p));

  // flux component at cell face
  fluxVector w_FF12;
  w_FF12.first.reserve(m_U12p.first->values().size());
  w_FF12.second.reserve(m_U12p.second->values().size());

  const auto dx = m_U12.first->grid().Delta(1);
  std::vector<double> w_n;
  if (!m_S.empty()) {
    m_S.clear(); // size is now zero
  }
  m_S.resize(m_listSections->size() - 1);

  // retrieve vector of H values of the global discretization
  auto w_NbH = GlobalDiscretization::instance()->Hvalues().size();
  vecdbl w_H;
  w_H.reserve(w_NbH);

  //   need to validate ... to be completed
  setH(w_H);

  // ready for the final phase (Evaluation de F1 et F2)
  w_numTermsAlgo->CalculFF(w_FF12.first, w_FF12.second, w_U1p, w_U2p);
  w_numTermsAlgo->TraitementTermeSource2(m_S, w_U2p, w_U1p, w_H, w_n, dx,
                                         w_NbSections /*, B*/);

  // corrector (no need to compute inside dt and H inside the loop)
  // this version version we use scalarField to update at this time step
  corrector(std::make_pair(w_FF12.first, w_FF12.second));
}

void EMcNeil1d_f::setAmont(std::vector<double> &aU1, std::vector<double> &aU2) {
  // GlobalDiscretization::instance()->gamma().applyBC();

  // setting boundary values
  // retrieve boundary values and set upstream/downstream values
  //     const Gamma& w_bc = GlobalDiscretization::instance()->gamma();
  //     m_amontBC = w_bc.getBCNodeAmont();
  aU1[0] = m_amontBC[0];
  aU2[0] = m_amontBC[1];
}

void EMcNeil1d_f::setAval(std::vector<double> &aU1, std::vector<double> &aU2) {
  // GlobalDiscretization::instance()->gamma().applyBC();

  // retrieve boundary values and set upstream/downstream values
  //     const Gamma& w_bc = GlobalDiscretization::instance()->gamma();
  //     m_avalBC = w_bc.getBCNodeAval();

  // ghost node added for the purpose of the reconstruction procedure
  aU1.push_back(m_avalBC[0]);
  aU2.push_back(m_avalBC[1]);
}

// Design Note temporary fix in the refactoring process
void EMcNeil1d_f::setInitSln(
    const StateVector &aU /*, ListSectFlow *aListofSect*/) {
  // copy semantic? not sure we have a pair
  m_U12 = aU;               // set initial condition
  m_listSections = nullptr; // need to define operator=
}

void EMcNeil1d_f::setH(vecdbl &aVecofH) {
  using namespace std;

  // Design Note deprecated it will be removed in the next refactoring
  // list of section flow (always use the boost cast operators, throw exception)
  // ListSectFlow* w_lsectF = static_cast<ListSectFlow*>(m_listofObs.front());

  // sanity check
  if (nullptr == m_listSections) // with static cast it is not guarantee
  {                              // that it will return nullptr on failure
    throw "Cast to List of SectionFlow didn't succeed\n";
  }

  // Actually we don't calculate it since Z is zero. Could use the functional
  // "minus" with std transform()
  // H=A-Z (water depth) same comment as above, why don't we use Z of base
  // class?

  auto w_gblU1Size = GlobalDiscretization::instance()->U1values().size();

  vector<double> w_Z(
      w_gblU1Size); // initialized to zero as default (assume flat bed)
  std::transform(m_listSections->getList().begin(),
                 m_listSections->getList().end(), std::back_inserter(aVecofH),
                 std::bind(&SectFlow::H, std::placeholders::_1));

  // set boundary value at both end
  // 		w_H[0]=w_amontBC[2];
  // 		w_H[m_U12.first->values().size()]=w_avalBC[2]; // ghost node

  // set boundary value at both end (downstream and upstream)
  // 		m_listSections->getList()[0]->setH(m_amontBC[2]);
  // 		m_listSections->getList()[m_U12.first->values().size()]->setH(m_avalBC[2]);
  // // ghost node
}
} // namespace dbpp
