#pragma once

#include <cassert>
#include <numeric> //adjacent_diffeence
#include <vector>

namespace dbpp {
/** We are testing an implementation of the upwind scheme
*   using generic programming (STL). It will be part of the
    numerical library.*/
template <typename Range> // Range of values
std::vector<typename Range::value_type> UpwindDerivatr1st(const Range &aRng) {
  typedef typename std::iterator_traits<typename Range::iterator>::value_type
      value_type;
  // typedef typename std::iterator_traits<typename
  // Range::iterator>::difference_type diff_t;

  // diff_t w_Length = std::distance(std::begin(aRng), std::end(aRng));
  std::vector<value_type> w_Cont(
      std::distance(std::begin(aRng), std::end(aRng)));

  // compute the gradient (U_i-U_i-1) at first order
  std::adjacent_difference(std::begin(aRng), std::end(aRng), w_Cont.begin());
  assert(w_Cont.size() ==
         std::distance(std::begin(aRng), std::end(aRng))); // really need it?

  // 		w_Cont.pop_front();  // remove first element, not part of the
  // result
  // 		assert( w_Cont.size()==w_Length-1);

  // it shall return the result of adjacent difference
  // return std::vector( w_Cont.begin(), w_Cont.end());
  return w_Cont;
}
} // namespace dbpp
