
// Author: Jean Belanger (Elligno Inc.)
// Date of creation: July 21, 2007
// Revision history:

#ifndef phyconstant_H
#define phyconstant_H

namespace dbpp {
/** Brief Physical constant.
 *   Define some physical constant
 */
class PhysicalConstant {
public:
  static const double sGravity; /**< gravity constant */
};

//------------------------------------------------------------
//                    Math Utilities
//------------------------------------------------------------

/**
 * @brief Gravity acceleration constant (m/s^2)
 * @tparam T float number precision
 */
template <typename T> constexpr T cGravity{9.80665};

/**
 * @brief Helper to define some application constant
 * @tparam T constant type
 */
// template <auto v> struct AppConstant {
//  static constexpr auto value =
//      v; // inline variable and a definition since C++17
//};

/**
 *   @brief E. McNeil constant for the number of sections (scenario)
 */
// using EMCNEILNbSections = AppConstant<101>;
} // namespace dbpp
#endif // Include guard
