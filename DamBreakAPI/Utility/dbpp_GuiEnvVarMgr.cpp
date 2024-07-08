
#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#include "dbpp_GuiEnvVarMgr.h"
#include <cassert>

namespace dbpp {
void EnvVariablesManager::put(const std::string &name,
                              const std::string &value) {
  const VariableContainer pair = PairToContainer(name, value);
  const std::pair<Variables::iterator, bool> inserted =
      m_vars.insert(std::make_pair(name, pair));
  if (!inserted.second)
    inserted.first->second = pair;
  char *data = &(inserted.first->second[0]);
  PutOSVariable(data);
}

std::string EnvVariablesManager::get(const std::string &name) const {
  return GetOSVariable(name.c_str());
}

void EnvVariablesManager::del(const std::string &name) { put(name, ""); }

void EnvVariablesManager::PutOSVariable(char *value) { ::_putenv(value); }

std::string EnvVariablesManager::GetOSVariable(const char *name) {
#ifdef WIN32
  size_t sz = 0;
  assert(getenv_s(&sz, NULL, 0, name) == 0);
  if (sz == 0)
    return std::string();
  std::vector<char> value(sz + 1);
  assert(getenv_s(&sz, &value[0], sz, name) == 0);
  return std::string(&value[0], sz - 1);
#else
  const char *const value = std::getenv(name);
  return value ? value : "";
#endif
}

bool EnvVariablesManager::IsOSVariableSet(const char *name) {
#ifdef WIN32
  size_t sz = 0;
  assert(getenv_s(&sz, NULL, 0, name) == 0);
  return sz > 0;
#else
  const char *value = std::getenv(name);
  return value != NULL && *value != '\0';
#endif
}

EnvVariablesManager::VariableContainer
EnvVariablesManager::PairToContainer(const std::string &name,
                                     const std::string &value) const {
  VariableContainer pair;
  std::copy(name.begin(), name.end(), std::back_inserter(pair));
  pair.push_back('=');
  std::copy(value.begin(), value.end(), std::back_inserter(pair));
  pair.push_back('\0');
  return pair;
}
} // namespace dbpp
