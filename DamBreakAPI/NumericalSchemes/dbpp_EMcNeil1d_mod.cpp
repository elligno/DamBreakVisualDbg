
// package include
#include "dbpp_EMcNeil1d_mod.h"
#include "../Discretization/dbpp_EmcilNumTreatment.h"
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
#include "../Utility/dbpp_TestLogger.h"
#include "dbpp_HLL_RK2_Algo_Vec.h"

namespace dbpp {
EMcNeil1d_mod::EMcNeil1d_mod() : EMcNeil1D(), m_amontBC{}, m_avalBC{} {
  //	dbpp::Logger::instance()->OutputSuccess("EMcNeil1d_mod default ctor");
  //	dbpp::Logger::instance()->OutputSuccess("Active RHS Discretization:
  // BaseNumTreatment (EMcNeil)");

  // create a grid with E. mcNeil discretization
  std::shared_ptr<dbpp::gridLattice>
      w_grid = // E McNeil discretization as default
      std::make_shared<dbpp::gridLattice>(std::string("d=1 [0,1000] [1:100]"));

  // Create scalar field for the A-variable
  m_U12.first.reset(new dbpp::scalarField(w_grid, std::string("A")));
  // Create scalar field for the Q-variable
  m_U12.second.reset(new dbpp::scalarField(w_grid, std::string("Q")));

  // Create a scalar field for mid-time step
  m_U12p.first.reset(new dbpp::scalarField(w_grid, std::string("A1p")));
  m_U12p.second.reset(new dbpp::scalarField(w_grid, std::string("A2p")));
}

// user specific implementation (abstract in the base class) to this purpose
void EMcNeil1d_mod::timeStep() {
  using namespace std;
  using namespace std::placeholders;

  // require resource
  dbpp::DbgLogger::instance()->open();
  // sanity check
  if (!dbpp::DbgLogger::instance()->isOpen()) {
    dbpp::Logger::instance()->OutputError( // C++17 return char* and not const
        std::string{"Couldn't open file for writing debug info\n"}.data());
  }

  // first step of two-phase algorithm
  predictor();

  auto w_U1p = m_currU.first->values().to_stdVector();
  auto w_U2p = m_currU.second->values().to_stdVector();
  assert(w_U1p.size() == w_U2p.size());

  // write state values for debugging purpose
  const auto w_iterNostr =
      std::to_string(Simulation::instance()->getIterationNumber());
  dbpp::DbgLogger::instance()->write2file(
      std::string{"++++ Iteration number: "} + w_iterNostr);
  dbpp::DbgLogger::instance()->write2file_p(
      std::make_tuple(static_cast<unsigned>(w_U1p.size()), w_U1p, w_U2p));

  // corrector (no need to compute inside dt and H inside the loop)
  // again it's hard to differentiate (wrap it in a method call 'corrector').
  corrector();

  // clear vector before filling with new values
  // write to debug file (m_U1p, m_U2p are both vectors)
  w_U1p.clear(); // size set to zero
                 // m_currU.first->values().to_stdVector(w_U1p);

  w_U1p = m_currU.first->values().to_stdVector();
  w_U2p.clear();

  // m_currU.second->values().to_stdVector(w_U2p);
  w_U2p = m_currU.second->values().to_stdVector();

  assert(w_U1p.size() == w_U2p.size());
  dbpp::DbgLogger::instance()->write2file(
      std::make_tuple(static_cast<unsigned>(w_U1p.size()), w_U1p, w_U2p));

  // shall we call update H, in this implementation we do not use
  // H values from global discretization, we are using the list of
  // section flow, anyway just update it to make sure everything is sync
  // updateH();

  // notify list of section flow and update GlobalDiscretization and
  // BoundaryCondition
  setState();

  // release respource
  if (dbpp::DbgLogger::instance()->isOpen()) {
    dbpp::DbgLogger::instance()->close();
  }
}

void EMcNeil1d_mod::setAmont(vecdbl &aU1, vecdbl &aU2) {

  // retrieve boundary values and set upstream values
  const Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  m_amontBC = w_bc.getBCNodeAmont();
  aU1[0] = m_amontBC[0];
  aU2[0] = m_amontBC[1];
}

void EMcNeil1d_mod::setAval(vecdbl &aU1, vecdbl &aU2) {

  // retrieve boundary values and set downstream values
  const Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  m_avalBC = w_bc.getBCNodeAval();

  // ghost node added for the purpose of the reconstruction procedure
  //		const Nodal_Value w_avalBC = w_bc.getBCNodeAval();
  aU1.push_back(m_avalBC[0]);
  aU2.push_back(m_avalBC[1]);
}

void EMcNeil1d_mod::predictor() {
  // user implementation (could be wrapped in a factory)
  // in the refactoring of the DB++
  RhsStruct w_rhs = createRhsDiscretization(m_U12);

  // computational step algorithm
  predictor(w_rhs);
}

void EMcNeil1d_mod::corrector() {
  // user implementation (could be wrapped in a factory)
  // in the refactoring of the DB++
  RhsStruct w_rhs = createRhsDiscretization(m_U12p);

  // advance on time step
  corrector(w_rhs);
}

// Design Note: do we really need it? just use the version of the base class
void EMcNeil1d_mod::predictor(const RhsStruct &aRhs) {

  // set values (move semantic) no copy
  const vecdbl w_U1{m_U12.first->values().to_stdVector()};  // prvalue
  const vecdbl w_U2{m_U12.second->values().to_stdVector()}; // prvalue

  // again call for bc (no!!no!! we don't use the boundary node downstream!!)
  // and the values of the scalarField bc already apply in advance(), is it?
  // yes, since we are looping from 1, node downstream imply that boundary set

  const auto dx = m_U12p.first->grid().Delta(1);
  const auto w_nbGridPts = m_U12p.first->grid().getDivisions(1);
  const auto dt = dbpp::Simulation::instance()->simulationTimeStep();

  // working variable for numerical algorithm
  // 			std::vector<double> w_U1p; //w_U1p.reserve(
  // m_U12.first->values().size()); 			std::vector<double>
  // w_U2p;
  // //w_U2p.reserve( m_U12.first->values().size());

  // shall be done in the advance() method before calling predictor
  // predictor-step w_U1p[0]?? what we gonna do? might as well to set it
  // to the boundary condition?

  // temporary = move semantic (swap resources) no copy
  vecdbl w_U1p(
      m_U12p.first->values().to_stdVector().size()); // prvalue, call move ctor
  vecdbl w_U2p(
      m_U12p.second->values().to_stdVector().size()); // prvalue, call move ctor

  const Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  const Nodal_Value w_bcAmont = w_bc.getBCNodeAmont();
  w_U1p[0] = w_bcAmont[0];
  w_U2p[0] = w_bcAmont[1];

  // (temporary fix to adapt to legacy code)
  // whenever you can use the range ctor (Scott Meyer's)
  const vecdbl w_FF1(aRhs.m_fVec.first.cbegin(),
                     aRhs.m_fVec.first.cend()); // range ctor
  const vecdbl w_FF2(aRhs.m_fVec.second.cbegin(),
                     aRhs.m_fVec.second.cend());        // range ctor
  const vecdbl w_S(aRhs.m_S.cbegin(), aRhs.m_S.cend()); // range ctor

  // compute st-Venant model (1D)
  for (int j = 1; j < w_nbGridPts;
       j++) // computational domain [1,100] -> [0,99]
  {         // scalarField -> std::vector
    w_U1p[j] = w_U1[j] - dt / dx * (w_FF1[j] - w_FF1[j - 1]); // mass equation
    w_U2p[j] = w_U2[j] - dt / dx * (w_FF2[j] - w_FF2[j - 1]) +
               dt * w_S[j]; // momentum equation
  }

  // copy values to our scalar field structure
  std::copy(w_U1p.data(), w_U1p.data() + w_U1p.size(),
            m_U12p.first->values().getPtr());
  std::copy(w_U2p.data(), w_U2p.data() + w_U2p.size(),
            m_U12p.second->values().getPtr());

  // current solution of the integrator
  m_currU = m_U12p;

  // set bc
  // global discretization nbSections=101
  //     const Nodal_Value w_bcAval = w_bc.getBCNodeAval();
  //     m_U1p[w_nbGridPts] = w_bcAval[0];
  //     m_U2p[w_nbGridPts] = w_bcAval[1];

  // 		// set value to scalarField
  // 		dbpp::RealNumArray<real>& w_realU1 = m_U12p.first->values();
  // 		dbpp::RealNumArray<real>& w_realU2 = m_U12p.second->values();
  // 		for( int i=0;i<w_realU1.size();++i) // set scalarField values
  // 		{
  // 			w_realU1(i+1)=m_U1p[0];
  // 		}
  //
  // 		// writing to file for validation and debugging (shall be to the
  // data store) 		dbgui::DbgLogger::instance()->write2file_p(
  // std::make_tuple( U1p.size(),U1p,U2p));
}

void EMcNeil1d_mod::corrector(const RhsStruct &aRhs) {
  // set values
  const vecdbl w_U1p(
      m_U12p.first->values().to_stdVector()); // prvalue, shall call move ctor
  const vecdbl w_U2p(
      m_U12p.second->values().to_stdVector()); // prvalue, shall call move ctor

  // (temporary fix to adapt to legacy code)
  // whenever you can use the range ctor (Scott Meyer's)
  const vecdbl w_FF1(aRhs.m_fVec.first.cbegin(),
                     aRhs.m_fVec.first.cend()); // range ctor
  const vecdbl w_FF2(aRhs.m_fVec.second.cbegin(),
                     aRhs.m_fVec.second.cend()); // range ctor
  const vecdbl w_S(aRhs.m_S.cbegin(), aRhs.m_S.cend());

  // set values
  vecdbl w_U1(
      m_U12.first->values().to_stdVector()); // prvalue, shall call move ctor
  vecdbl w_U2(
      m_U12.second->values().to_stdVector()); // prvalue, shall call move ctor

  const Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  const Nodal_Value w_bcAmont = w_bc.getBCNodeAmont();
  w_U1[0] = w_bcAmont[0];
  w_U2[0] = w_bcAmont[1];

  const auto dx = m_U12.first->grid().Delta(1);
  const auto w_nbGridPts = m_U12.first->grid().getDivisions(1);
  const auto dt = dbpp::Simulation::instance()->simulationTimeStep();

  // Note we are using std vector which are indexed from 0,...,N-1
  // Calcul des valeurs des variables d'état (compute U1, U2)
  for (int j = 1; j < w_nbGridPts;
       j++) // computational node except i=0 which is set by B.C.
  {
    w_U1[j] = 0.5 * (w_U1[j] + w_U1p[j] - dt / dx * (w_FF1[j] - w_FF1[j - 1]));
    w_U2[j] = 0.5 * (w_U2[j] + w_U2p[j] - dt / dx * (w_FF2[j] - w_FF2[j - 1]) +
                     dt * w_S[j]);
  }

  // copy values to our scalar field structure
  std::copy(w_U1.data(), w_U1.data() + w_U1.size(),
            m_U12.first->values().getPtr());
  std::copy(w_U2.data(), w_U2.data() + w_U2.size(),
            m_U12.second->values().getPtr());

  // is that make sense?
  //     m_U12.first->values().to_stdVector() = std::move(w_U1);
  //     m_U12.second->values().to_stdVector() = std::move(w_U2);

  // current solution of the integrator
  m_currU = m_U12;
}

void EMcNeil1d_mod::setH(vecdbl &aH) {
  // shall we throw an exception?
  if (nullptr == m_listSections) {
    return;
  }
  // create the vector of H
  //	std::vector<double> w_H;
  //		aH.reserve(m_listSections->getList().size());
  // Design Note
  //  Shall we set the
  std::transform(m_listSections->getList().begin(),
                 m_listSections->getList().end(), std::back_inserter(aH),
                 std::bind(&SectFlow::H, std::placeholders::_1));
}

// not clear yet what we do here
// Design note pass a type DamBreakIC (encapsulate initila cond.)
void EMcNeil1d_mod::setInitSln(const StateVector &aU)
// ListSectFlow *aListofSect)
{
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"EMcNeil1d_mod initial solution"}.data());

  // debugging stuff
  //  auto w_ptrCountBef = aU.first.use_count();
  m_U12 = aU; // share resource!!!
  // auto w_ptrCountAft = aU.first.use_count();
  //  assert(w_ptrCountBef== w_ptrCountAft);
  // auto checkAttr = m_U12.first.use_count();

  // ctor
  // gridLattice(int nx, int ny, double xmin_, double xmax_, double ymin_,
  //            double ymax_);

  // sanity check (debug purpose)
  assert(100 == aU.first->grid().getNoPoints());
  assert(0. == aU.first->grid().xMin(1));
  assert(1000. == aU.first->grid().xMax(1));
  auto testGridCtor = std::make_shared<dbpp::gridLattice>(
      aU.first->grid().getNoPoints(), 0, aU.first->grid().xMin(1),
      aU.first->grid().xMax(1), 0., 0.);

  // don't need to call ctor with string initialization (hard coded and error
  // prone, dependency on a given data, hard to debug!!!)
  // m_U12p = aU; or i should i do the following
  // create a grid with E. mcNeil discretization
  auto w_grid = // E McNeil discretization as default
      std::make_shared<dbpp::gridLattice>(std::string("d=1 [0,1000] [1:100]"));

  // Create a scalar field for mid-time step
  m_U12p.first.reset(new dbpp::scalarField{w_grid, std::string("A1p")});
  m_U12p.second.reset(new dbpp::scalarField{w_grid, std::string("A2p")});

  // will be removed in the next version
  // m_listSections = aListofSect;
  //  m_listSections = nullptr;
  // throw std::exception("Could create initial solution");
}

// Could be wrapped in a kind of factory method
EMcNeil1d_mod::RhsStruct
EMcNeil1d_mod::createRhsDiscretization(const StateVector &aU) {
  // based on E. McNeil algorithm (Physical Algorithm)
  // deprecated signature will be change
  BaseNumPtr w_numTermsAlgo(new BaseNumTreatmemt);

  RhsStruct w_rhs{}; // aggregate ...
  w_rhs.m_fVec.first.resize(
      GlobalDiscretization::instance()->faces().size()); // faces j+1/2
  w_rhs.m_fVec.second.resize(
      GlobalDiscretization::instance()->faces().size()); // faces j+1/2
  assert(100 == w_rhs.m_fVec.first.size());
  assert(100 == w_rhs.m_fVec.second.size());

#if 0 // deprecated (older version)
		vecdbl U1; U1.reserve( aU.first->values().size());
		aU.first->values().to_stdVector(U1);

		vecdbl U2; U2.reserve( aU.second->values().size());
		aU.second->values().to_stdVector(U2);
#endif

  vecdbl U1 = aU.first->values().to_stdVector();
  vecdbl U2 = aU.second->values().to_stdVector();

  // gradient dU (evaluated  inside of the flux algorithm)
  vecdbl w_dU1(GlobalDiscretization::instance()
                   ->U1values()
                   .size()); // ghost node (calculFF() reconstruction)
  vecdbl w_dU2(GlobalDiscretization::instance()
                   ->U2values()
                   .size()); // ghost node (calculFF() reconstruction)

  // apply boundary condition at both end
  setAmont(U1, U2); // now we have 101 pts
  setAval(U1, U2);

  // Solve by a Riemann solver of type HLL (compute flux at cell face)
  // this signature is deprecated (next refactoring it will be removed)
  // dU are gradient evaluation used a slope limiter algorithm
  w_numTermsAlgo->CalculFF(w_rhs.m_fVec.first, w_rhs.m_fVec.second, U1, U2,
                           w_dU1, w_dU2, static_cast<const int>(U1.size()));

  // Physical algorithm (source term are discretized by a ... to be completed)
  vecdbl n(U1.size()); // initialize to 0 as default
  using GblDiscr = GlobalDiscretization;
  vecdbl w_H;
  w_H.reserve(GblDiscr::instance()->Hvalues().size());
  GblDiscr::instance()->to_stdVector(w_H, GblDiscr::NodalValComp::H);
  // setH(w_H);//?? what for?? use listofsections, don't really need it

  // source term (at each grid node of the computational domain)
  w_rhs.m_S.resize(m_U12.first->grid().getDivisions(1));

  // next version of this method which make more sense
  /*m_numTermsAlgo->TraitementTermeSource2(S, U2, U1,dx, w_listSF);*/
  const auto dx = m_U12.first->grid().Delta(1); // only first dimension
  w_numTermsAlgo->TraitementTermeSource2(
      w_rhs.m_S, U2, U1, w_H, n, dx,
      static_cast<const int>(U1.size())); // as default unit width

  return w_rhs;
}

} // namespace dbpp
