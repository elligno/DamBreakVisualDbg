#pragma once

// C++ include
#include <memory> // smart pointer
// library include
#include "dbpp_scalarField.h"

namespace dbpp {

/** Model of a two components state vector over a field.
 */
class vectorField {
  /**
   * @brief Alias for compatibility with original version
   *
   */

  using shrdgrid = std::shared_ptr<gridLattice>;

public:
  vectorField() = default;
  // shall we use a default ctor? does it make sense?
  vectorField(std::string aName, const std::shared_ptr<gridLattice> &aGrid);
  //     vectorField( std::string aName, const Dsn::scalarField& aField1,
  //                  const Dsn::scalarField& aField2);

  // Design Note for backward compatibility still use old version of scalar
  // field
  vectorField(const scalarField &aStVec1, const scalarField &aStVec2)
      : m_A{aStVec1}, m_Q{aStVec2} {}

  // All ctor (copy, assignable, move and assignable move) are automatically
  // generated no need to declare them as defaulted, by doing this you may
  // disable move semantic shall we have a copy ctor? or default will do
  //     vectorField( const vectorField&) = default;
  //     vectorField& operator= ( const vectorField&) = default;

  const std::string &name() const { return m_name; }

  // ... to be com
  scalarField A() const { return m_A; } // return a copy
  scalarField Q() const { return m_Q; }
  const RealNumArray<double> &Avalues() const { return m_A.values(); }
  const RealNumArray<double> &Qvalues() const { return m_Q.values(); }

  //
  // Math operations on scalar field (+=,-=,*= ...)
  //

  // return field values
  // 		      jb::RealNumArray<real>& U1values()       { return
  // m_U1->values(); } 		      jb::RealNumArray<real>& U2values()       {
  // return m_U2->values(); } 		const jb::RealNumArray<real>& U1values()
  // const { return m_U1->values(); } 		const jb::RealNumArray<real>&
  // U2values() const { return m_U2->values(); }

  // return vector component (shall i return it as a reference?)
  // 		std::shared_ptr<jb::scalarField> getU1() const { return m_U1; }
  // 		std::shared_ptr<jb::scalarField> getU2() const { return m_U2; }
  // std::pair<fieldPtr,fieldPtr> asStateVector() const { return
  // std::make_pair(m_U1,m_U2); }

private:
  std::string m_name; /**< field name*/
  // why we use smart pointer? any reason? raw pointer will do?
  //		std::shared_ptr<jb::scalarField> m_U1;  /**< smart pointer ...*/
  //		std::shared_ptr<jb::scalarField> m_U2;  /**< smart pointer ...*/

  // just a plain scalar field?? if we really need smart pointer for some
  // reason, user can use it at his convenience, otherwise just keep it
  // simple!!!
  scalarField m_A; /**< scalar field of the first component*/
  scalarField m_Q; /**< scalar field of the second component*/
};
} // namespace dbpp
