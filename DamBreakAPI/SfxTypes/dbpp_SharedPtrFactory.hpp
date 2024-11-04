#pragma once

// C++ include
#include <memory> // smart pointer
namespace dbpp {

// S. Meyers factory (std::unique pointer)
// "Effective Modern C++" book using unique_ptr

/** @brief Another factory from an article
 * "A Brief Introduction to Rvalue Reference"
 * Forward reference can bind to factory parameters
 * according to value categorie (preserve the
 * lvalue/rvalue-ness of the argument that is passed
 * to factory)
 */
template <typename T /*Type*/, typename Args /*Argumens*/>
std::shared_ptr<T> factory(Args &&aArgs) {
  return std::shared_ptr<T>{new T{std::forward<Args>(aArgs)}};
}

/** @brief Factory based on perfect-forwarding.
 *  Arguments can bind to any types (based on value categorie)
 *  to the factory parameters. Forward reference preserves the
 *  lvalue/rvalue-ness of the argument that is pass to the factory.
 * @tparam Args ctor argument type
 * @tparam T Type created
 * @return Type smart pointer (pointed-to)
 */
template <typename T, typename... Args>
std::shared_ptr<T> factoryCreator(Args &&...aArgs) {
  return std::shared_ptr<T>{new T{std::forward<Args>(aArgs)...}};
}
} // namespace dbpp
