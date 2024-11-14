// C++ includes
#include <algorithm>
#include <functional>
#include <iterator>
// Library includes
#include "dbpp_BaseMacros.h"     //  Shall include the include below to avoid
#include "dbpp_ExceptionUtils.h" // Macro dependency
#include "dbpp_scalarField.h"

namespace dbpp {
scalarField::scalarField(const std::shared_ptr<gridLattice> &g,
                         const std::string &name_) {
  grid_lattice = g;
  // allocate the grid_point_values array:
  if (grid_lattice->getNoSpaceDim() == 1) {
    // grid_point_values.rebind( new
    // RealNumArray<real>(grid_lattice->getDivisions(1)));
    grid_point_values.reset(
        new RealNumArray<double>(grid_lattice->getDivisions(1)));
  } else if (grid_lattice->getNoSpaceDim() == 2) {
    // 			grid_point_values.rebind( new RealNumArray<real>(
    // 			grid_lattice->getDivisions(1),grid_lattice->getDivisions(2)));
    grid_point_values.reset(new RealNumArray<double>(
        grid_lattice->getDivisions(1), grid_lattice->getDivisions(2)));
  } else
    ; // three-dimensional fields are not yet supported...
  fieldname = name_;
}

// NOTE this is deprecated and will be removedd in next version
// by a scalar field based on std valarray (keep it for now)
scalarField::scalarField(const std::shared_ptr<gridLattice> &g,
                         std::vector<double> aInitialValues,
                         const std::string &name_) {
  grid_lattice = g;
  // allocate the grid_point_values array:
  // NOTE
  if (grid_lattice->getNoSpaceDim() == 1) {
    grid_point_values.reset(new RealNumArray<double>(
        grid_lattice->getDivisions(1), aInitialValues.data()));
    fieldname = name_;
  } else if (grid_lattice->getNoSpaceDim() == 2 &&
             (grid_lattice->xMin(2) == grid_lattice->xMax(2))) {
    grid_point_values.reset(new RealNumArray<double>(
        grid_lattice->getDivisions(1), aInitialValues.data()));
    fieldname = name_;
  } else {
    // Throw an exception with a string message
    throw std::runtime_error("scalarField initialization error");
  }
}

void scalarField::values(RealNumArray<double> &new_array) {
  // grid_point_values.rebind(&new_array);
  grid_point_values.reset(&new_array);
}

void scalarField::values(RealNumArray<double> &&new_array) {
  // grid_point_values.rebind(&new_array);
  // not sure about this one!!!!
  grid_point_values.reset(new RealNumArray<real>(new_array));

  std::cout << "Check move semantic\n";
}

void scalarField::print(std::ostream &aValues2Print) {
  std::cout << "Current values of the field\n";

  std::copy(values().begin(), values().end(),
            std::ostream_iterator<float64>(aValues2Print, "\n"));
}

scalarField &scalarField::operator+=(const scalarField &aOther) {
#ifdef _DEBUG
  SFX_REQUIRE(values().size() == aOther.values().size(),
              "Cannot add field with different size");
#endif
  std::transform(values().begin(), values().end(), aOther.values().cbegin(),
                 values().begin(), std::plus<float64>{});

  return *this;
}

// __________________________________________________________
//
scalarField &scalarField::operator-=(const scalarField &aOther) {
#ifdef _DEBUG
  SFX_REQUIRE(values().size() == aOther.values().size(),
              "Cannot subtract field with different size");
#endif
  std::transform(values().begin(), values().end(), aOther.values().cbegin(),
                 values().begin(), std::minus<float64>{});

  return *this;
}

// __________________________________________________________
//
scalarField &scalarField::operator*=(const float64 aVal) {
  std::for_each(
      values().begin(), values().end(),
      std::bind(std::multiplies<float64>{}, std::placeholders::_1, aVal));

  return *this;
}
} // namespace dbpp
