#pragma once

// C++ includes
#include <map>
#include <vector>
// Qt include
#include <QVector>

namespace qplot {

/**@brief Mapping with full result format columns
 * In the process extracting data from file, these
 * are stored (each line of data) in a tuple, ...
 * to be completed
 * Format is
 *  X  H, A, Q, V, Fr these variables defined as
 *  x-coord, water depth or level?? (check),
 *   wetted areea, discharge, velocity and Froude number
 */
using dbltpl = std::tuple<double, double, double, double, double>;
/**@brief Mapping with full result format columns
 */
using pairtimeline =
    std::pair<double /*Time*/, QVector<dbltpl> /*line of values*/>;

/**@brief Mapping with full result format columns (alias template)
 */
template <typename... prms> using qvectpl = QVector<std::tuple<prms...>>;

// data also could be profile format {x,val}
/**@brief store data {U1p,U2P} as a pair, read each line and store
 */
using vecpair = // could be qvectpl<double,double>;
    QVector<std::pair<double, double>>;

// useful typedef (actually we don't really need it, just the "auto" keyword)
using mapValU12 =
    std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;

using mapIterUValue = std::map<unsigned, vecpair>;

// using mapIterUValue = std::map<unsigned, std::vector<std::tuple<double,
// double>>>;
using pairxVar = std::pair<QVector<double> /*x-coord*/,
                           QVector<double> /*var value to cmp*/>;
// using twographcmp = std::tuple
} // namespace qplot
