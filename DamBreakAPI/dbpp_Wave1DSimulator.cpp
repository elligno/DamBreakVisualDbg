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

#if 0
	// design Note: shall return a boolean to specify if
	// creation was done successfully. If not may need to 
	// notify user that result won't saved because of some problem.
	// this is our first version of this functionality of creating 
	// folders and files with the boost file system.
	void Wave1DSimulator::createFolderAndFile()
	{
		// create a directory 
		// set a namespace for clarity
		namespace bfs = boost::filesystem;

    // what we do here? create folder? no create the path
    auto w_currrentWrkDirectory = setDbgWorkingDir();
    w_currrentWrkDirectory.normalize(); // Windows standardize path "\\"

    //bfs::path DirContainingFile11;  store founded file 

		// just a test name (file already exist in directory) 
		//const std::string w_activeAlgo("JeanB");

		// corresponding file name to simulation result
		auto w_fileName = CreateAlgoWrkFile();

		// directory format is the following "myDir_2016-04-27"
		//bfs::path w_destination(w_curDirString);
		if( bfs::exists(w_currrentWrkDirectory))  // check if destination exist?
		{                                               // date is same or different
			//std::cerr << "Destination directory "   
			//	<< w_destination.string()  
			//	<< " already exists." << '\n';
//       auto w_msg = std::string{ "Destination directory: " }+w_destination.string() + std::string{ " already exists." };
//       dbpp::Logger::instance()->OutputSuccess(const_cast<char*>(w_msg.c_str()));
      dbpp::Logger::instance()->OutputSuccess("Destination directory: %s %s", w_currrentWrkDirectory.string().c_str(), std::string{ " already exists." }.c_str());

      if( bfs::exists(w_currrentWrkDirectory) && bfs::is_directory(w_currrentWrkDirectory))
      {
        const bfs::directory_iterator end;
        const bfs::directory_iterator dir_iter(w_currrentWrkDirectory/*directory*/);
        auto folder_dist = std::distance(dir_iter, end);
        if( folder_dist==0) // empty, no files
        {
          // corresponding file name to simulation result
          auto w_fileName = CreateAlgoWrkFile();
        }
        else // not empty, contains 
        {
          bfs::directory_entry& w_file = *dir_iter; // last file in directory
          auto strName = w_file.path().filename().string(); // file name
          std::vector<std::string> w_nameExt;
          w_nameExt.reserve(2);
          boost::split(w_nameExt, strName, boost::is_any_of(".")); // without extension
          auto w_lastDigit = w_nameExt[0].back(); // last digit of file name
          auto w_toInt = std::atoi(&w_lastDigit); // char to integer
          w_toInt += 1;
          auto& w_updatdInt = w_nameExt[0].back();
          w_updatdInt = *const_cast<char*>(std::to_string(w_toInt).c_str());
          auto w_newName = w_nameExt[0]; // update version number
          w_newName += std::string{ "." }+w_nameExt[1];
        }
      }

#if 0
      // find file by recursive procedure
			if( FindFile11( w_destination, DirContainingFile11, w_fileName))
			{
				// actually it's not what we want, format of our file is with underscore
				// with a number, so we want to retrieve that number and increment it
				size_t w_foundIt = w_fileName.find_last_of("_");
				if (w_foundIt != std::string::npos)
				{
					// found one "_"
					std::string::iterator w_beg = w_fileName.begin(); 
					std::advance(w_beg,w_foundIt+1);  // version number location
					const char w_char2look = *w_beg;  // what we are looking for
					int versionNo = atoi(&w_char2look); // convert to an integer
					do // search for last version saved
					{
						// replace version number by upgraded one
						++versionNo; // next version
						w_fileName.replace( w_foundIt+1,1,std::to_string(versionNo));
					} while( FindFile11( w_destination, DirContainingFile11, w_fileName));
				}

         // testing ... ???
				std::string w_save_file_name = w_curDirString+ "\\" +w_fileName;

				// initialize the sole instance of the logger (global instance)
				dbpp::DbgLogger* w_dbgLog = dbpp::DbgLogger::instance();
				w_dbgLog->open( w_save_file_name);

				// sanity check
				if( !w_dbgLog->isOpen())
				{
					dbpp::Logger::instance()->OutputError("Problem to open a debug log file\n");
				}
			}
			else // need to create a new file 
			{
				const std::string w_save_file_name = CreateAlgoWrkFile(w_curDirString);

				// initialize the sole instance of the logger (global instance)
				dbpp::DbgLogger* w_dbgLog = dbpp::DbgLogger::instance();
				w_dbgLog->open( w_save_file_name);

				// sanity check
				if( !w_dbgLog->isOpen())
				{
					dbpp::Logger::instance()->OutputError("Problem to open a debug log file\n");
				}
			} // find file
#endif
		} // directory exist
		else // need to create a new directory
		{    // then save file in this directory
// 			std::string w_newDate = getDateFormat();
// 			bfs::path w_destDir(w_currrentWrkDirectory);

			// Create the destination directory  
			if( !bfs::create_directory(w_currrentWrkDirectory))
			{  
// 				std::cerr << "Unable to create destination directory"  
// 					<< w_destDir.string() << '\n';  
        //auto w_msg = std::string{ "Unable to create destination directory " }+w_destDir.string();
        dbpp::Logger::instance()->OutputSuccess("Unable to create destination directory: %s", w_currrentWrkDirectory.string().c_str());
        //dbpp::Logger::instance()->OutputError(const_cast<char*>(w_msg.c_str()));
				return;  
			}
			else // new directory created
			{
#if 0
				std::string w_save_file_name = CreateAlgoWrkFile(w_curDirString);

				// initialize the sole instance of the logger (global instance)
				dbpp::DbgLogger* w_dbgLog = dbpp::DbgLogger::instance();
				w_dbgLog->open( w_save_file_name);

				// sanity check
				if( !w_dbgLog->isOpen())
				{
					dbpp::Logger::instance()->OutputError("Problem to open a debug log file\n");
				}
#endif
			}
		}

		//std::cout << "Exiting of the boost test dir creation and find file\n";
    dbpp::Logger::instance()->OutputSuccess("Exiting createFolderAndFile() dir creation and find file");
	}
#endif

// default ctor (we shall use the ofstream of C++11)
Wave1DSimulator::Wave1DSimulator(/*unsigned int aNbIterationsMax  =50 ,*/
                                 double aCFL /* =0.6 */)
    : m_lambda{nullptr}, m_tip{nullptr}, m_H{nullptr}, m_I{nullptr},
      m_ListSectFlow{nullptr},           // Section flow collection
      Sf1{}, S0am{}, S0av{},             // ...
      m_CFL{aCFL},                       // Courant-Friedrich-Levy
      m_xmin{0.},                        // E. McNeil default value
      m_xmax{1000.},                     // E. McNeil default value
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
  m_supportedAlgorithm.push_back(std::string{"TestBcSectF"});
  m_supportedAlgorithm.push_back(std::string{"TestNewAlgo"});
}

Wave1DSimulator::~Wave1DSimulator() {
  // save final result to file
  if (m_opened == true) {
    // simulation has terminated, might as well save result
    // to file for visualizing and plot
    //	saveResult(m_u,m_lambda,m_tip->time());  debugging purpose ()

    // delete the file result not needed
    if (FichierResultat) {
      // close it
      errno_t err = fclose(FichierResultat);
      if (err == 0) {
        // printf( "The file 'EMcNeil1D_Result.txt' was closed\n" );
        auto *w_msg = "The file 'EMcNeil1D_Result.txt' was closed";
        Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));
        // FichierResultat=nullptr;
      } else {
        // printf( "The file 'EMcNeil1D_Result.txt' was not closed\n" );
        auto *w_msg = "The file 'EMcNeil1D_Result.txt' was not closed";
        Logger::instance()->OutputError(const_cast<char *>(w_msg));
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
  m_grid.reset(new gridLattice(w_gridprms));

  // DEPRECATED! will be removed in future version
  // attach a field of values on the grid
  std::get<0>(m_u).reset(new scalarField(m_grid, "A"));
  std::get<1>(m_u).reset(new scalarField(m_grid, "Q"));

  m_lambda.reset(new scalarField(m_grid, "lambda_H"));

  // NOTE need to initialize the command line args in the main
  // to make it work, without this it will crash (argv and argc empty)

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
        std::string{"We are considering flat bed topography"}.data());

    Logger::instance()->OutputSuccess(
        std::string{"We set DamBreak step function"}.data());
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

  // debug check (assume that were set by GUI)
  assert(10. == m_Phi0);
  assert(1. == m_Phi1);
  assert(500. == m_shockLoc);

  // set reference this way we modify the original values
  auto &w_lambda = m_lambda->values();
  // initial cnd: 10.|1. E. McNeil
  assert(100 == w_lambda.size());
  std::fill_n(w_lambda.begin(), (w_lambda.size() / 2) + 1, m_Phi0);
  std::fill_n(std::next(w_lambda.begin(), w_lambda.size() / 2),
              w_lambda.size() / 2, m_Phi1);

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Wave1DSimulator::setH() completed"}.data());
}

// NOTE: ???  i do not remember
// Design Note: it shouldn't be handled by the simulator (DEPRECATED!!!!)
real Wave1DSimulator::calculateDt() {
  using namespace std;
  using namespace dbpp;

  // real maxValue = 0;
  const auto &w_U1 = m_u.first->values();  // (A)
  const auto &w_U2 = m_u.second->values(); // (Q)
  // const int w_nx = m_grid->getBase(1); // index of the first grid node
  const int w_nbPts = m_grid->getNoPoints();
  const auto dx = m_grid->Delta(1); // in x-direction (shall be 10)

  // dt = BIG_dt;  huge double value (debugging)
  const double w_grav = dbpp::PhysicalConstant::sGravity;

  // NOTE (important) for some reason the numeric_limits compile time error
  // it seems it doesn't recognize the max() method. I think there is a
  // conflict. no choice we make use of big_dt When i include the EMcNeil1D.h
  // header file, that create he problem lot header are included that comes
  // from c language, it may the reason that create a compile time error.
  // Finally i found it, the header file include the windows.h which windef.h
  // that contains minmax macro. Resolve it by surrounding the function with
  // parenthese.
  // real dt = BIG_dt;

  auto dt = (std::numeric_limits<real>::max)();
  for (auto j = 2; j <= w_nbPts; j++) // start j=2, i think j=1 is tied node
  {
    auto V = w_U2(j) / w_U1(j);        // ...
    auto c = ::sqrt(w_grav * w_U1(j)); // unit width
    auto dtc = dx / (::fabs(V) + c);   // ...
    dt = std::min<double>(dtc, dt);    // ...
  }

  // debugging purpose don't really need to do that, actually this is not
  // as important as the simulation time, since we are comparing with E.
  // McNeil source code (see file 'debug de la fonction de controle') cout <<
  // "\nAutomatic variable adjustment of time step to " << dt << endl;

  // Calcul de dt (shall be done outside the method, this method
  // compute time step, multiplication by cfl number not here, when
  // call the increase time step in the main simulation time loop)
  return m_CFL * dt;
}

StateVector Wave1DSimulator::getIC() {
  DamBreakData w_dbData(
      dbpp::Simulation::instance()->getActiveDiscretization());
  if (w_dbData.getDType() == DamBreakData::DiscrTypes::emcneil) {
    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Wave1DSimulator::setIC() done successfully"}.data());

    std::cout << "Active discretization used by GlobalDiscretization is: "
              << std::string("EMcNeil") << "\n";
  }

  // auto w_grid = std::make_shared<gridLattice>( new gridLattice{
  // w_dbData.toString(DamBreakData::DiscrTypes::emcneil) } );
  auto w_grid1D = std::make_shared<gridLattice>(
      w_dbData.toString(DamBreakData::DiscrTypes::emcneil));
  std::shared_ptr<scalarField> w_U1{new scalarField{w_grid1D, "A"}};
  std::shared_ptr<scalarField> w_U2{new scalarField{w_grid1D, "Q"}};

  // fill the field for the current time period
  // with values from the appropriate function
  auto &w_U1val = w_U1->values(); // reference to modify it (A)
  auto &w_U2val = w_U2->values(); // reference to modify it (Q)

  // set discharge Q (set to zero follow E. McNeil source code)
  std::fill(w_U2val.getPtr() /*begin range*/,
            w_U2val.getPtr() + w_U2val.size() /*end range*/, 0. /*value*/);

  // sanity check
  if (std::all_of(w_U2val.getPtr() /*begin range*/,
                  w_U2val.getPtr() + w_U2val.size() /*end range*/,
                  std::bind(std::greater_equal<real>(), std::placeholders::_1,
                            0.)) == true) {

    dbpp::Logger::instance()->OutputSuccess(
        std::string{"Wave1DSimulator::setIC() done successfully"}.data());
  }

  for (auto i = 1; i <= w_U1->grid().getNoPoints(); i++) {
    // we assume that each section is unit width, this shall be
    // removed in the future version of the simulator
    // this is the same as "Evaluation_A_Enfonction_H"
    // m_I is the bottom topography and we substract water level
    // from Z that gives water depth
    // NOTE we have also m_lambda which i am not sure represent
    // water depth
    w_U1val(i) = m_H->valuePt(m_grid->getPoint(1, i), 0.) -
                 m_I->valuePt(m_grid->getPoint(1, i), 0.);
  }

  // RVO (Return Value Optimization)
  return StateVector{w_U1, w_U1};
}

// use the pre-defined function to set initial condition
// TODO: return StateVector
void Wave1DSimulator::setIC() {
  using namespace dbpp;

  // number of points in x-direction
  // const int nx = m_grid->getMaxI(1);

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
  std::fill_n(w_U1.begin(), (w_U1.size() / 2) + 1, m_Phi0); // m_Phi0
  //  std::fill_n(w_U1.begin(), std::prev(w_U1.end(), 50), 10.);
  std::fill_n(std::next(w_U1.begin(), w_U1.size() / 2), w_U1.size() / 2,
              m_Phi1); // m_Phi1

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
  // initial condition are set at the scan process
  // startup phase (scan method).
  auto *w_msg = "Initial condition startup phase (scan method)";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));

  setH();  // initial depth array
  setIC(); // set initial conditions

  // saveIC2File();  save initial data to file (no need to save that)
  initTime(); // ...

  // NOTE this part is done in the factory method (same test is performed)
  // check in which mode we are in the manual mode or we are using
  // the GUI to manage the simulation
  //  if (getSimulatorMode() == Wave1DSimulator::eSimulationMode::manualMode)
  //  {
  //    // creating our algorithm for this simulation
  //    m_numRep = createEMcNeil1DAlgo(); // algorithm name from comd line
  //    args if (m_numRep != nullptr) {
  //      auto *w_msg = "Created Numerical Scheme for manual mode";
  //      dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg));
  //    } else {
  //      auto *w_msg = "Could not create Numerical Scheme for manual mode";
  //      dbpp::Logger::instance()->OutputError(const_cast<char *>(w_msg));
  //    }
  //  } else // GUI mode
  //  {
  if (nullptr == m_numRep) {
    // create from user selection
    m_numRep = createEMcNeil1DAlgo(); // algorithm name from user selection
    if (m_numRep != nullptr) {
      dbpp::Logger::instance()->OutputSuccess(
          std::string{"Created Numerical Scheme for GUI mode"}.data());
    } else {
      dbpp::Logger::instance()->OutputError(
          std::string{"Could not create Numerical Scheme for GUI mode"}.data());

      exit(EXIT_FAILURE); // no numerical method no simulation!!! no point to
                          // go further
    }
  }
  //}

  // TODO: see Design Note inside and return a list of sections
  // create section flow
  if (nullptr != m_ListSectFlow) // from simulation GUI ON
  {
    delete m_ListSectFlow;
    m_ListSectFlow = nullptr;
  }

  createListSections();

  // TODO: ListSectionFlowObserver
  // in the current version, this all done when creating
  // list of sections flow (in ctor) which don't make sense.
  //     m_numRep->attach(m_ListSectFlow); // why not do that?
  //     m_ListSectFlow->setSubject(m_numRep);

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

  // rvalue reference???
  // Set solver initial condition
  m_numRep->setInitSln(m_u /*, m_ListSectFlow*/);

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

  // 		dbpp::Logger::instance()->OutputNewline();
  // 		// write to log file instead of writing to console. Use Regex to
  // find information 		dbpp::Logger::instance()->OutputSuccess(
  // "Iteration number is: %d",         m_tip->getTimeStepNo());
  // 		dbpp::Logger::instance()->OutputSuccess( "Time step computed is:
  // %f", m_tip->Delta()); 		dbpp::Logger::instance()->OutputSuccess(
  // "Simulation time computed is: %f", m_tip->time());

  //*****************
  // Design Note the signature of this method shall be somthing like
  // advance(t0,t1,dt,timestepCriteria) time loop is done inside
  // see "Dsn_FiniteDifferenceModel" advance method, its the way
  // it shall be done. In this method a call is made to explicit integrator
  // but this wrong, the model create the ODESolver, this must be replaced
  // by odeSolver->solve(...) which is more natural.
  // sanity check about the numerical algorithm that is running
  m_numRep->advance(/*m_u*/); // variable is updated (pass as reference)

#if 1 // next version (section flow updated at the end timeStep() when calling
      // notify()), i think that's the way it should be
  // NOTE
  //  Just don't need that!! because we have the state vector which is made
  //  made of shared_ptr that we pass as initial solution to numerical scheme
  //  which update these values, all we have to do (update water depth)
  //  list of sections is an observer of numerical scheme and is updated
  //  at each iteration (don't make sense)
  //  std::transform(m_ListSectFlow->getList().begin(),
  //                 m_ListSectFlow->getList().end(),
  //                 m_lambda->values().begin(), std::bind(&SectFlow::H,
  //                 std::placeholders::_1));

  // that will do exactly the same (dependency of list of sections on numerical
  // scheme can be removed)
  // NOTE we have A (cross-section surface: wetted area) but we solve for h
  // water depth for each cross-section.
  // Important to know the following:
  // 'H' is the water level measured about some threshold
  // 'h' is the water depth measure from topography (bottom)
  // we are solving swe for (h,hv) mass/momentum, so 'A' represent
  // 'h' (H-Z) for now we have Z=0. flat bed
  auto &w_hValues = m_u.first->values(); // A(h) water depth
  // std::copy(w_hValues.begin(), w_hValues.end(), m_lambda->values().begin());
  // could i do that copy ctor?? is it working?
  // good test for the copy ctor of numericla array!!!
  auto &lambdaArray = m_lambda->values();
  lambdaArray = w_hValues; // no need to call copy algorithm

  // now we have the water depth, but we need the water level
  // then if we want to update sections flow we can do it here
  auto w_sectUp2Date = m_ListSectFlow->getList(); // 100 or 101??
  auto begList = w_sectUp2Date.begin();
  // call Evaluation_H_
  while (begList != w_sectUp2Date.end()) // computational domain
  {                                      // but we need also the extra section
    // Important to note
    // section index 0...N-1
    // numerical array 1...N
    SectFlow *w_currSect = *begList++;
    w_currSect->setH(HydroUtils::Evaluation_H_fonction_A(
        w_hValues(w_currSect->getId() + 1), w_currSect->B(), w_currSect->Z()));
  }

  // not sure about this one
//  std::transform(m_lambda->values().begin(), m_lambda->values().end(),
//                 w_sectUp2Date.begin(),
//                 [](double aH, SectFlow &aSection) { aSection.setH(aH); });
#endif

  // save iteration result to file (more a debugging file, right we are not
  // really using it)
  if (isSaveResult2File()) {
    // call m_numRep->getState() and pass list of section flow
    // saveResult(m_numRep->getState(), m_ListSectFlow, m_tip->time());
    saveResult(m_u, m_lambda, m_tip->time());
  }
}

void Wave1DSimulator::initTime() {
  // according to Eric McNeil result, the initial time step
  // shall be equal to t: 0.6058 (make sure ...)
  // dbgui::Logger::instance()->OutputSuccess("Initial time step is: %f",
  // m_tip->Delta());
  using namespace dbpp;
  // initial condition for t=0. is set, we are ready for
  // the next time step. So increase time before we go
  // into the main simulation loop. Then since in our
  // algorithm we decide to compute the time step according
  // to some stability criteria, we must set the time
  // changing stepping mode, we would like, for some stability
  // criteria to change to a variable time stepping
  m_tip->setTimeStepMode(dbpp::TimePrm::TimeStepMode::VARIABLE_TIME_STEP);
  m_tip->initTimeLoop();              // initialize simulation time parameters
  m_tip->increaseTime(calculateDt()); // t1=t0+dt;

  // CFL shall be outside of the compute dt method
  // IMPORTANT use 'm_u' to compute the initial time step
  auto w_dt = calculateDt();

  // debug check (take scalarField as argument)
  const auto w_dtCheck =
      m_CFL * TimeStepCriteria::timeStep(*m_u.first, *m_u.second);

  assert(w_dt == w_dtCheck);

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
  dbpp::Simulation::instance()->incrIteration(); // ready for the first
                                                 // iteration iteration = 1
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

  // EMcNeilAlgo timestep()
  // Symetry: open/close at same location you write
  // initialize the sole instance of the logger (global instance)
  // dbpp::DbgLogger *w_dbgLog = dbpp::DbgLogger::instance();
  //  auto w_fileName = Simulation::instance()->getAlgorithmName();
  //  w_fileName += ".txt";
  //  w_dbgLog->open(
  //      w_fileName
  // w_save_file_name);  be careful because this is a default arg
  // which is not valid anymore but still in use
  // sanity check
  //  if (!w_dbgLog->isOpen()) {
  //    dbpp::Logger::instance()->OutputError(
  //        std::string{"Problem to open the debug log file\n"}.data());
  //  }

  // for code clarity
  dbpp::Simulation *w_sim = dbpp::Simulation::instance();

  // debugging purpose (correspond to the original number of iteration
  // for 22.5 sec) w_sim->setNbIterationMax(1);  temporary fix (set on the GUI
  // side)
  // assert(10 == w_sim->getNbIterationMax());  shall be put in a unit test
  //  auto check = w_sim->getNbIterationMax();

  // we are at first iteration and time is time step
  // initial cnd are has been set and we are ready
  // to start the simulation with first iteration.
  // NOTE first time we get in the loop,
  while ( //! m_tip->finished() ||in comment for debugging purpose
      w_sim->getIterationNumber() <=
      w_sim->getNbIterationMax()) // || mb iter != max iter not reached
  {
    // for now we use it in the global discretization boundary cnd.
    // emcil::Simulation::instance()->setSimulationTimeStep(w_dt);
    //
    // 			std::cout << "\n";
    // 			std::cout << "+++++++++++++Simulation parameters
    // for this time
    // step++++++++++++++\n"; 			std::cout << "Time step
    // computed is:
    // "
    // << m_tip->Delta() << "\n"; 			std::cout << "Simulation
    // time computed is: " <<
    // m_tip->time() << "\n"; 			std::cout << "Iteration
    // number is:
    // "
    // << m_tip->getTimeStepNo() << "\n"; 			std::cout <<
    // "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n";

#if 1
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
#endif

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
    // auto w_dt =
    //   m_CFL*TimeStepCriteria::timeStep(m_numrep->getState().first,
    //                                    m_numrep->getState().second)

    auto w_dt = calculateDt(); // just for check
    // simulation time is updated
    dbpp::Simulation::instance()->setSimulationTimeStep(w_dt);
    dbpp::Simulation::instance()->incrIteration(); // ready for next iteration
    m_tip->increaseTime(calculateDt());            // next time step t+=dt

    // block execution thread for 3 sec let time to the gui
    // to display info in the editor window
    std::this_thread::sleep_for(std::chrono::seconds(5));
  } // while loop (time loop)

  // finish writing, close file
  // open is done in Wave1DSimulator::createFolderAndFile() because
  // we need to set filename which is defined in this method
  // Design Note: something wrong with this, should use a string
  // to store file name for writing and then at some place open
  // with this file name. In its actual version, its hard to find
  // out where the open call is done.
  //  dbpp::DbgLogger::instance()->close();
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
    SweRhsAlgorithm *w_rhsTest = nullptr; // new TestRhsImpl(m_ListSectFlow);
    // just for testing and debugging
    w_num_rep = factoryCreator<TestEMcNeilVec>(w_rhsTest, TimePrm{0., 0., 0.});
    //  return nullptr;
  } else {
    // log entry
    dbpp::Logger::instance()->OutputError(
        std::string{"Unrecognised solver name: %s"}.data());
  }

  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Created Numerical Scheme successfully: %s"}.data(),
      m_activeAlgo.c_str());

  return w_num_rep;

  // Re-factor this methodcheck active algorithm

  if (m_activeAlgo == std::string{"EMcNeil1D_mod"}) {
    return w_num_rep = dbpp::EMcNeil1DFactory::CreateSolver(m_activeAlgo);
  } else if (m_activeAlgo == std::string{"EMcNeil1D_f"}) {
    return w_num_rep = dbpp::EMcNeil1DFactory::CreateSolver(m_activeAlgo);
  } else if (m_activeAlgo == std::string{"TESTEMCNEILVEC"}) {
    return nullptr;
  }
}

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
  saveResult(m_u, m_lambda, 0.);
}

// loop index has been changed ...
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
      //  printf("The file 'EMcNeil1D_mod.out' was opened\n");
      m_opened = true;
    } else {
      // printf("The file 'DamBreakIC.out' was not opened\n");
      Logger::instance()->OutputError(
          std::string{"The file 'EMcNeil1D_Result.txt' was not opened"}.data());
    }
  }

  using namespace dbpp;

  // int j; looping variable
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

// DESIGN NOTE: we have already set "H" values (setH() in the solveProblem()).
// Actually the "H" that we have set "setH()" correspond to m_lambda, which
// was pass as an argument to advance, which is not the case anymore. Now we
// using the GlobalDiscretization to retrieve the nodal variables that are
// updated after each iteration. IMPORTANT: we are now considering 100 section
// (domain discretization, at each node there is a section flow as it should
// be)
//  **It shall return a ListOfSections!! not void, it's a kind of factory
//  method!!
//    use of shared ptr for reference semantic??? maybe!
// IMPORTANT
//	Spécification des conditions initiales
//	Dans la version finale, ces conditions initiales "devraient être"
//	spécifiées sous la forme de l'élévation de la surface libre et
//	du débit pour chaque section
void Wave1DSimulator::createListSections() {
  // this is temporary fix for debugging purpose
  if (nullptr == m_ListSectFlow) {
    // number of grid node(computational domain)
    // NOTE m_lambda->values().size(); is probably more appropriate
    // we have a section at each grid node (just some checks to make sure we
    // we are on the right track) shall be removed in the future
    assert(m_lambda->values().size() == m_lambda->grid().getNoPoints());
    assert(m_lambda->grid().getMaxI(1) == m_lambda->grid().getNoPoints());
    // number of sections should be equal to the number of grid
    // of the computational domain
    m_ListSectFlow =
        new dbpp::ListSectFlow(m_numRep, m_lambda->grid().getMaxI(1));

    // const auto w_xmax = m_grid->xMax(1); // dimension 1

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

  //  if (m_simulatorMode == Wave1DSimulator::eSimulationMode::manualMode) {
  //    // debugging purpose (print all information about a section)
  //    std::for_each(m_ListSectFlow->getList().cbegin(),
  //                  m_ListSectFlow->getList().cend(),
  //                  std::mem_fn(&dbpp::SectFlow::printSectionInfo));
  //  }

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
  setIC();
  initTime();
}
} // namespace dbpp
