
// Package includes
#include "dbpp_TestBcSectF.h"
//#include "PhyConstant.h"
#include "../Discretization/dbpp_EmcilNumTreatment.h" // rhs discretization
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
#include "../Utility/dbpp_TestLogger.h"

// BaseNumTreatment library include
#include "../SfxTypes/dbpp_IRhsDiscretization.h" // POM (Physics Object Model)
// Right-hand-side discretization

// NOTE
//  this file need a serious clean-up!!! hardly understandeable

using namespace dbpp;

// Design Note
//  in this version we use a bc implementation based on the section flow
//  these are updated at each iteration and ... to be completed
//  All bc handling will be done by a separate class in the next version
//  it doesn't make sense that numerical method implement bc (Gamma)
//
dbpp::TestBcSectF::TestBcSectF()
    : EMcNeil1D(),    // base ctor
      m_U12(nullptr), // initialize shared pointer
      m_U12p(nullptr) //
{
  // no choice to do that, "OutputSuccess" taking char* instead of const char*
  // should change the signature of this method
  auto *w_msg = "TestBcSectF::TestBcSectF() ctor";
  // logging information for check later
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));

  // just a check, for now we only support 2 types of discretization
  // 	if( dbapp::Simulation::instance()->getDiscrType() ==
  // dbapp::DamBreakData::DiscrTypes::emcneil)
  // 	{
  // 		//m_NbSections = dbpp::Simulation::instance()->getNbSections();
  // 	}
  // 	else
  // 	{
  // 		m_NbSections = 101; // as default value
  // 	}
  //
}

// dbpp::TestBcSectF::~TestBcSectF() {
//  dbpp::Logger::instance()->OutputSuccess("TestBcSectF::TestBcSectF() dtor");
//}

// deprecated, firbidden!!!
// void dbpp::TestBcSectF::setBC()
//{
//	Calcul des valeurs intermédiaires des variables d'état
// 	m_U1p[0] = U1[0]; // w_amVal[0]
// 	m_U2p[0] = U2[0]; // w_amVal[1]
//
// 	m_U1p[m_NbSections-1] = U1[m_NbSections-1];  // w_avVal[0]
// 	m_U2p[m_NbSections-1] = U2[m_NbSections-1];  // w_avVal[1]
//}

// i ma not sure i understand the purpose of  this
std::unique_ptr<BaseNumTreatmemt> TestBcSectF::createBaseNumTreatment() {
  // is it nullptr? i do think so
  std::unique_ptr<BaseNumTreatmemt> m_numTermsAlgo = nullptr;
  if (m_numTermsAlgo == nullptr) // sanity check
  {
    // sanity check
    // reset unique pointer to Base numerical algorithm
    m_numTermsAlgo.reset(new BaseNumTreatmemt);
  }

  // would it work??
  auto dbdata = Simulation::instance()->getActiveDBdata();
  // auto w_domainExt = dbdata.domainExtent();

  // create a grid with E. mcNeil discretization
  // values are based on E. McNeil discretization
  std::shared_ptr<dbpp::gridLattice>
      w_grid = // E McNeil discretization as default
      std::make_shared<dbpp::gridLattice>(std::string("d=1 [0,1000] [1:100]"));

  // initialize scalar field
  m_U12.reset(new dbpp::scalarField(w_grid, std::string("U12")));
  m_U12p.reset(new dbpp::scalarField(w_grid, std::string("U12p")));

  // create state vector (this type is deprecated)
  StateVector w_testCreate = make_pair(m_U12, m_U12p);

  // Physical algorithm (use a Riemann solver to compute flux)
  // new version
  // 	std::map<unsigned,std::vector<double>,std::vector<double>> w_FF =
  // 		CalculFF(StateVector,listsectionsflow);

  // from base class member
  // ListSectFlow *w_listSF = static_cast<ListSectFlow *>(m_listofObs.front());

  // 	m_numTermsAlgo->CalculFF( m_FF1, m_FF2, U1, U2, w_dU1, w_dU2,
  // 		w_listSF->getList().size(), w_Section0->B());
  //
  // 	// compute the source term S_f and S_b (friction and bottom)
  // 	m_numTermsAlgo->TraitementTermeSource2( m_S, U2, U1, w_vH, w_n,
  // 		m_U12.first->grid().Delta(1), w_listSF->getList().size(),
  // w_Section0->B());
  return m_numTermsAlgo;
}

// time stepping and compute the rhs (right-hand-side)
// algorithm that is implemented by the user
void TestBcSectF::timeStep() {
  using namespace std;
  using namespace std::placeholders;

  // working variables (deprecated)
  // 	std::vector<double> w_dU2;
  // 	std::vector<double> w_dU1;

  // number of  sections for this simulation
  //	const auto m_NbSections = dbpp::Simulation::instance()->getNbSections();
  // memory allocation
  // 	if( w_dU1.empty())
  // 	{
  // 		w_dU1.resize(m_NbSections);
  // 		w_dU2.resize(m_NbSections);
  // 	}

  // 	if (m_U1p.empty())
  // 	{
  // 		m_U1p.resize(m_NbSections);
  // 	}
  // 	if (m_U2p.empty())
  // 	{
  // 		m_U2p.resize(m_NbSections);
  // 	}

  // POM (Physics Object Model) factory method to retrieve
  // the right-hand-side discretization that is in use for
  // this simulation (this is default implementation)
  dbpp::IRhsDiscretization *w_pom = ::GetRhsDiscretization();

  // just testing our implementation (POM Physics Object Model)
  //  dbpp::IAccess *w_supportedInterface = dynamic_cast<dbpp::IAccess
  //  *>(w_pom);

  // just a piece of crapt, there is no implementation for the update method
  // just validate a design that could be implemented for POM concept
  //  if (dbpp::IAccess *w_supportedInterface =
  //        dynamic_cast<dbpp::IAccess *>(w_pom)) {
  // Check for supported interface
  //  dbpp::IUpdateable *w_up2Date =
  //    nullptr; // debugging purpose
  // static_cast<dbpp::IUpdateable*>(
  // w_supportedInterface->getInterface(SFX_IDENTITY(IUpdateable)));

  // i do not think (for example could be implemented to to update some global
  // discretization stuff)
  // w_up2Date->update();
  // }

  // Retrieve the H-values by using the Observer
  // First time we step in this method, section flow has been
  // created and initialized with initial data. When leaving advance(...)
  // a call to setState() and and then notify(), section are updated.
  ListSectFlow
      *w_listSF = // downcast (convert base to derived) need an explicit cast
      static_cast<ListSectFlow *>(
          m_listofObs.front()); // IObserver cast to ListSectionFlow
                                //
                                //   // sanity check (use the logger utility)
  //   if( w_listSF==nullptr) // with static cast it is not guarantee
  //   {                      // that it will return nullptr on failure
  //     throw "Cast to List of SectionFlow didn't succeed\n";
  //   }

  // boundary condition shall be set at this point, this way the user
  // can re-implement according to his algorithm. For example we have
  // re-written setAmont and setAval by considering the section flow

  // create the vector of H
  std::vector<double> w_vH;
  w_vH.reserve(w_listSF->getList().size());

  // fill vector wit H value (water level) of each section
  // by going through the list of the section flow
  std::transform(w_listSF->getList().begin(), w_listSF->getList().end(),
                 std::back_inserter(w_vH),
                 std::bind(&SectFlow::H, std::placeholders::_1));

  // debugging purpose
  //   const int NbSections = w_listSF->size();
  //   assert(101 == NbSections); // sanity check

  // retrieve spatial discretization from scalar field
  //   const dbpp::gridLattice& w_grid1D = m_U12.first->grid();
  //   const double dx = w_grid1D.Delta(1);
  //   assert(10.==dx); // sanity check and debugging purpose

  // Spatial discretization data
  // is it E McNeil as default? no Hudson is the default discretization
  //	const DamBreakData w_dbData(Simulation::instance()->getDiscrType());

  // sanity check
  // 	if (w_dbData.getDType() != DamBreakData::DiscrTypes::emcneil)
  // 	{
  // 		std::cerr << "We have a problem with our discretization data\n";
  // 	}

  // retrieve Manning coefficient (uniform section flow)
  // 	const std::vector<double>& w_n = w_dbData.getManning();
  // 	std::vector<double> m_FF1, m_FF2;
  // 	m_FF1.reserve(NbSections-1); m_FF2.reserve(NbSections-1);

  // NOTE: we use the first section of the list
  // if this one is unit width, all are also unit width
  // SectFlow* w_Section0 = w_listSF->getList()[0];

  if (w_pom !=
      nullptr) // we are not considering the pressure term for this simulation
  {            // neither the friction term (frictionless).
    // compute the convective flux (F_c) with a Riemann Solver of type HLL
    //    w_pom->CalculFF(m_FF1, m_FF2, U1, U2, w_dU1, w_dU2, NbSections,
    //    w_Section0->B());
    // source term discretization use a standard finite difference
    // discretization
    //  //   w_pom->TraitementTermeSource2(m_S, U2, U1, w_vH, w_n, dx,
    //  NbSections, w_Section0->B());
    TwoStepsScehmeIntegrator w_rk2(w_pom);
    w_rk2.setInitSln(
        std::vector<double>(m_U1.begin(), m_U1.end()), // r-value move semantic
        std::vector<double>(m_U2.begin(), m_U2.end()));
    // predictor step in Runge-Kutta algorithm
    w_rk2.predictor(w_vH); // declared in initialization method, need to check
                           // to put it as attribute
    w_rk2.getSolutionU12();
    // write result to log file
    dbpp::DbgLogger::instance()->write2file_p(std::make_tuple(
        static_cast<unsigned>(w_rk2.getSolutionU12().first.size()),
        w_rk2.getSolutionU12().first, w_rk2.getSolutionU12().second));

    // need to implement corrector
  }
  //   else // POM not in use, just use a default discretization
  //   {
  // 		// is it nullptr? i do think so
  // 		std::unique_ptr<BaseNumTreatmemt> m_numTermsAlgo;
  // 		if (m_numTermsAlgo == nullptr) // sanity check
  // 		{
  // 			// sanity check
  // 			// reset unique pointer to Base numerical algorithm
  // 			m_numTermsAlgo.reset( new BaseNumTreatmemt);
  // 		}
  // 		// Physical algorithm (use a Riemann solver to compute flux)
  //     m_numTermsAlgo->CalculFF( m_FF1, m_FF2, U1, U2, w_dU1, w_dU2,
  // 			w_listSF->getList().size(), w_Section0->B());
  //
  //     // compute the source term S_f and S_b (friction and bottom)
  //     m_numTermsAlgo->TraitementTermeSource2( m_S, U2, U1, w_vH, w_n,
  // 			m_U12.first->grid().Delta(1),
  // w_listSF->getList().size(), w_Section0->B());
  //   }

  // set boundary node values, in this case i=0 and i=N+1 (ghost node)
  // U1p=..., U2p=... intermediate state
  // setBC();

  // first step of two-phase algorithm (computational domain: 1,...,99 with 0
  // tied)
  // predictor();  compute U1p, U2p (computational node except i=0 which is set
  // by B.C.)

  // write to debug file
  // dbpp::DbgLogger::instance()->write2file_p(
  // std::make_tuple(m_NbSections,m_U1p,m_U2p));

  //   if (w_pom!=nullptr)
  //   {
  //     w_pom->CalculFF(m_FF1, m_FF2, m_U1p, m_U2p, w_dU1, w_dU2, NbSections,
  //     w_Section0->B()); w_pom->TraitementTermeSource2(m_S, m_U2p, m_U1p,
  //     w_vH, w_n, dx, NbSections, w_Section0->B());
  //  }
  //   else
  //   {
  // 		// is it nullptr? i do think so
  // 		std::unique_ptr<BaseNumTreatmemt> m_numTermsAlgo;
  // 		if (m_numTermsAlgo==nullptr) // sanity check
  // 		{
  // 			// sanity check
  // 			// reset unique pointer to Base numerical algorithm
  // 			m_numTermsAlgo.reset( new BaseNumTreatmemt);
  // 		}
  //
  //     // ready for the final phase (Evaluation de F1 et F2)
  //     m_numTermsAlgo->CalculFF( m_FF1, m_FF2, m_U1p, m_U2p, w_dU1, w_dU2,
  //     NbSections, w_Section0->B()); m_numTermsAlgo->TraitementTermeSource2(
  //     m_S, m_U2p, m_U1p, w_vH, n, dx, NbSections, w_Section0->B());
  //   }

  // corrector (no need to compute inside dt and H inside the loop)
  // again it's hard to differentiate (wrap it in a method call 'corrector').
  // corrector();

  // for now we leave it like that, but i ma not sure if we should do it that
  // way
  // updateH();  deprecated

  // what about setState(not sure)?
  setState();

  // clean-up
  //   if( w_pom!=nullptr)
  //   {
  ::RhsRelease(w_pom);
  //   }
}

void dbpp::TestBcSectF::setInitSln(const StateVector &aU,
                                   ListSectFlow *aListofSect) {
  // this step was done in the base class in the first version of the prototype
  // set the  initial solution (U1,U2)
  const dbpp::RealNumArray<real> &w_A = aU.first->values();  // A-values
  const dbpp::RealNumArray<real> &w_Q = aU.second->values(); // Q-values

  // copy in our data  structure (range construct)
  m_U1.assign(w_A.getPtr(), w_A.getPtr() + w_A.size());
  m_U2.assign(w_Q.getPtr(), w_Q.getPtr() + w_Q.size());

  // section flow to initialize our water level
  // create the vector of H
  std::vector<double> m_vH;
  m_vH.reserve(aListofSect->getList().size());

  // fill vector wit H value (water level) of each section
  // by going through the list of the section flow
  std::transform(aListofSect->getList().begin(), aListofSect->getList().end(),
                 std::back_inserter(m_vH),
                 std::bind(&SectFlow::H, std::placeholders::_1));
}

// void TestBcSectF::updateH()
// {
//   // base class implementation (update node 1,...99) not boundary 0,100
//   EMcNeil1D::updateH();
//
//   // since we implement the bc with section flow (water level is set)
//   // it means we retrieve water level from section flow not from H variable
//   // but not the H variable of the base class, we need to set i=0,100
//   // retrieve section flow
//   ListSectFlow* w_sectionAmont =
//   static_cast<ListSectFlow*>(m_listofObs.front()); SectFlow* w_beg =
//   *w_sectionAmont->getList().begin(); // first section i=0 const double wH =
//   w_beg->H(); assert(10.== wH); H[0] = wH; // amont (i=0)
//
//   // now the last section i=100
//   ListSectFlow::vec_iter w_end = w_sectionAmont->getList().end();
//   SectFlow* w_checkSect100 = w_sectionAmont->getList()[100];
//   if (w_checkSect100)
//   {
//     // we are at valid location
//     const double w_H = w_checkSect100->H();
//     assert(1.==w_H);
//     // do i have to do a push back?
//     H[100] = w_H;
//   }
// }

// that is responsible for time stepping algorithm
// 	void TestBcSectF::predictor()
// 	{
// time step at ...
// 		const double dt = Simulation::instance()->simulationTimeStep();
//
//     // retrieve spatial discretization
//     const dbpp::gridLattice& w_gridInUde =  m_U12.first->grid();
//     const double dx = w_gridInUde.Delta(1);
//
// 		// predictor-step (subtract 1 to because we are using stl vector
// 		// which are indexed from 0 to 1, 0 is set by boundary condition
// 		// and the last node index of the domain is N-1 of the grid
// 		// 0-99 domain of the stl
// 		// 1-100 domain of the grid
// 		for (int j = 1; j < w_gridInUde.getDivisions(1); j++)
// 		{
// 			m_U1p[j] = U1[j] - dt/dx*(m_FF1[j] - m_FF1[j-1]);
// 			m_U2p[j] = U2[j] - dt/dx*(m_FF2[j] - m_FF2[j-1])
// 		//		- dt/dx * (PF2[j] - PF2[j-1])
// 				+ dt * m_S[j];
// 		}
//	}

// that is responsible for time stepping algorithm
// these U1,2 correspond to array from base class
//    void TestBcSectF::corrector()
//    {
//   	// time step at ...
//   	const double dt = Simulation::instance()->simulationTimeStep();
//     // retrieve spatial discretization
//     const dbpp::gridLattice& w_gridInUde =  m_U12.first->grid();
//     const double dx = w_gridInUde.Delta(1);
//
//   	//	Calcul des valeurs des variables d'état (compute U1, U2)
//   	for( int j = 1; j < w_gridInUde.getDivisions(1); j++) // computational
//   node except i=0 which is set by B.C.
//   	{
//   		U1[j] = 0.5 * (U1[j] + m_U1p[j] - dt/dx * (m_FF1[j] -
//   m_FF1[j-1])); 		U2[j] = 0.5 * (U2[j] + m_U2p[j] - dt/dx *
//   (m_FF2[j]
//   - m_FF2[j-1])
//    				//- dt/dx * (PF2[j] - PF2[j-1]) not considering
//    pressure for this simulation
//    				+ dt * m_S[j]);
//   	}
//   }

void dbpp::TestBcSectF::TwoStepsScehmeIntegrator::predictor(
    const std::vector<double> &avH) {
  std::vector<double> w_S;
  // design note: list of section flow shall be managed by the simulation beans
  // since it reads the section from file. Then return pass to Observer to be
  // updated as simulation is running. return when needed.
  // const auto& myList = m_listSectFlow->getList();
  // debug purpose (error about using m_listofObs in this context??? i do not
  // understand)
  // ListSectFlow* w_listSF = nullptr; // downcast (convert base to derived)
  // need an explicit cast static_cast<ListSectFlow*>(m_listofObs.front()); //
  // IObserver cast to ListSectionFlow

  // time step at ...
  const auto dt = Simulation::instance()->simulationTimeStep();
  const auto NbSections = Simulation::instance()->getNbSections();
  const DamBreakData &w_discrData =
      Simulation::instance()->getActiveDiscretization();
  const auto dx = w_discrData.dx();
  const auto w_domainExtent = w_discrData.domainExtent();
  const auto w_leftEnd = w_domainExtent.first;
  const auto w_rightEnd = w_domainExtent.second;
  const auto w_nbPts =
      (w_rightEnd - w_leftEnd) /
      dx; // result is an integer? promote to int?? not sure at all!!

  double w_sectWidth = 1.; // unit width as default
  if (!w_discrData.isUnitSectionWidth()) {
    // set it to unit
    w_sectWidth = 1.;
  }

  // retrieve Manning coefficient (uniform section flow)
  std::vector<double> w_n(NbSections); // set to zero
  std::vector<double> w_FF1, w_FF2;
  w_FF1.reserve(NbSections - 1);
  w_FF2.reserve(NbSections - 1);

  // Set boundary cnd, don't forget for both nodes
  // design note:
  const Gamma &w_bc = GlobalDiscretization::instance()->gamma();
  const Nodal_Value &w_amNode = w_bc.getBCNodeAmont();
  // const Nodal_Value &w_avNode = w_bc.getBCNodeAval();
  m_U1p[0] = w_amNode[0]; // A variable
  m_U2p[0] = w_amNode[1]; // Q variable

  std::vector<double> w_dU1, w_dU2; // shall reserve??
  w_dU1.resize(NbSections);
  w_dU2.resize(NbSections);

  // compute flux a t cell face (what about B.C. taken into account in
  // CalculFF?) B.C. must be apply before passing to calculFF shall reserve on
  // on gradient dU, i think so reconstruction procedure done in calculFF
  m_Rhsdiscr->CalculFF(w_FF1, w_FF2, U1, U2, w_dU1, w_dU2, NbSections - 1,
                       w_sectWidth);

  // compute the source term S_f and S_b (friction and bottom)
  // call reserve on source?
  // water level, use global discretization??
  // const auto& w_vH = GlobalDiscretization::instance()->Hvalues();
  m_Rhsdiscr->TraitementTermeSource2(w_S, U2, U1, avH, w_n, dx, NbSections,
                                     w_sectWidth);

  // predictor-step (subtract 1 to because we are using stl vector
  // which are indexed from 0 to 1, 0 is set by boundary condition
  // and the last node index of the domain is N-1 of the grid
  // 0-99 domain of the stl
  // 1-100 domain of the grid
  for (int j = 1; j < w_nbPts; j++) // check loop-index
  {
    m_U1p[j] = U1[j] - dt / dx * (w_FF1[j] - w_FF1[j - 1]); // mass term
    m_U2p[j] =
        U2[j] -
        dt / dx * (w_FF2[j] - w_FF2[j - 1]) // momentum term
                                            //		- dt/dx * (PF2[j]
                                            //- PF2[j-1]) //pressure term
        + dt * w_S[j];                      // rhs source term
  }
}
