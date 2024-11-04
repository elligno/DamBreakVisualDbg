#pragma once

// STL includes
#include <valarray>
#include <vector>
// lib includes
#include "../Discretization/dbpp_EMcnilGamma.h"
#include "../Utility/dbpp_HydroCodeDef.h"

namespace dbpp { // forward declaration
class ListSectFlow;
}

namespace dbpp {

/** @brief Wrapper around some implementation of the algorithm
 *  to be completed
 */
class SourceTermsTreatment {
public:
  /**
   *   signature which is probably more natural
   */
  std::valarray<double>
  TraitementTermeSource2(const StateVector &aStateVec,
                         const ListSectFlow *aListSectFlow);

  std::valarray<double>
  TraitementTermeSource2(const StateVector &aStateVec, const Gamma &aBC,
                         const ListSectFlow *aListSectFlow);

  bool useManningFormula() { return true; }

private:
  bool m_useManning; /**< Manning formula*/
  /** @brief
   *  @deprecated signature of this method is deprecated
   */
  void TraitementTermeSource2(std::vector<double> &S,
                              const std::vector<double> &Q,
                              const std::vector<double> &A, const double aDx,
                              const ListSectFlow *aListSectFlow);

  /** @brief
   *   signature which is probably more natural (bottom slope)
   */
  std::valarray<double> TraitementTermeS0(const std::vector<double> &Q,
                                          const std::vector<double> &A,
                                          const double aDx,
                                          const ListSectFlow *aListSectFlow);
};
} // namespace dbpp
