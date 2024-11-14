#pragma once

// C++ include
#include <cassert>
// stl includes
#include <list>
#include <vector>
// lib includes
#include "../SfxTypes/dbpp_BaseMacros.h"
#include "../SfxTypes/dbpp_ExceptionUtils.h"
#include "../SfxTypes/dbpp_cellFace.h"
#include "../Utility/dbpp_AppConstant.hpp"

namespace dbpp {
/** @brief Possible implementation of numerical algorithm using base class of
 * the framework. In the current version, we have a base class for numerical
 * discretization of the right-hand-side terms. But user may want to provide its
 * own implementation of the flux algorithm (for example E. McNeil Nujic
 * algorithm but decided for some) reason to implement a different algo for flux
 * face evaluation.
 *
 */
class TestCalculFF // the flux algorithm
{
public:
  /** Alias for code clarity
   */
  using cellfaceVar = std::vector<std::pair<double /*UL*/, double /*UR*/>>;

  /** @brief In this version the reconstruction procedure have been removed
   * inside of the flux algorithm. This way we don't need to modify the flux
   * algorithm to change the reconstruction procedure just switch to another. By
   * encapsulating method in class, Usage: reconstr_vec( const scalarField&
   * aScalrField, , vecULUR); // return calculFF(..., vecULUR);
   */
  void calculFF(std::vector<double> &FF1, std::vector<double> &FF2,
                const cellfaceVar &aU1LR, const cellfaceVar &aU2LR);

  /** @brief Reconstruction outside of the Riemann solver (assume that ...)
   *  compute numerical ayt cell face j+1/2
   */
  void calculFF(const std::list<cellFace> &aListOface, std::vector<double> &FF1,
                std::vector<double> &FF2) {
    // temporary fix for now (debugging)
    assert(aListOface.size() == DIM::value); // computational domain
    SFX_REQUIRE(DIM::value == FF1.size(), "Face Flux of different size");
    SFX_REQUIRE(DIM::value == FF2.size(), "Face Flux of different size");
  }
};
} // namespace dbpp
