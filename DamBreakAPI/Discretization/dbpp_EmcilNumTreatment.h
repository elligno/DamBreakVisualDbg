#pragma once

// stl includes
#include <tuple>
#include <vector>

namespace dbpp {
/** Brief Base implementation according to Eric McNeil.
 * To be overriden by subclass (IPhysicalAlgorithm)
 * provide an interface with services to implement
 * physical based algorithm to solve st-Venant equations
 * in a conservative form (divergence). Eric McNeil original
 * code use an approach based on Nujic but implement the
 * numerical flux with a HLL algorithm. Nujic provides different
 * implementation of the flux algorithm, ENO type scheme,
 * Roe simplified version and i don't remember the last one.
 * Re-use Nujic implementation of the source term and pressure terms,
 * but provide also
 *
 *  Design Note
 *    All the signature of these methods will removed (deprecated)
 *    in the next version of this implementation. The framework
 *    provides math types such as scalarFiel, vectorFiled and tensorField
 *    which are more representative and have the information.
 *
 *    Refactoring Note (July 2024)
 *     This class should be static, main responsibility is to provides
 *     discetization for the RHS terms. Here, this is E. McNeil implementation.
 *     Also, method's signature should be revised accordingly.
 *   Reference: Nujic (1995) paper "Efficent Implementation of Non-Oscillatory
 *   Schemes for the Computation of the free-Surface flows"
 */
class BaseNumTreatmemt {
public:
  /** Numerical algorithm to calculate the pressure term.
   *
   * @param PF2 ...
   * @param P2 pressure
   * @param U1 state variable for wetted area
   * @param NbSections number of sections
   * @param B width of the section
   */
  void TraitementTermeP(std::vector<double> &PF2, std::vector<double> &P2,
                        const std::vector<double> &U1, const int NbSections,
                        const double B = 1);

  /** Numerical algorithm to calculate the pressure term.
   *
   * @param S0 ...
   * @param Z pressure
   * @param dx state variable for wetted area
   * @param NbSections number of sections
   */
  void CalculS0(std::vector<double> &S0, const std::vector<double> &Z,
                const double dx, const int NbSections);

  /** Numerical algorithm to calculate the pressure term.
   *
   * @param PF2 ...
   * @param P2 pressure
   * @param U1 state variable for wetted area
   * @param NbSections number of sections
   * @param B width of the section
   * @param Z pressure
   * @param dx state variable for wetted area
   * @param NbSections number of sections
   */
  void TraitementTermeSource(std::vector<double> &S,
                             const std::vector<double> &Q,
                             const std::vector<double> &A,
                             const std::vector<double> &Z,
                             const std::vector<double> &n, const double dx,
                             const int NbSections, const double B = 1);

  /** Numerical algorithm to calculate the pressure term.
   *
   * @param S source (slope and bottom)
   * @param Q discharge state variable
   * @param A wetted area state variable
   * @param H water level
   * @param B section width
   * @param Z pressure
   * @param dx state variable for wetted area
   * @param NbSections number of sections
   */
  void TraitementTermeSource2(std::vector<double> &S,
                              const std::vector<double> &Q,
                              const std::vector<double> &A,
                              const std::vector<double> &H,
                              const std::vector<double> &n, const double dx,
                              const int NbSections, const double B = 1.);

  /** Numerical algorithm to calculate the pressure term.
   *
   * @param PF2 ... to be completed
   * @param P2 pressure
   * @param U1 state variable for wetted area
   * @param NbSections number of sections
   * @param B width of the section
   * @param Z pressure
   * @param dx state variable for wetted area
   * @param NbSections number of sections
   */
  // deprecated
  void CalculFF(std::vector<double> &FF1, std::vector<double> &FF2,
                const std::vector<double> &U1, const std::vector<double> &U2,
                std::vector<double> &dU1, std::vector<double> &dU2,
                const int NbSections, const double B = 1.);

  /** Numerical algorithm to calculate the pressure term.
   *
   * @param FF1 ...
   * @param FF2 pressure
   * @param U1 state variable for wetted area
   * @param U2 state variable for discharge
   * @param B width of the section
   */
  void CalculFF(std::vector<double> &FF1, std::vector<double> &FF2,
                const std::vector<double> &U1, const std::vector<double> &U2,
                const double B = 1.);
};
} // namespace dbpp
