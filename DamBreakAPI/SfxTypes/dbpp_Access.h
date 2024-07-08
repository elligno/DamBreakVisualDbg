
#pragma once

// C++ includes
#include <list>
#include <string>
// physics env includes
#include "dbpp_IAccess.h"

namespace dbpp {
/** Moveable only (mechanism to retrieve an interface of Physics Object Model
 * (POM) )
 */
class Access : public IAccess // inherit from the top abstract class
{
public:
  Access() = default;
  // no default ctor defined yet
  Access(std::string alistOfInterface);
  Access(const Access &aOther) = delete;
  Access &operator=(const Access &aOther) = delete;
  ~Access() = default;
  Access(Access &&aOther) {
    m_listofinterfaces =
        std::move(aOther.m_listofinterfaces); // is it ok? as simple as that!!
    m_strRep = std::move(aOther.m_strRep);
    m_intVal = aOther.m_intVal;
  }
  Access &operator=(Access &&aOther) {
    if (&aOther != this) {
      m_listofinterfaces =
          std::move(aOther.m_listofinterfaces); // is it ok? as simple as that!!
      m_strRep = std::move(aOther.m_strRep);
      m_intVal = aOther.m_intVal;
    }
    return *this;
  }

  // return pointer to interface
  virtual void *getInterface(const unsigned aGuid) override;
  virtual void *getInterface(const std::string &aStr2Interface) override = 0;

protected:
  // want access right from derived class
  std::list<std::string> m_listofinterfaces;

  // check if this type support interface
  bool hasInterface() const {
    return (std::distance(m_listofinterfaces.cbegin(),
                          m_listofinterfaces.cend()) != 0);
  }

private:
  std::string m_strRep;
  int m_intVal;
};
} // namespace dbpp
