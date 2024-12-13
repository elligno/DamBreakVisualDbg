// C++ includes
#include <cassert> // assert()
#include <chrono>
#include <cmath>
#include <ctime>
#include <limits> // std::numeric_limits
#include <thread>
// STL includes
#include <algorithm> // fill
#include <functional>
#include <vector>
// BaseNumTypes include
//#include "../Utility/dbpp_CommandLineArgs.h"
// src package includes
#include "../Discretization/dbpp_GlobalDiscretization.h"
#include "dbpp_EMcNeil1DFactory.h"
#include "dbpp_ListSectFlow.h"
#include "dbpp_Wave1DSimulator.h"
// Util package includes
#include "../Numerics/dbpp_TimeStepCriteria.hpp"
#include "../SfxTypes/dbpp_PhyConstant.h"
#include "../SfxTypes/dbpp_SharedPtrFactory.hpp"
#include "../SfxTypes/dbpp_Simulation.h"
#include "../Utility/dbpp_AppConstant.hpp" //DIM cte
#include "../Utility/dbpp_CommandLineArgs.h"
#include "../Utility/dbpp_Hydro1DLogger.h"
#include "../Utility/dbpp_SimulationUtility.h"
#include "../Utility/dbpp_TestLogger.h"
// Numerical Schemes include
#include "../NumericalSchemes/dbpp_TestEMcNeilVec.h"
// need to include this because link error about
// operator= unresolved symbol
#include "../SfxTypes/dbpp_RealNumArray.h"

namespace dbpp {

// default ctor (we shall use the ofstream of C++11)
Wave1DSimulator::Wave1DSimulator(/*unsigned int aNbIterationsMax  =50 ,*/
                                 double aCFL /* =0.6 */)
    : m_lambda{nullptr}, m_tip{nullptr}, m_H{nullptr}, m_I{nullptr},
      m_ListSectFlow{nullptr},           // Section flow collection
      m_CFL{aCFL},                       // Courant-Friedrich-Levy
      FichierResultat{nullptr},          // vs2012 we have nullptr
      m_opened{false},                   // result file
      m_activeAlgo{"EMcNeil1D_mod"},     // default value
      m_validationDir{"AlgoValidation"}, // default value
      m_numRep{nullptr},                 // numerical scheme representation
      m_saveResult2File{true},           // default value
      m_NumberIterationsMax{1},          // max number of iteration
      m_Phi1{},                          // E. McNeil default value
      m_Phi0{},                          // E. McNeil default value
      m_shockLoc{},                      // Added by Jean B.
      m_finalTime{22.5},                 // time loop stop criteria
      m_simulatorMode(Wave1DSimulator::eSimulationMode::guiMode) {

  // // still use this in the code???  if not might as well remove it!!!
  // all supported algorithm for our demo (global variable)
  //		std::vector<std::string> w_supportedAlgorithm;
  m_supportedAlgorithm.reserve(5); // memory allocation
  m_supportedAlgorithm.push_back(
      std::string{"EMcNeil1D_f"}); // rvalue reference version
  m_supportedAlgorithm.push_back(std::string{"EMcNeil1D_mod"});
  m_supportedAlgorithm.push_back(std::string{"TestEMcNeilVec"});
  //  m_supportedAlgorithm.push_back(std::string{"TestBcSectF"});
  //  m_supportedAlgorithm.push_back(std::string{"TestNewAlgo"});
}

Wave1DSimulator::~Wave1DSimulator() {
  // save final result to file
  if (m_opened == true) {
    // simulation has terminated, might as well save result
    // delete the file result not needed
    if (FichierResultat) {
      // close it
      errno_t err = fclose(FichierResultat);
      if (err == 0) {
        Logger::instance()->OutputSuccess(
            std::string{"The file 'EMcNeil1D_Result.txt' was closed"}.data());
      } else {
        Logger::instance()->OutputError(
            std::string{"The file 'EMcNeil1D_Result.txt' was not closed"}
                .data());
      }
    }
  } // if m_opened

  FichierResultat = nullptr;

  // delete the list of sections flow
  if (nullptr != m_ListSectFlow) {
    delete m_ListSectFlow;
    m_ListSectFlow = nullptr;
  }
}

// create, read simulation parameters
void Wave1DSimulator::scan() {
  using namespace dbpp;

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Wave1DSimulator::scan initialization started"}.data());

  DamBreakData w_activeData{Simulation::instance()->getActiveDiscretization()};
  std::string w_gridprms{"d=1 [0,"};
  w_gridprms += std::to_string(w_activeData.x_max()) +
                std::string{"] [1:"}; // ui value int x-max coord ] [1:
  w_gridprms += std::to_string(w_activeData.nbSections() - 1) +
                std::string{"]"}; // ui value nbgridpoints
  // one-dimensional; grid x=0,...,1000
  std::shared_ptr<dbpp::gridLattice> w_grid1D;
  w_grid1D.reset(new gridLattice(w_gridprms));

  // DEPRECATED! will be removed in future version
  // attach a field of values on the grid
  //  std::get<0>(m_u).reset(new scalarField(w_grid1D, "A"));
  //  std::get<1>(m_u).reset(new scalarField(w_grid1D, "Q"));

  m_lambda.reset(new scalarField(w_grid1D, "lambda_H"));

  //   the wave-function can be specified on the command line
  //   1: Gaussian function
  //   2: SWE DamBreak initialization
  int func = CmdLineArgs::read("-func", 1); // set to 0 in cmd line args

  // select the appropriate initial functions
  // describing the seabed and surface.
  if (func == 1) {
    m_H.reset(new GaussianBell('H'));
    m_I.reset(new GaussianBell('U'));
    Logger::instance()->OutputSuccess(
        std::string{"We set DamBreak GaussianBell function"}.data());
  } else                   // in the current version of the Simulator
  {                        // func = 0 in cmd line argument of the project
    m_I.reset(new Flat{}); // we consider flat bed bathymetry
    m_H.reset(new Step1D{m_Phi0, m_Phi1, // DamBreak initial wave profile
                         m_shockLoc});   // we set DamBreak step function

    Logger::instance()->OutputSuccess(
        std::string{"DamBreak initial Phi0 and Phi1 set to: %f %f"}.data(),
        m_Phi0, m_Phi1);

    Logger::instance()->OutputSuccess(
        std::string{"We are considering flat bed topography"}.data());
  }

  // initialize the parameters in the functions.
  // Design Note: these parameters could be read from
  // an XML file with boost property tree.
  m_H->scan(); // water level according to dam-break parameters
  m_I->scan(); // initial surface (bed), which is flat bed
  // simulation time parameters
  m_tip.reset(new TimePrm{0, 0., m_finalTime});
  // 0, 0.,                                       initial dt (time step)
  // CmdLineArgs::read("-tstop", m_finalTime)));  fixed time step as default

  Logger::instance()->OutputSuccess(
      std::string{"Final simulation time is: %f"}.data(), m_finalTime);

  Logger::instance()->OutputSuccess(
      std::string{"Wave1DSimulator::scan initialization completed"}.data());
}

// load depth function into lambda's array (that will yield faster
// code since we then avoid virtual function calls every time we need
// to evaluate lambda - now we just look the value up in an array):
void Wave1DSimulator::setH() {
  using namespace dbpp;
#if 0
  // debug check (assume that were set by GUI)
  assert(1. == m_Phi0);
  assert(10. == m_Phi1);
  assert(500. == m_shockLoc);
#endif

  // set reference this way we modify the original values
  auto &w_lambda = m_lambda->values();
  // initial cnd: 10.|1. E. McNeil
  assert(100 == w_lambda.size()); // number of grid points (set from GUI)
  std::fill_n(w_lambda.begin(), (w_lambda.size() / 2) + 1, m_Phi1);
  std::fill_n(std::next(w_lambda.begin(), w_lambda.size() / 2),
              w_lambda.size() / 2, m_Phi0);

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Wave1DSimulator::setH() completed"}.data());
}

StateVector Wave1DSimulator::getIC() {
  DamBreakData w_dbData(
      dbpp::Simulation::instance()->getActiveDiscretization());

  if (w_dbData.getDType() == DamBreakData::DiscrTypes::emcneil) {
    //    dbpp::Logger::instance()->OutputSuccess(
    //        std::string{"Wave1DSimulator::setIC() done successfully"}.data());

    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Active data discretization for this simulation: %s"}
            .data(),
        std::string("EMcNeil").data());
  }

  // auto w_grid = std::make_shared<gridLattice>( new gridLattice{
  // w_dbData.toString(DamBreakData::DiscrTypes::emcneil) } );
  auto w_grid1D = std::make_shared<gridLattice>(
      w_dbData.toString(DamBreakData::DiscrTypes::emcneil));
  std::shared_ptr<scalarField> w_U1{new scalarField{w_grid1D, "A"}};
  std::shared_ptr<scalarField> w_U2{new scalarField{w_grid1D, "Q"}};

  // with values from the appropriate function
  auto &w_U1val = w_U1->values(); // reference to modify it (A)
  auto &w_U2val = w_U2->values(); // reference to modify it (Q)

  // set discharge Q (set to zero follow E. McNeil source code)
  std::fill(w_U2val.getPtr() /*begin range*/,
            w_U2val.getPtr() + w_U2val.size() /*end range*/, 0. /*value*/);

  // sanity check set to zero
  //  if (std::all_of(w_U2val.getPtr() /*begin range*/,
  //                  w_U2val.getPtr() + w_U2val.size() /*end range*/,
  //                  std::bind(std::greater_equal<double>(),
  //                  std::placeholders::_1,
  //                            0.)) == true) {
  //  }

  for (auto i = 1; i <= w_U1->grid().getNoPoints(); i++) {
    // we assume that each section is unit width, this shall be
    // removed in the future version of the simulator
    // this is the same as "Evaluation_A_Enfonction_H"
    // m_I is the bottom topography and we substract water level
    // from Z that gives water depth
    // NOTE we have also m_lambda which i am not sure represent
    // water depth
    w_U1val(i) = m_H->valuePt(w_grid1D->getPoint(1, i), 0.) -
                 m_I->valuePt(w_grid1D->getPoint(1, i), 0.);
  }

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Wave1DSimulator Initial Condition done successfully"}
          .data());

  // debugging check (to be removed)
  assert(w_U1val(1) == 10.);
  assert(w_U1val(49) == 10.);
  assert(w_U1val(50) == 10.);
  assert(w_U1val(2) == 10.);
  assert(w_U1val(51) == 1.);
  assert(w_U1val(3) == 10.);
  assert(w_U1val(52) == 1.);
  assert(w_U1val(100) == 1.);

  // RVO (Return Value Optimization)
  return StateVector{w_U1, w_U2};
}

#if 0
// use the pre-defined function to set initial condition
// TODO: return StateVector
void Wave1DSimulator::setIC() {
  using namespace dbpp;

  // fill the field for the current time period
  // with values from the appropriate function
  auto &w_U1 = m_u.first->values();  // reference to modify it (A)
  auto &w_U2 = m_u.second->values(); // reference to modify it (Q)

  // set discharge Q (set to zero follow E. McNeil source code)
  std::fill(w_U2.getPtr() /*begin range*/,
            w_U2.getPtr() + w_U2.size() /*end range*/, 0. /*value*/);

#if 0
  auto w_x = m_u.first->grid().xMin(1);
  assert(w_x == 0.);

  for (auto i = 1; i <= m_u.first->grid().getMaxI(1); i++) {
    // we assume that each section is unit width, this shall be
    // removed in the future version of the simulator
    //    w_U1(i) = m_H->valuePt(m_u.first->grid().getPoint(1, i), 0.) -
    //             m_I->valuePt(m_u.first->grid().getPoint(1, i), 0.);

    auto checkPt = m_u.first->grid().getPoint(1, i);
    auto valu = m_H->valuePt(w_x, 0.);

    w_U1(i) = m_H->valuePt(w_x, 0.) - m_I->valuePt(w_x, 0.);
    // next grid node
    w_x += m_u.first->grid().Delta(1);
  }
#endif

  // initial cnd: 10.|1. E. McNeil
  std::fill_n(w_U1.begin(), (w_U1.size() / 2) + 1, m_Phi1); // m_Phi0
  //  std::fill_n(w_U1.begin(), std::prev(w_U1.end(), 50), 10.);
  std::fill_n(std::next(w_U1.begin(), w_U1.size() / 2), w_U1.size() / 2,
              m_Phi0); // m_Phi1

  // debugging check
  assert(w_U1(1) == 10.);
  assert(w_U1(49) == 10.);
  assert(w_U1(50) == 10.);
  assert(w_U1(2) == 10.);
  assert(w_U1(51) == 1.);
  assert(w_U1(3) == 10.);
  assert(w_U1(52) == 1.);
  assert(w_U1(100) == 1.);
}
#endif

// Design Note: need method with at max 5 lines of code, that's it!!
// just don't know if we really need it!!
void Wave1DSimulator::solveProblem() {

  // Since C++17 string::data() return a char* and not const char*
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Solving the problem"}.data());
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Starting time loop"}.data());

  // run the algorithm
  timeLoop();

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Ending time loop"}.data());
  dbpp::Logger::instance()->OutputSuccess(std::string{"Problem Solved"}.data());
}

// Design Note:
// ------------
//  Check if time is 0. (initial time), if so then
//  we have to setup initial condition, otherwise
//  we jump directly to 'timeLoop()' and continue the
//  simulation. Check if simulation has finished,
//  then jump at the end of the of the function because
//  problem has been solved.
//  Also there is another mode, call a step mode when debugging,
//  it is very handy because sometimes we just want to go one
//  step at a time and check result of the simulation for some reasons.
void Wave1DSimulator::initialize() {
  // initial condition are set in the scan process
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Initial condition startup phase (scan method)"}.data());

  setH(); // initial depth array
  // setIC();     set initial conditions
  initTime(); // ...
  // create section flow
  if (nullptr != m_ListSectFlow) // from simulation GUI ON
  {
    delete m_ListSectFlow;
    m_ListSectFlow = nullptr;
  }

  createListSections();

  if (nullptr == m_numRep) {
    // create from user selection
    m_numRep = createEMcNeil1DAlgo(); // algorithm name from user selection
    if (m_numRep == nullptr) {
      dbpp::Logger::instance()->OutputError(std::string{
          "Could not create Numerical Scheme, must stop application"}
                                                .data());

      exit(EXIT_FAILURE); // no numerical method no simulation!!!
    }
  }

  // DESIGN NOTE
  // Remove class attributes: m_u, m_up, m_grid don't need that
  // Shall be something like that m_numRep->setInitSln(getIC()); setIC return
  // a StateVector!!
  // ----- Discretize the math equations U_i(n) = U_i(n-1) - lambda*dF -dt*S -
  // dt*P dbpp::Logger::instance()->OutputSuccess("Discretize Equation
  // completed"); BaseNumTreatment w_baseNumTreatment; could be a shared_ptr
  // since we pass it to many instances (shared by them) HLLFaceFluxAlgorithm
  // w_hllFFAlgo{w_baseNumTreatment};
  // w_hllFFAlgo.setReconstrAtCellFace(MUSCL-type);
  // w_hllFFAlgo.setReconstrVariableOrder(2);
  // TraitementTermeSrc2 w_treatmentSrc{w_baseNumTreatment, ListSectionFlow};
  // w_treatmentSrc->useManningFormula();
  // w_treatmentSrc->setManningCoeff(); 0 as default frictionless
  // w_treatmentSrc.setSpatialDerivativeOrder(2);
  // m_numRep->setFluxAlgorithm(HLLFluxAlgo); Godunov-type scheme
  // m_numRep->setTraitementSrc(TraitementTermeSrc2); Source (bottom and
  // friction)
  // dbpp::Logger::instance()->OutputSuccess("Wave Simulator set initial
  // condition");

  // Set solver initial condition
  m_numRep->setInitSln(getIC()); // debugging purpose

#if 0
  if (isSaveResult2File()) {
    // create folders and files to save result of the simulation (debug file)
    createDbgFolderWithFile();

    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Creating folder and file to save simulation"}.data());
  }
#endif

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Initial condition done successfully"}.data());
}

void Wave1DSimulator::doOneStep() {
  using namespace std;
  using namespace dbpp;

  // update all nodes
  m_numRep->advance();

  // NOTE we have A (cross-section surface: wetted area) but we solve for h
  // water depth for each cross-section.
  // Important to know the following:
  // 'H' is the water level measured about some threshold
  // 'h' is the water depth measure from topography (bottom)
  // we are solving swe for (h,hv) mass/momentum, so 'A' represent
  // 'h' (H-Z) for now we have Z=0. flat bed
  // auto &w_hValues = m_u.first->values();  A(h) water depth
  auto &w_hValues = m_numRep->getState().first->values();
  auto &lambdaArray = m_lambda->values();
  lambdaArray = w_hValues; // no need to call copy algorithm

  // now we have the water depth, but we need the water level
  auto w_sectUp2Date = m_ListSectFlow->getList(); // 100 or 101??
  auto begList = w_sectUp2Date.begin();
  //
  while (begList != w_sectUp2Date.end()) // computational domain
  {                                      // but we need also the extra section
    // Important to note
    // section index 0...N-1, numerical array 1...N
    // compute water level
    SectFlow *w_currSect = *begList++;
    w_currSect->setH(HydroUtils::Evaluation_H_fonction_A(
        w_hValues(w_currSect->getId() + 1), w_currSect->B(), w_currSect->Z()));
  }
  // Global nodes values updated
  GlobalDiscretization::instance()->update();
  // Set b.c. (physical boundary condition)
  GlobalDiscretization::instance()->gamma().applyBC();

  // save iteration result to file
  if (isSaveResult2File()) {
    // call m_numRep->getState() and pass list of section flow
    // saveResult(m_numRep->getState(), m_ListSectFlow, m_tip->time());
    saveResult(m_numRep->getState(), m_lambda, m_tip->time());
  }
}

void Wave1DSimulator::initTime() {
  // according to Eric McNeil result, the initial time step
  // shall be equal to t: 0.6058 (make sure ...)
  using namespace dbpp;

  // initial condition for t=0. is set, we are ready for
  // the next time step. So increase time before we go
  // into the main simulation loop. Then since in our
  // algorithm we decide to compute the time step according
  // to some stability criteria, we must set the time
  // changing stepping mode, we would like, for some stability
  // criteria to change to a variable time stepping
  m_tip->setTimeStepMode(dbpp::TimePrm::TimeStepMode::VARIABLE_TIME_STEP);
  m_tip->initTimeLoop(); // initialize simulation time parameters

  // deprecated stuff
  auto w_IC = getIC();
  const auto w_dtmp =
      m_CFL * TimeStepCriteria::timeStep(*w_IC.first, *w_IC.second);
  // const auto w_dt = m_CFL * TimeStepCriteria::timeStep(*m_u.first,
  // *m_u.second);

  // check
  auto chectDiscr = Simulation::instance()->getActiveDiscretization();
  assert(chectDiscr == dbpp::DamBreakData::DiscrTypes::emcneil);

  // create from a vector  (DamBreak Data) temporary fix
  DamBreakData w_dbData(DamBreakData::DiscrTypes::emcneil);
  auto w_grid2Str = w_dbData.toString(DamBreakData::DiscrTypes::emcneil);
  scalarField w_A{std::make_shared<gridLattice>(w_grid2Str),
                  std::vector<double>(w_dbData.getIC().m_U1.begin(),
                                      std::prev(w_dbData.getIC().m_U1.end())),
                  std::string{"A"}}; // initialize scalar field

  // set to zero (Q discharge) according to E. McNeil data
  scalarField w_Q{std::make_shared<gridLattice>(w_grid2Str),
                  std::vector<double>(DIM::value), "A"}; // Computational Domain

  const auto w_dt = m_CFL * TimeStepCriteria::timeStep(w_A, w_Q);

  // sanity check for sake of comparison (both compute with different data)
  assert(w_dtmp == w_dt);

  m_tip->increaseTime(w_dt); // t1=t0+dt;

  // Simulation parameters at start up (dt, Numerical scheme)

  // according to Eric McNeil result, the initial time step
  // shall be equal to t: 0.6058 (make sure ...)
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Initial time step is: %f"}.data(), w_dt);

  // set simulation parameters initialization
  dbpp::Simulation::instance()->setCFL(m_CFL); // hard coded for debugging
  dbpp::Simulation::instance()->setSimulationTimeStep(w_dt);

  // Shall reset incrementation
  if (dbpp::Simulation::instance()->getIterationNumber() != 0) {
    // previous simulation
    dbpp::Simulation::instance()->resetIterationNumber2Zero();
  }
  // ready for the first iteration = 1
  dbpp::Simulation::instance()->incrIteration();
  assert(Simulation::instance()->getIterationNumber() == 1);
}

//++++++++++++++++++++++++++++++++++++++++++++++
// physical algorithm (calculate)
//   -- reconstruction (MUSCL-type)
//   -- numerical flux (Appr. Riemann solver)
//   -- source terms   (Finite Difference)
//  Formula -> lambda*dF_i - dt*S_i (...)
//
// TODO: this function need a serious clean-up.
//
void Wave1DSimulator::timeLoop() // run equivalent
{
  using namespace std;
  using namespace dbpp;

  // Working debug file for this simulation
  // initialize file name and file location
  // and file ready to be open for writing
  dbpp::DbgLogger::instance()->setDbgWorkingFile();

  // for code clarity
  dbpp::Simulation *w_sim = dbpp::Simulation::instance();

  // we are at first iteration and time is time step
  // initial cnd are has been set and we are ready
  // to start the simulation with first iteration.
  // NOTE first time we get in the loop,
  while ( //! m_tip->finished() ||in comment for debugging purpose
      w_sim->getIterationNumber() <=
      w_sim->getNbIterationMax()) // || mb iter != max iter not reached
  {
    dbpp::Logger::instance()->OutputNewline();
    dbpp::Logger::instance()->OutputSuccess(
        std::string{"+++++Simulation parameters for this time step+++++"}
            .data());
    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Time step computed is: %f"}.data(), m_tip->Delta());
    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Simulation time computed is: %f"}.data(), m_tip->time());
    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Iteration number is: %d"}.data(),
        w_sim->getIterationNumber());
    dbpp::Logger::instance()->OutputNewline();

    doOneStep(); // time step (advance numerical algorithm)

    //	plot(false); next version
    // 			if( m_tip->getTimeStepNo()%5 == 0)
    // 			{
    // 				// write a message every 100th step:
    // 				std::cout << "time step " <<
    // m_tip->getTimeStepNo()
    // <<
    // 					": u(x," << m_tip->time() << ")
    // is computed.\n"
    // 					<< std::endl;
    // 			}

    // t+=dt computed by CFL criteria
    auto w_dt = TimeStepCriteria::timeStep(*m_numRep->getState().first,
                                           *m_numRep->getState().second);

    //  auto w_dt = calculateDt(); // just for check
    // simulation time is updated
    dbpp::Simulation::instance()->setSimulationTimeStep(m_CFL * w_dt);
    dbpp::Simulation::instance()->incrIteration(); // ready for next iteration
    m_tip->increaseTime(m_CFL * w_dt);             // next time step t+=dt

    // block execution thread for 3 sec let time to the gui
    // to display info in the editor window
    std::this_thread::sleep_for(std::chrono::seconds(5));
  } // while loop (time loop)
}

std::shared_ptr<dbpp::EMcNeil1D> Wave1DSimulator::createEMcNeil1DAlgo() {
  // shared pointer for numerical representation (base class)
  std::shared_ptr<dbpp::EMcNeil1D> w_num_rep{}; // set to nullptr

#if 1 // dbpp::CmdLineArgs not supported for some reason
      // Check which mode simulator is running (manual/gui)
  if (getSimulatorMode() == eSimulationMode::manualMode) {
    m_activeAlgo = dbpp::CmdLineArgs::read(
        "-algo" /*used defined*/,
        std::string( // default name if none is specified in cmd args
            "EMcNeil1D_mod"));
    for (const std::string &w_algoInUdse : // why?? really need that?
         m_supportedAlgorithm)             // compare with supported algorithm
    {
      if (w_algoInUdse == m_activeAlgo) {
        break; // supported algorithm by our simulator
      } else {
        continue; // check next
      }
    }
  }
#endif

  // Design Note (Deprecated!!!) use the new factory based on perfect
  // forwarding
  //  Don't need to pass DamBreaData to ctor of the factory
  // Create EMcNeil1D type (using abstract factory design pattern)
  // dbpp::DamBreakData w_compileTest;  settings of the solver??? what
  // exactly? w_num_rep = dbpp::EMcNeil1DFactory::CreateSolver(m_activeAlgo);
  if (m_activeAlgo == std::string{"EMcNeil1D_mod"}) {
    // this is deprecated, a new factory method will be use
    w_num_rep = dbpp::EMcNeil1DFactory::CreateSolver(m_activeAlgo);

    if (nullptr == w_num_rep) {
      dbpp::Logger::instance()->OutputError(
          std::string{"Unable to create solver with name: %s"}.data(),
          m_activeAlgo.c_str());
    }
    // return nullptr;  maybe throw an  exception, i think so!!
  } else if (m_activeAlgo == std::string{"TestEMcNeilVec"}) {
    // temporary fix for now (we have a problem!!)
    // in the current version list sections is an observer and takes
    // base class (will be back later to complete implementation)
    // this algorithm will selected by user from GUI
    // SweRhsAlgorithm *w_rhsTest = new TestRhsImpl(m_ListSectFlow);
    // just for testing and debugging
    assert(nullptr != m_ListSectFlow);
    w_num_rep = factoryCreator<TestEMcNeilVec>(new TestRhsImpl(m_ListSectFlow),
                                               TimePrm{0., 0., 0.});

    if (nullptr == w_num_rep) {
      dbpp::Logger::instance()->OutputError(
          std::string{"Unable to create solver with name: %s"}.data(),
          m_activeAlgo.c_str());
    }
  } else {
    // log entry
    dbpp::Logger::instance()->OutputError(
        std::string{"Unrecognized solver name: %s"}.data());
  }

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Created Numerical Scheme successfully: %s"}.data(),
      m_activeAlgo.c_str());

  return w_num_rep;
}

#if 0
// really need it?
// save initial cnd. to file. It's not the final; version of this
// method, ... not completed!!! name missing (not sure about this??)
void Wave1DSimulator::saveIC2File() {
  using namespace dbpp;

  // i am not sure about this line, really need it, i do not think so
  if (m_opened == true)
    return;

  std::string w_fileName =
      std::string("DamBreakIC") + std::string(".") + std::string("out");
  ::strcpy_s(NomFichierResultat, 256,
             w_fileName.c_str() /*"Hydro1DResultat.out"*/);

  // Open for write
  errno_t err = fopen_s(&FichierResultat, NomFichierResultat, "w");
  if (err == 0) {
    printf("The file 'DamBreakIC.out' was opened\n");
    m_opened = true;
  } else {
    printf("The file 'DamBreakIC.out' was not opened\n");
  }

  // save result to file
  saveResult(getIC(), m_lambda, 0.);
}
#endif

// loop index has been changed ...
//  Design Note
//   Use std::filesystem (C++17 feature) and not string to represent a path
void Wave1DSimulator::saveResult(const StateVector &aStateVec,
                                 const fieldptr &aFptr, double aTime) {
  static bool sFirstime = true;

  // first time
  if (m_opened != true && sFirstime == true) {
    std::string w_fileName =
        std::string("EMcNeil1D_Result") + std::string(".") + std::string("txt");
    ::strcpy_s(NomFichierResultat, 256,
               w_fileName.c_str() /*"Hydro1DResultat.out"*/);
    sFirstime = false;

    errno_t err = fopen_s(&FichierResultat, NomFichierResultat, "w");
    if (err == 0) {
      Logger::instance()->OutputSuccess(
          std::string{"The file 'EMcNeil1D_Result.txt' was opened"}.data());
      //  printf("The file 'EMcNeil1D_mod.out' was opened\n");
      m_opened = true;
    }
    // Enregistrement des conditions initiales dans le fichier de résultats
    //       strcpy(NomFichierResultat, "Resultat.out");
    //       FichierResultat = fopen(NomFichierResultat, "w");
  } else {
    errno_t err = fopen_s(&FichierResultat, NomFichierResultat, "a");
    if (err == 0) {
      Logger::instance()->OutputSuccess(
          std::string{"The file 'EMcNeil1D_Result.txt' was opened"}.data());
      m_opened = true;
    } else {
      // printf("The file 'DamBreakIC.out' was not opened\n");
      Logger::instance()->OutputError(
          std::string{"The file 'EMcNeil1D_Result.txt' was not opened"}.data());
    }
  }

  using namespace dbpp;

  const auto &U1 = aStateVec.first->values();  // A
  const auto &U2 = aStateVec.second->values(); // Q
  const auto &H = aFptr->values();             // H

  fprintf(FichierResultat, "t:  %8.4lf\n", aTime);
  fprintf(FichierResultat, "H:  ");

  /*for( j = 0; j < NbSections; j++)*/
  for (auto j = 1; j <= H.size(1); j++)
    fprintf(FichierResultat, "%8.4lf  ", H(j));

  fprintf(FichierResultat, "\n");
  fprintf(FichierResultat, "U1: ");

  // 		for (j = 0; j < NbSections; j++)
  // 			fprintf (FichierResultat, "%8.4lf  ", U1(j));
  for (auto j = 1; j <= U1.size(1); j++)
    fprintf(FichierResultat, "%8.4lf  ", U1(j));

  fprintf(FichierResultat, "\n");
  fprintf(FichierResultat, "U2: ");

  /*for (j = 0; j < NbSections; j++)*/
  for (auto j = 1; j <= U2.size(1); j++)
    fprintf(FichierResultat, "%8.4lf  ", U2(j));

  fprintf(FichierResultat, "\n");
  fprintf(FichierResultat, "V:  ");

  for (auto j = 1; j <= U2.size(1); j++)
    fprintf(FichierResultat, "%8.4lf  ", U2(j) / U1(j));

  // TODO add Froude Number (Fr: sqrt(V^2/gH))
  //  fprintf(FichierResultat, "\n");
  //  fprintf(FichierResultat, "Fr:  ");
  //  for (auto j = 1; j <= U2.size(1); j++)
  //    fprintf(FichierResultat, "%8.4lf  ", (U2(j) / U1(j)) / H(j));

  fprintf(FichierResultat, "\n");

  if (m_opened && FichierResultat) {
    fclose(FichierResultat); // not sure
    m_opened = false;
  }
}

// IMPORTANT
//	Spécification des conditions initiales
//	Dans la version finale, ces conditions initiales "devraient être"
//	spécifiées sous la forme de l'élévation de la surface libre et
//	du débit pour chaque section
void Wave1DSimulator::createListSections() {
  if (nullptr == m_ListSectFlow) {
    // number of grid node(computational domain)
    assert(m_lambda->values().size() == m_lambda->grid().getNoPoints());
    assert(m_lambda->grid().getMaxI(1) == m_lambda->grid().getNoPoints());
    m_ListSectFlow = new dbpp::ListSectFlow(m_lambda->grid().getMaxI(1));

    // Remember that we are indexing from 1 and not 0
    // this means that if we are comparing with E. McNeil code
    // these number correspond to 0...99, but E. McNeil has
    // array of 101 values, here we have array of 100 (domain)
    for (auto i = m_lambda->grid().getBase(1); i <= m_lambda->grid().getMaxI(1);
         i++) // node index
    {
      // auto w_xCurr = m_lambda->grid().getPoint(1, i);
      // set water level to initial dam-break config (index i-1 because
      // numeric array index start at 1)
      m_ListSectFlow->add(new SectFlow(i - 1, m_lambda->grid().getPoint(1, i),
                                       m_lambda->values()(i)));
    }
  }

  // hard coded (debugging purpose) it should be 100
  assert(100 == m_ListSectFlow->size());

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Created list of sections successfully"}.data());
}

void Wave1DSimulator::initializeGuiMode() {
  // just testing some new functionalities of the shared pointer
  // this is a new functionality of the C++11
  std::shared_ptr<Step1D> w_Hstep = std::dynamic_pointer_cast<Step1D>(m_H);
  if (nullptr != w_Hstep) {
    if (w_Hstep->getPhi1() != m_Phi1) {
      // we have a valid cast
      w_Hstep->setPhi1(m_Phi1);
    }
    if (w_Hstep->getPhi0() != m_Phi0) {
      // we have a valid cast
      w_Hstep->setPhi0(m_Phi0);
    }
    if (w_Hstep->getShockLocation() != m_shockLoc) {
      w_Hstep->setShockLocation(m_shockLoc);
    }
  }

  // Initialize simulation before we start
  setH();
  // setIC();
  initTime();
}
} // namespace dbpp
