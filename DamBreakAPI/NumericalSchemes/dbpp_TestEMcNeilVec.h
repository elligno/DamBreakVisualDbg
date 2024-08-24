#pragma once

// stl include
#include <vector>
// project includes
#include "../Algorithm/dbpp_TestRhsImpl.h"
#include "dbpp_HLL_RK2_Algo_Vec.h"

namespace dbpp {
// re-implement advance() with the new type for the Rhs
class TestEMcNeilVec : public EMcNeil1D {
  using swe_rhs = SweRhsAlgorithm::SWERHS;

public:
  TestEMcNeilVec();
  //	~TestEMcNeilVec();
  TestEMcNeilVec(const TestEMcNeilVec &) = delete;
  TestEMcNeilVec &operator=(const TestEMcNeilVec &) = delete;

  // copy and assignment ctor are declared private
  // as inheriting from boost noncopyable
protected:
  // form base class (this ???)
  void advance() override final;
  void setBC();

private:
  unsigned m_NbSections;
  void predictor();
  void corrector();
  swe_rhs m_rhs;
  // time stepping??
  std::vector<double> m_U1p; // future implementation
  std::vector<double> m_U2p;

  // migration (architecture modification base class older version inherit from
  // these 2 var init solution)
  std::vector<double> U1, U2; /**< state variables*/
  StateVector m_U12;          // temporary fix, this algo won't work anyway!!
  StateVector m_U12p;         /**< state vector for mid time step*/
};
} // namespace dbpp
