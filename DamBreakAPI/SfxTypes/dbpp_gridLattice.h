#pragma once

// C++ includes
#include <iostream>
#include <string>
// boost include
//#include <boost/cast.hpp>
#include <boost/cast.hpp>
// library include
#include "dbpp_RealNumArray.h"

namespace dbpp {
/** @brief class GridLattice describes the domain the PDE
 *  will be applied to.
 */
class gridLattice {
private:
  // currently limited to two dimensions
  static constexpr int MAX_DIMENSIONS = 2;

  // deprecated!!!
  // variables defining the size of the grid
  double min[MAX_DIMENSIONS];   /**< min coordinate values in each dimension */
  double max[MAX_DIMENSIONS];   /**< max coordinate values in each dimension */
  int division[MAX_DIMENSIONS]; /**< number of points in each dimension */

  int dimensions; /**< number of dimensions */

  // new version pair.first: dimension 1, pair.second: dimension 2
  std::pair<double, double>
      m_min; /**< min coordinate values in each dimension */
  std::pair<double, double>
      m_max; /**< max coordinate values in each dimension */
  std::pair<int, int> m_division; /**< number of points in each dimension */

  std::string
      m_distrRep; /**< string representation of the discretization parameters */
  // must be private, access denied to client
  // scan parameters from string
  void scan(const std::string &init_string);

public:
  // default ctor (be careful because what we do here we initialize
  // a 2-dimensional grid and provide a method scan that can overwrite
  // the initial parameter).
  // First, scan() shouldn't be public, it's a private method
  // Second,
  gridLattice();
  gridLattice(const gridLattice &) = default;
  gridLattice &operator=(const gridLattice &) = default;

  // initialize the grid with the following syntax:
  // d=2 [0,1]x[0,10] [1:30]x[1:300]
  gridLattice(const std::string &aInitStr);

  // may not need anymore of the ctor above
  gridLattice(std::string &&aStrRep) {
    m_distrRep = std::move(aStrRep);
    scan(m_distrRep);
  }

  // ctor from parameters
  gridLattice(int nx, int ny, double xmin_, double xmax_, double ymin_,
              double ymax_);

  // default copy and assignment ctor will be enough???

  int getNoSpaceDim() const;

  double xMin(int dimension) const;
  double xMax(int dimension) const;

  // get the number of points in each dimension
  int getDivisions(int i) const;

  // get the total number of points in the grid.
  int getNoPoints() const;

  double Delta(int dimension) const;
  double getPoint(int dimension, int index);

  int getBase(int dimension) const; // get base values
  int getMaxI(int dimension) const; // upper limit of array

  // access to private member
  friend std::ostream &operator<<(std::ostream &, gridLattice &);
};

inline double gridLattice::Delta(int dimension) const {
  using boost::numeric_cast;
  using boost::numeric::bad_numeric_cast;
  using boost::numeric::negative_overflow;
  using boost::numeric::positive_overflow;

  // return (max[dimension-1] - min[dimension-1])/double(division[dimension-1]);

  // return first dimension parameter as default
  try {
    // try numerical cast and catch
    if (dimension < 1 || dimension > MAX_DIMENSIONS) {
      // default value ...
      return (m_max.first - m_min.first) /
             numeric_cast<double>(m_division.first);
    }
    // ...
    if (dimension == 1) {
      return (m_max.first - m_min.first) /
             numeric_cast<double>(m_division.first);
    } else {
      return (m_max.second - m_min.second) /
             numeric_cast<double>(m_division.second);
    }
  } catch (bad_numeric_cast &bc) {
    std::cerr << bc.what() << "\n";
    return 0.;
  }
}

inline double gridLattice::getPoint(int dimension, int index) {
  // return min[dimension-1] + (Delta(dimension) * (index - 1));
  if (dimension == 1) {
    return m_min.first + (Delta(dimension) * (index - 1));
  } else {
    return m_min.second + (Delta(dimension) * (index - 1));
  }
}

inline int gridLattice::getNoSpaceDim() const { return dimensions; }

inline int gridLattice::getDivisions(int i) const {
  // 		return division[i-1];
  if (i < 1 || i > MAX_DIMENSIONS) // shall throw an exception outside of range
  {
    return m_division.first;
  }
  // otherwise return corresponding number of points
  if (i == 1) // first dimension
  {
    return m_division.first;
  } else {
    return m_division.second;
  }
}

inline int gridLattice::getBase(int i) const {
  if (i == dimensions)
    return 1; // in this version it shall always return 1
  else
    return 0; // don't make sense (cold be -1 for valarray support)
}

inline int gridLattice::getMaxI(int dimension) const {
  // return division[dimension-1];
  if (dimension == 1) {
    return m_division.first;
  } else {
    return m_division.second;
  }
}
} // namespace dbpp
