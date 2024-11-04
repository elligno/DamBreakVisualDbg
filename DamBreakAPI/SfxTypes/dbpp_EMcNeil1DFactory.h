#pragma once

// C++ include
#include <memory> // shared_ptr
// Library includes
#include "../NumericalSchemes/dbpp_HLL_RK2_Algo_Vec.h"
#include "../SfxTypes/dbpp_DamBreakData.h"

namespace dbpp {
/** Class that represent parameters settings for factory class.
 *
 */
class EMcNeilSettings {
public:
  EMcNeilSettings() : m_dbData(DamBreakData::DiscrTypes::emcneil) {}

  void readFromFile() { throw "Not implemented yet"; }

private:
  DamBreakData m_dbData;
};

/** Factory to create EMcNeil1D instance (related class).
 *
 */
class EMcNeil1DFactory {
  /** C++11 enumeration (strongly type enum)
   *
   */
  enum class EMcNeil1DType // supported implementations
  {
    //	    EMCNEIL1D = 0,
    EMCNEIL1D_MOD = 0,
    EMCNEIL1D_F = 1,
    TESTEMCNEILVEC = 2,

    // Add implementations here (of what?)
    TESTBCSECTF = 3,
    TESTNEWALGO = 4
  };
  EMcNeil1DFactory();
  // ~EMcNeil1DFactory();

public:
  static std::shared_ptr<dbpp::EMcNeil1D>
  CreateSolver(EMcNeil1DType eType /*, const DamBreakData &rSettings*/);
  static std::shared_ptr<dbpp::EMcNeil1D>
  CreateSolver(const std::string &rstrType /*, const DamBreakData &rSettings*/);
  static std::string GetName(EMcNeil1DFactory::EMcNeil1DType eType);
  static EMcNeil1DFactory::EMcNeil1DType GetType(const std::string &rstrType);
};
} // namespace dbpp
