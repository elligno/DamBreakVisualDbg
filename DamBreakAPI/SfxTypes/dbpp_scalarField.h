#pragma once

// C++ includes
#include <memory> // shared pointer
#include <string>
// library includes
#include "dbpp_RealNumArray.h"
#include "dbpp_gridLattice.h"

// i am not sure yet what for
typedef double real;

// shall use another namespace (when migrating to VS19 with C++17 support)
// SfxBaseTypes library
namespace dbpp {
/** Class scalarField is an abstraction of the numerical
 *  concept of a field, a scalar field over a grid.
 */
class scalarField {
  using float64 = double;

private:
  std::shared_ptr<gridLattice> grid_lattice;               /**< */
  std::shared_ptr<RealNumArray<double>> grid_point_values; /**< */
  std::string fieldname;                                   /**< */

public:
  // need it from vector field class
  scalarField() = default;

  // copy ctor
  scalarField(const scalarField &aOther) = default;

  // assignment ctor ?? default will do i guess.
  scalarField &operator=(const scalarField &aOther) = default;

  // destructor
  ~scalarField() = default;

  // make a field from a grid and a fieldname
  scalarField(std::shared_ptr<gridLattice> &aGrid,
              const std::string &aFieldName);

  // enable access to grid-point values
  RealNumArray<double> &values() { return *grid_point_values; }
  const RealNumArray<double> &values() const { return *grid_point_values; }

  // enable access to the grid
  gridLattice &grid() { return *grid_lattice; }
  const gridLattice &grid() const { return *grid_lattice; }

  std::string name() const { return fieldname; }

  // set the values of the field
  void values(RealNumArray<double> &new_array);
  void values(RealNumArray<double> &&new_array); // move semantic??

  // printing the field values
  void print(std::ostream &aValues2Print);

  // math operations on field
  scalarField &operator+=(const scalarField &aOther);
  scalarField &operator-=(const scalarField &aOther);
  scalarField &operator*=(const float64 aVal);
};
} // namespace dbpp
