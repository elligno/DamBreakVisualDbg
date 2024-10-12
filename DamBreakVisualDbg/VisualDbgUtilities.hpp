#pragma once

// C++ include
#include <sstream>
// Qt include
#include <QTextStream>

namespace qplot {

/** @brief Read value from a stream
 *  @return type
 */
template <typename T> T read(std::istringstream &is) {
  T t;
  is >> t;
  return t;
}

// template <typename... Args> std::tuple<Args...> parse(std::istringstream &is)
// {
//  return std::make_tuple(read<Args>(is)...);
//}

/** @brief Parse a line and return values as tuple
 *  @return let compiler deduce return type
 */
template <typename... Args> auto parse(std::istringstream &is) {
  return std::make_tuple(read<Args>(is)...);
}

/** @brief Read file format "full result" for each simulation time
 * Use utility function to parse line (return a tuple with
 * value in reverse order tuple index 4->0 instead of 0->4)
 * Return a vector of values corresponding at simulation time.
 */
template <typename... Args> // let compiler deduce return type
auto extractLineValuesTmplt(QTextStream &afileStream, QString &&aFirstLine) {
  QString w_line2Read{std::move(aFirstLine)};
  QVector<std::tuple<Args...>> w_vecoftpl;
  w_vecoftpl.reserve(101 /*EMCNEILCTE::value*/); // hard coded!!!
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

/** @brief Reverse tuple index order (problem when parsing
 *  each line and return a tuple)
 *  @return
 */
template <class Tuple, std::size_t... Is>
Tuple tuple_inverse_idx_order(const Tuple &t, std::index_sequence<Is...>) {
  return std::make_tuple(std::get<Is>(t)...);
}
} // namespace qplot
