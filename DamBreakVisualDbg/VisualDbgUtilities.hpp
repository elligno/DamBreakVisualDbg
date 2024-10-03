#pragma once

// C++ includes
#include <functional>
#include <sstream>
// Qt include
#include <QTextStream>

namespace qplot {
/**
 * @brief Helper to define some application constant
 * @tparam T constant type (template automatic type deduction)
 */
template <auto v> struct AppConstant { static constexpr auto value = v; };
// E. McNeil constant used to validate code
using EMCNEILCTE = AppConstant<101>;

// Access copies of local variables at the time the lambda was created.
// Every call to make_offseter returns a new lambda function object through
// the std::function< > function wrapper. This is the safest form of local
// variable capture.
std::function<int(int)> make_offseter(int aOffset) {
  return [=](int j) { return aOffset + j; };
}

template <typename T> T read(std::istringstream &is) {
  T t;
  is >> t;
  return t;
}

// template <typename... Args> std::tuple<Args...> parse(std::istringstream &is)
// {
//  return std::make_tuple(read<Args>(is)...);
//}

// let compiler deduce return type
template <typename... Args> auto parse(std::istringstream &is) {
  return std::make_tuple(read<Args>(is)...);
}

// Read file format "full result" for each simulation time
// Use utility function to parse line (return a tuple with
// value in reverse order tuple index 4->0 instead of 0->4)
// Return a vector of values corresponding at simulation time.
template <typename... Args> // let compiler deduce return type
auto extractLineValuesTmplt(QTextStream &afileStream, QString &&aFirstLine) {
  QString w_line2Read{std::move(aFirstLine)};
  QVector<std::tuple<Args...>> w_vecoftpl;
  w_vecoftpl.reserve(101); // hard coded!!!
  do {
    // replace sequence of white by a single one
    auto w_lineTrimmed = w_line2Read.simplified().toStdString();
    // step to next white space one after one
    std::istringstream w_iis{w_lineTrimmed};
    // parse a line with format X|H|A|...
    auto w_tplResult = parse<Args...>(w_iis);
    w_vecoftpl.push_back(
        std::move(w_tplResult)); // not sure we are moving something!! No, see
                                 // Scott Meyer's book item #25
  } while (afileStream.readLineInto(&w_line2Read) && !w_line2Read.isEmpty());

  return w_vecoftpl;
}

// See Note above
// reverse tuple index order (problem when parsing each line and return a tuple)
template <class Tuple, std::size_t... Is>
Tuple tuple_inverse_idx_order(const Tuple &t, std::index_sequence<Is...>) {
  return std::make_tuple(std::get<Is>(t)...);
}
} // namespace qplot
