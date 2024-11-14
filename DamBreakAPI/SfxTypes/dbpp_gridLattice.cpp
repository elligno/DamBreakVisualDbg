// file        : GridLattice.cpp
// description : implementation of GridLattice

// c++ includes
#include <cstdio>
#include <cstdlib>
#include <strstream>
// library include
#include "dbpp_gridLattice.h"

using namespace std;

namespace dbpp {
// default ctor
// in this new implementation we use the pair utility and avoid
// using index and array which are prone to error. Also we avoid
// doing a loop for only 2 element which is expensive and prone to error.
gridLattice::gridLattice()
    : m_min(std::make_pair(0., 0.)), // x-y pair
      m_max(std::make_pair(1., 1.)), // x-y pair
      dimensions(2), m_division(std::make_pair(2, 2)) {
  //		dimensions = 2;
  // 		int i;
  // 		for (i = 1; i <= MAX_DIMENSIONS; ++i)
  // 		{
  // 			min[i] = 0;  max[i] = 1;  division[i] = 2;
  // 			// debugging purpose
  // 			std::cout << "i :" << i << " min[]: " << min[i] << " max[]: "
  // << max[i] << " division: " << division[i] << "\n";
  // 		}
}

gridLattice::gridLattice(int nx, int ny, double xmin, double xmax, double ymin,
                         double ymax)
    : m_min(std::make_pair(xmin, ymin)),                // x-y pair
      m_max(std::make_pair(xmax, ymax)),                // x-y pair
      dimensions(2), m_division(std::make_pair(nx, ny)) //
{
  // 		dimensions = 2;
  // 		max[0] = xmax;     max[1] = ymax;
  // 		min[0] = xmin;     min[1] = ymin;
  // 		division[0] = nx;  division[1] = ny;

  // need to check nx and ny that must greater than 0, otherwise
  // it doesn't make sense. A grid cannot have 0 grid points
  if (nx == 0 && ny == 0) {
    // set default value (we need at least 2 points)
    nx = 2;
    ny = 2;
    // reset division number
    m_division = std::make_pair(nx, ny);

    // we need to check for the x-y extent
  } else if (nx == 0 && ny != 0) {
    nx = 2;
    // reset division number
    m_division = std::make_pair(nx, ny);
  } else if (ny == 0 && nx != 0) {
    ny = 1; // originally set to 2, but for our test
    // reset division number
    m_division = std::make_pair(nx, ny);
  }
}
// ctor from string construction (verbose)
gridLattice::gridLattice(const std::string &aInitStr)
    : m_min{}, m_max{}, m_division{} {
  // now read parameters from string
  scan(aInitStr);

  // any test we could do to check
}

double gridLattice::xMin(int dimension) const {
  //	return min[dimension - 1];
  if (dimensions < 1 || dimensions > MAX_DIMENSIONS) {
    cerr << "GridLattice::xmin() -- illegal dimensions " << dimensions << endl;
    cerr << "                       MAX_DIMENSIONS is set to " << MAX_DIMENSIONS
         << endl;
    exit(1); // shall throw an exception
  }
  // otherwise return corresponding value
  if (dimension == 1) {
    return m_min.first;
  } else {
    return m_min.second;
  }
}

double gridLattice::xMax(int dimension) const {
  //	return max[dimension - 1];
  if (dimensions < 1 || dimensions > MAX_DIMENSIONS) {
    cerr << "GridLattice::xmax() -- illegal dimensions " << dimensions << endl;
    cerr << "                       MAX_DIMENSIONS is set to " << MAX_DIMENSIONS
         << endl;
    exit(1); // shall throw an exception
  }
  // otherwise return corresponding value
  if (dimension == 1) {
    return m_max.first;
  } else {
    return m_max.second;
  }
}

// something wrong with this
int gridLattice::getNoPoints() const {
  // initial value
  int return_value = 1;
  // 		for (int i = 0; i != dimensions; ++i) // why from 0?? don't get
  // it!!! 			return_value *= division[i];

  for (int i = 0; i != dimensions; ++i) {
    if (i == 0) {
      return_value *= m_division.first;
      continue;
      ;
    }
    return_value *= m_division.second;
  }

  return return_value;
}

// Read grid with syntax
//   d=2 [0,1]x[0,10] [1:30]x[1:300]
void gridLattice::scan(const string &init_string) {
  using namespace std; // allows dropping std:: prefix

  // istrstream is used, although it is deprecated in ISO/IEC
  // 14882:1998(E). however, G++ does not yet support istringstream,
  // so istrstream is used instead.
  istrstream is(init_string.c_str());

  // ignore "d="
  is.ignore(1, 'd');
  is.ignore(1, '=');

  // get the dimensions
  is >> dimensions;
  if (dimensions < 1 || dimensions > MAX_DIMENSIONS) {
    cerr << "GridLattice::scan() -- illegal dimensions " << dimensions << endl;
    cerr << "                       MAX_DIMENSIONS is set to " << MAX_DIMENSIONS
         << endl;
    exit(1);
  }

  // for each dimension, read the min and max values:
  for (int i = 1; i <= dimensions; i++) {
    is.ignore(1, ' ');
    is.ignore(1, '[');

    double r;
    is >> r;
    min[i - 1] = r;
    is.ignore(1, ',');

    double s;
    is >> s;
    max[i - 1] = s;
    is.ignore(1, ']');
  }

  // the new version parameters
  m_min.first = min[0];
  m_max.first = max[0];
  if (dimensions == 2) {
    m_min.second = min[1];
    m_max.second = max[1];
  }

  // for each dimension, read the base and top index values.
  for (int i = 1; i <= dimensions; i++) {
    is.ignore(1, ' ');
    is.ignore(1, '[');

    int base;
    is >> base;
    if (base != 1) {
      std::cout << "base=" << base << " not allowed yet (only 1 works)\n";
      exit(1);
    }
    is.ignore(1, ':');
    int top;
    is >> top;
    division[i - 1] = top - base + 1;
    is.ignore(1, ']');
  }

  // set new version parameters
  m_division.first = division[0];
  if (dimensions == 2) {
    m_division.second = division[1];
  }
}

std::ostream &operator<<(std::ostream &os, gridLattice &g) {
  // write out the grid in the same syntax as scan reads it:
  os << "d=" << g.dimensions << "   ";
  int i;
  for (i = 1; i <= g.dimensions; ++i) {
    os << '[' << g.xMin(i) << ',' << g.xMax(i) << ']';
    if (i < g.dimensions)
      os << 'x';
  }
  os << "   ";
  for (i = 1; i <= g.dimensions; ++i) {
    os << "[1:" << g.getDivisions(i) << ']';
    if (i < g.dimensions)
      os << 'x';
  }
  os << endl;

  return os;
}
} // namespace dbpp
