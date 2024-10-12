#pragma once

#include <functional>

namespace qplot {
/** Access copies of local variables at the time the lambda was created.
 * Every call to make_offseter returns a new lambda function object through
 * the std::function< > function wrapper. This is the safest form of local
 * variable capture.
 */
std::function<int(int)> make_offseter(int aOffset) {
  return [=](int j) { return aOffset + j; };
}
} // namespace qplot
