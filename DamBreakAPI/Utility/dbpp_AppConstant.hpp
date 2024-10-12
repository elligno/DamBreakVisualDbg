#pragma once

namespace dbpp {
/**
 * @brief Helper to define some application constant
 * @tparam v constant type (template automatic type deduction)
 *
 *  Note inline variable and a definition since C++17
 */
template <auto v> struct AppConstant { static constexpr auto value = v; };

/**
 *   @brief E. McNeil constant for the number of sections (scenario)
 */
using EMCNEILNbSections = AppConstant<101>;
} // namespace dbpp
