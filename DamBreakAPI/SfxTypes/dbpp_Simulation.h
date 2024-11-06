#pragma once

// C++ includes
#include <string>
// STL include
#include <list>
// Library include
#include "dbpp_Singleton.hpp"
//#include "../Utility/dbpp_SimulationUtility.h"
#include "../SfxTypes/dbpp_DamBreakData.h"

namespace dbpp {
/** @brief Just a prototype of a class that manage the simulation parameters.
 *	 These parameters could be read from an XML file for example or any
 *other format. Use the CRTP (Curious Recursive Template Pattern) to implement
 *the simulation bean. Bean that represents a simulation in the framework. Many
 *of the attributes of the simulation bean are defined in the
 *Simulation.properties file by the user of the framework. The Controller class
 *uses the simulation bean to control the creation of the correct classes for
 *the simulation.
 */
class Simulation : public Singleton<Simulation> {
public:
  /**
   * @return simulation time
   */
  double simulationTime() const { return m_time; }
  /**
   * @return simulation time step
   */
  double simulationTimeStep() const { return m_dt; }
  /**
   * @param time step
   */
  void setSimulationTimeStep(double aDt) {
    m_dt = aDt;
    m_time += m_dt;
  }
  /**
   * @return simulation status
   */
  bool isRunning() const { return m_running; }
  /**
   * @param simulation status
   */
  void setRunning(bool aIsRunning = true) { m_running = aIsRunning; }
  /**
   * @return name of the algorithm
   */
  std::string getAlgorithmName() const { return m_algoName; }

  void setAlgorithmName(const std::string aName) { m_algoName = aName; }

  /**
   * @return number of iteration
   */
  unsigned getIterationNumber() const { return m_iterNumber; }
  /**
   * @return number of iteration set to zero
   */
  void resetIterationNumber2Zero() { m_iterNumber = 0; }

  /**
   * @return
   */
  void incrIteration() { ++m_iterNumber; }

  /**
   * @return maximum number of iteration
   */
  unsigned getNbIterationMax() const { return m_nbIterMax; }

  void setNbIterationMax(const unsigned aNbIterMax) {
    m_nbIterMax = aNbIterMax;
  }
  /**
   * @return Courant-Friedrich-Levy number
   */
  double getCFLNumber() const { return m_cfl; }
  /**
   * Courant-Friedrich-Levy number for this simulation
   */
  void setCFL(const double aCFL) { m_cfl = aCFL; }
  /**
   * @return number o f section flow for this simulation
   */
  unsigned getNbSections() const { return m_NbSections; }

  void setNbSections(const unsigned aNbSect) { m_NbSections = aNbSect; }

  /** read form simulation settings (GUI interface)
   * in the current version it's a text file, but
   * in future we will use XML file and the GUI.
   */
  void readParams() const {
    // not implemented yet
  }

  bool useFlatBed() const { return true; }
  bool isSectionFlowUnitWidth() const { return true; }

  /**
   * @return
   */
  // 		void setDBdata( DamBreakData::DiscrTypes aType)
  // 		{
  // 			// set to this for debugging purpose
  // 			// when loading simulation set parameters from reading
  // file 			m_dbDataType=aType;
  // 		}
  //
  // 		DamBreakData::DiscrTypes getDiscrType() const {return
  // m_dbDataType;}

  // client side in production code
  // Design Note
  // -----------
  //  Since we have copy and assignment ctor private (boost::noncopyable)
  //  we cannot return by copy (error message complaining about access
  //  private member of the class). Need to think if we really need it
  //  noncopyable???
  const DamBreakData &getActiveDBdata() const { return m_dbData; }
  // for now we leave it like that, confusing since we have dbData
  void setActiveDiscretization(dbpp::DamBreakData::DiscrTypes aDataType) {
    m_dbDataType = aDataType;
  }
  auto getActiveDiscretization() const { return m_dbDataType; }

  /** declare as friend of the class to access protected default ctor*/
  friend class Singleton<Simulation>;

protected:
  Simulation(); // access denied
private:
  DamBreakData m_dbData; /**< dam-break data for this simulation*/
  DamBreakData::DiscrTypes
      m_dbDataType;       //**< Hudson as default, otherwise EMcNeil*/
  double m_dt;            /**< simulation time step*/
  double m_time;          /*< simulation time */
  std::string m_algoName; /**< .. */
  bool m_running;         /**< .. */
  unsigned m_iterNumber;  /**< .. */
  unsigned m_nbIterMax;   /**< .. */
  double m_cfl;           /**< ... */
  unsigned m_NbSections;  /**< ... */
  std::list<std::string> m_listOfAlgorithm; /**< ... */
  void loadAlgorithm();
}; // class
} // namespace dbpp
