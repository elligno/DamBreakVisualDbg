#pragma once

// stl include
#include <vector>
// Library includes
#include "../Algorithm/dbpp_TestRhsImpl.h"
#include "dbpp_HLL_RK2_Algo_Vec.h"

namespace dbpp { // forward declaration
class SweRhsAlgorithm;
}

namespace dbpp {
/** @brief
 * We are using the new concept of the RHS algorithm (step of the algo)
 * wrapped inside of the algo, this way we can make different algorithm
 * base on the numerical representation (treatment of the rhs).
 */
class TestEMcNeilVec : public EMcNeil1D {
  using swe_rhs = SweRhsAlgorithm::SWERHS;

public:
  TestEMcNeilVec(SweRhsAlgorithm *aRhsAlgo, const Gamma &aBCnd);
  TestEMcNeilVec(SweRhsAlgorithm *aRhsAlgo, const TimePrm &aTimePrm);

  void advance() override final;
  void mainLoop(const GlobalDiscretization *aGblDiscr,
                const double aTimeTo) override final;

  void initialize(const GlobalDiscretization *aGblDiscr,
                  double aTime) override final;

  void setInitSln(const StateVector &aU) override final
  // ListSectFlow *aListofSect = nullptr) override final
  {
    m_U12 = aU; // just trying something
  }

  // not completed (return by copy, C++17 copy elison is mandatory)
  StateVector getState() const override { return m_U12; }

protected:
  void timeStep() override final {}

private:
  SweRhsAlgorithm *m_rhsAlgo;
  TimePrm m_timePrm;
  const Gamma m_bc;
  // swe_rhs m_rhs;
  // time stepping??
  //  std::vector<double> m_U1p; // future implementation
  //  std::vector<double> m_U2p;

  // migration (architecture modification base class older version inherit from
  // these 2 var init solution)
  std::vector<double> U1, U2; /**< state variables*/
  StateVector m_U12;          // temporary fix, this algo won't work
  //  anyway!! StateVector m_U12p;         /**< state vector for mid time step*/
};
} // namespace dbpp
