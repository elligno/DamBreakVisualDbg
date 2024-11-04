
#include "VectorField.h"

namespace dbpp {
vectorField::vectorField(std::string aName,
                         const std::shared_ptr<gridLattice> &aGrid)
    : m_name{std::move(aName)}, m_A{shrdgrid{aGrid}, m_name}, m_Q{shrdgrid{
                                                                      aGrid},
                                                                  m_name} {
  // not much to do
}
} // namespace dbpp
