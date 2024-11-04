#pragma once

// C++ include
#include <cassert>
// stl includes
#include <list>
#include <vector>
// lib include
#include "../SfxTypes/dbpp_cellFace.h"

namespace dbpp {
/** Brief Possible implementation of numerical algorithm using base class of the
 * framework. In the current version, we have a base class for numerical
 * discretization of the right-hand-side terms. But user may want to provide its
 * own implementation of the flux algorithm (for example E. McNeil Nujic
 * algorithm but decided for some) reason to implement a different algo for flux
 * face evaluation.
 *
 */
class TestCalculFF // the flux algorithm
{
public:
  /**
   *   Some typedef for code clarity
   */
  typedef std::vector<std::pair<double /*UL*/, double /*UR*/>> cellfaceVar;

  /** In this version the reconstruction procedure have been removed inside of
   * the flux algorithm. This way we don't need to modify the flux algorithm to
   * change the reconstruction procedure just switch to another. By
   * encapsulating method in class, Usage: reconstr_vec( const scalarField&
   * aScalrField, , vecULUR); // return calculFF(..., vecULUR);
   */
  void calculFF(std::vector<double> &FF1,
                std::vector<double> &FF2, /*const std::vector<double>& U1,*/
                /*const std::vector<double>& U2,*/ const cellfaceVar &aU1LR,
                const cellfaceVar &aU2LR);

  /**
   *
   * Reconstruction outside of the Riemann solver (assume that ...)
   */
  void calculFF(const std::list<cellFace> &aListOface, std::vector<double> &FF1,
                std::vector<double> &FF2);
};
} // namespace dbpp
