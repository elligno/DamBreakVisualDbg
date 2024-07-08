
#pragma once

// C/C++ includes
#include <stdio.h>
#include <tuple>
#include <utility>
// stl include
#include <list>
// boost include
#include <boost/filesystem.hpp> // boost file system utility
// BaseNumTypes library includes
#include "dbpp_TimePrm.h"
#include "dbpp_WaveFunc.h"

// project include
#include "../NumericalSchemes/dbpp_HLL_RK2_Algo_Vec.h"

// BaseNumTypes library (deprecated)
using wavfuncptr = std::shared_ptr<dbpp::WaveFunc>;
using TimePrmPtr = std::shared_ptr<dbpp::TimePrm>;

// typedef std::tuple<int /*index*/,real/*A*/,real/*Q*/,real/*H*/> bndnode;

namespace dbpp {
// forward declaration
class Observer;
class ListSectFlow;
class SemiDiscreteModel;
} // namespace dbpp
namespace dbpp {
//
// Testing GUI development with a simple case
//
// REMARK:
//   warning C4251: 'dbpp::Wave1DSimulator::m_grid' : class 'elgo_ptr<T>' needs
//   to have
//    dll-interface to be used by clients of class 'dbpp::Wave1DSimulator'
//    can export template from dll because we need that all member template be
//    instantiated. One possibility, make Wave1DSimulator fully abstract class
//    and export it. This class implementation will be created on client side
//    (work around for now). This means that in our GUI project, we will inherit
//    from this class
//
//    Reference
//       "How To Export C++ Classes From a DLL", Alex Blekhman, 13 nov 2012
//       Code Project web site
//
//   Very important to read and understand this article, because it's breath and
//   butter of COM approach (create a factory that return pointer to base
//   abstract type). Wave1DSimulator is nothing else than a POM/COM which is not
//   exported. Actually it implements an interface (pure abstract class) like
//   jb_ClassDll

/** Brief Main class of the application (DamBreak simulator).
 *    Responsible to run the simulation by using solver.
 *    This class sets up the simulation from the properties file and then runs
 * it based on the configuration classes. The main method of this class executes
 * the steps of the simulation.
 */
class Wave1DSimulator {
public:
  /**
   * Simulator run mode
   */
  enum class eSimulationMode {
    manualMode = 0, /**< default mode which command line */
    guiMode = 1     /**< run from GUI */
  };

  /**
   * Simulator state mode
   */
  enum class eSimulationState {
    running = 0, /**< running mode */
    pause = 1,   /**< paused mode */
    stopped = 2  /**< stopped mode */
  };

  /**
   * Simulator time stepping mode
   */
  enum class eTimeStep {
    variable = 0, /**< variable time step */
    fixed = 1     /**< fixed time step */
  };

public:
  /**
   *  default ctor (these values are E. McNeil code source)
   */
  Wave1DSimulator(unsigned int aNbIterationsMax = 50, double aCFL = 0.6);
  /**
   *  destructor
   */
  ~Wave1DSimulator();

  /**
   *   main loop (implemented from base class)
   */
  virtual void run() {
    // running the simulation in mode
    // check mode: single step or run nbIter?
    // timeLoop();
    // doOneStep();
  } // not completed!!

  /**
   *  Save initial condition to file on disk
   */
  virtual void saveIC2File();

  /**
   *   data store for final result (to be moved to implementation class??)
   */
  virtual void saveResult(const StateVector &aStateVec, const fieldptr &aFptr,
                          double aTime);
  /**
   *  can be overriden by subclass (time stepping loop)
   */
  virtual void timeLoop();
  /**
   *  set simulation mode
   * @param simulation mode
   */
  void setSimulatorMode(const eSimulationMode aSimode) {
    m_simulatorMode = aSimode;
  }
  /**
   * @return simulation mode
   */
  eSimulationMode getSimulatorMode() const { return m_simulatorMode; }
  /**
   *  create the numerical algorithm for this simulation
   * @return smart pointer on base class
   */
  std::shared_ptr<dbpp::EMcNeil1D> createEMcNeil1DAlgo();

  // i do not know what the hell is that? this whole class need to be
  // re-factored it's a big mess, i need a serious clean-up namespace jb must be
  // removed
  /**
   *  Create a list of cross-section for the simulation
   */
  void createListSections();

  // mainly for GUI interaction
  /**
   *  set algorithm name
   *  @param aAlgoName name of the algorithm
   */
  void setActiveAlgorithm(const std::string &aAlgoName) {
    if (!m_activeAlgo.empty()) {
      m_activeAlgo.clear(); // size to zero (not sure but quite )
    }
    m_activeAlgo = aAlgoName;

    // Design Note
  }
  /**
   *  return algorithm name
   * @return active algorithm string representation
   */
  const std::string getActiveAlgorithm() const { return m_activeAlgo; }

  /**
   * Numerical algorithm for this simulation (actually its no more than the
   * ODESolver) In current version concept overlap (solver and numerical
   * algorithm) "EMcNeil1d" is a representation of the numerical scheme
   * (Godunov-type scheme) Responsible to step through time the numerical
   * representation of the math equations.
   * @param aNumAlgo smart pointer on base algorithm class
   */
  void setNumericalAlgorithm(const std::shared_ptr<dbpp::EMcNeil1D> &aNumAlgo) {
    // Design Note shall create the physical algorithm (i don't see why ...)
    // not sure why it do anything else than creating the numerical scheme
    m_numRep = aNumAlgo;
  }
  std::shared_ptr<dbpp::EMcNeil1D> getNumericalLAgorithm() const {
    return m_numRep;
  }

  /** optimization return value (check Scott Meyers book second one "More
   * Effective C++") about returning value.
   *  @result container of supported algorithm
   */
  std::vector<std::string> getListAlgoSupported() {
    return std::vector<std::string>(m_supportedAlgorithm.begin(),
                                    m_supportedAlgorithm.end());
  }

  /**
   * @return the list of section
   */
  dbpp::ListSectFlow *getListSectionFlow() const { return m_ListSectFlow; }

  // Design Note: this will be part of the GUI interface (GUI package)
  // Provides interface (services that client need to implement).
  // GUI simulator need some information about active algorithm
  // By default simulator set an algorithm (ctor), GUI let user
  // set the physical algorithm for the simulation
  // 		void setActiveAlgorithm( const std::string& aActiveAlgo)
  // 		{
  // 			// - check if the algorithm is the same as simulator
  // default
  // 			// - if not, the check if the passed algorithm name is
  // supported
  // 			// - if yes, then need to create it with the algorithm
  // factory
  // 			// - ... to be completed
  // 		}
  // 		std::string getActiveAlgorithm() {return m_activeAlgo;}

  // directory where to store data
  void setValidationDir(const std::string &aValidationDir) {
    m_validationDir = aValidationDir;
  }
  std::string getValidationDir() const { return m_validationDir; }

  // flag to set if data is to be saved to file
  void setSaveResult2File(bool saveResult) { m_saveResult2File = saveResult; }
  bool isSaveResult2File() { return m_saveResult2File; }

  // ... to be completed
  void setIterationNumberMax(const unsigned aNumMaxIter) {
    m_NumberIterationsMax = aNumMaxIter;
  }
  unsigned getNumberIterationsMax() const { return m_NumberIterationsMax; }

  /**< read input and initialize */
  virtual void scan();

  /**< start the simulation */
  virtual void solveProblem();

  /**< Initialization simulator */
  void initialize();

  // DESIGN NOTE
  //  Call by GUI SLOT (Init Simulation) in the menu
  //  m_wavSim->initializeGuiMode();
  //  Simulator attributes for Phis have been set by
  //  double spin box (GUI) with SLOT for each of them.
  //  m_wavSim->setPhi1(ui->spinBphi1.value()) ...
  //  when this function will be called from GUI, it setup
  //  the initial condition mainly: water depth, then IC
  //  and time stepping parameters to be used for this.
  void initializeGuiMode() {
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

  /** Courant-Friedrich-Number*/
  void setCFL(const double aCFl) { m_CFL = aCFl; }
  double getCFL() const { return m_CFL; }

  /** computational domain extension*/
  /**
   * @param minimum x coordinate
   */
  void setXmin(const double aXmin) { m_xmin = aXmin; }
  /**
   * @result return minimum x coordinate
   */
  double getXmin() const { return m_xmin; }
  /**
   * @param maximum x coordinate
   */
  void setXmax(const double aXmax) { m_xmax = aXmax; }
  /**
   * @result return maximum x coordinate
   */
  double getXmax() const { return m_xmax; }

  // dambreak parameters (VS15 can i use constexpr and noexcept?)
  /**
   * @param minimum x coordinate
   */
  void setPhi1(double aPhi1) { m_Phi1 = aPhi1; }
  double getPhi1() const { return m_Phi1; }
  void setPhi0(double aPhi0) { m_Phi0 = aPhi0; }
  double getPhi0() const { return m_Phi0; }
  void setShockLocation(double aShokLoc) { m_shockLoc = aShokLoc; }
  double getShockLocation() const { return m_shockLoc; }

  /**
   *   time parameters of the simulation
   *  @param
   */
  void setFinalTime(const double aFinalTime) {
    m_finalTime = aFinalTime;

    // need to change the stop time of the timer!!
    m_tip->setStopTime(aFinalTime);
  }
  /**
   */
  double getFinalTime() const { return m_finalTime; }
  /**
   */
  void doOneStep();
  /**
   */
  void initTime();

  // DEPRECATED
  // temporary fix, this will be removed in the next version
  // shall be done by the scheduler or something else
  virtual real calculateDt(); // calculate optimal dt (the timestep)

  StateVector getIC(); /**<  Initial condition*/
protected:
  virtual void setIC(); // set initial conditions
  virtual void setH();  /**< load H into lambda for efficiency*/

  // virtual real calculateDt( /*int func*/);  calculate optimal dt (the
  // timestep) virtual void plot();  deprecated
  //	void setListSectFlow( std::shared_ptr<emcil::SemiDiscreteModel>
  //&w_num_rep);
private:
  // DEPRECATED
  Mesh1D m_grid;    /**< lattice grid here 1D grid */
  StateVector m_up; /**< solution u at time level l   (U1,U2) -> (A,Q) */
  StateVector m_u;  /**< solution u at time level l+1 (U1,U2) -> (A,Q) */
  // End DEPRECATED

  fieldptr m_lambda; /**< variable coefficient (depth) */
  //	fieldptr m_tmp;      /**< variable coefficient (depth) */
  TimePrmPtr m_tip; /**< time discretization parameters */
  wavfuncptr m_H;   /**< depth function */
  wavfuncptr m_I;   /**< initial surface function */

  // i am not sure about this one (still use it? where and what for?)
  // std::list<dbpp::Observer*> m_listObs; /**< list of observer */
  dbpp::ListSectFlow *m_ListSectFlow; /**< list of section flow */

  //	double *Z, *n;  /**< bathymetry and amnning coefficient */
  // coefficients of mathematical model
  double /*n1, V0, V1, R1, Y1,*/ Sf1, S0am, S0av; /**< slope function */
                                                  // not sure about those? why?
  double m_CFL;  /**< Courant-Friedrich-Levy number */
  double m_xmin; /**< x minimum coordinate */
  double m_xmax; /**< x maximum coordinate */

  // maybe i should use the ofstream instead
  // in the next version, for now we want to validate
  // 		FILE *FichierDEBUG;
  // 		char NomFichierDEBUG[256];

  FILE *FichierResultat;        /**< depth function */
  char NomFichierResultat[256]; /**< depth function */
  // some variables used in the original version
  bool m_opened;  /**< depth function */
  int NbSections; /**< depth function */
  std::string m_activeAlgo;
  std::string m_validationDir; // AlgoValidation
  std::shared_ptr<dbpp::EMcNeil1D> m_numRep;
  bool m_saveResult2File; // true as default
  unsigned int m_NumberIterationsMax;
  //		void createFolderAndFile();
  //    std::string CreateAlgoFile(const std::string& aCurDirStr = "");
  //		void updateHFromGDiscr(dbpp::RealNumArray<real>& w_tmpH);
  double m_Phi1;
  double m_Phi0;
  double m_shockLoc;
  double m_finalTime;
  eSimulationMode m_simulatorMode;
  std::vector<std::string> m_supportedAlgorithm;
};
} // namespace dbpp
