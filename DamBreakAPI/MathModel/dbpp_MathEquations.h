
#include <memory>
#include <string>

// l
#include "../Utility/dbpp_HydroCodeDef.h"

namespace dbpp {
/** @brief System Conservation Law Equations
 */
class SCLEquation {
  /**
   * @brief flux physical
   * @param aStateVec vector of state variable
   * @return physical flux at grid node points
   */
  virtual double flux(const StateVector &aStateVec) = 0;
  /**
   * @brief getDimension dimension of the physical system
   * @return number of dimension
   */
  virtual unsigned getDimension() const { return 0; }
  /**
   * @brief getNumberOfEquations math equations
   * @return number of equations
   */
  virtual unsigned getNumberOfEquations() const { return 0; }
};

/**
 * @brief createSclEquations factory method of mathematical equation creation
 * @return mathematical equation type
 */
std::shared_ptr<SCLEquation> createSclEquations();
} // namespace dbpp
