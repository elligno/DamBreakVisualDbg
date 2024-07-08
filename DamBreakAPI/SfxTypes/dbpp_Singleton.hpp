
#pragma once

// C++ includes
#include <cstddef>
#include <iostream>
// boost include
#include <boost/noncopyable.hpp>

namespace dbpp {
/** Brief CRTP James Coplien (Curious Recursive Template Pattern).
 *   This is a model of CRTP to define Global instance of the application
 *   Specialize a base class using the derived class as a template argument.
 */
template <class T>
class Singleton //: boost::noncopyable private copy and
{               // assignment ctor
public:
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;
  /**
   *  @return sole instance
   */
  static T *instance();
  /**
   *  destroy the sole instance
   */
  void release();

protected:
  /**
   *  Want to prevent of using them from client side
   */
  Singleton();

  // Shouldn't the destructor be virtual?
  virtual ~Singleton();

private:
  // The sole instance of the class
  static T *m_singleton; /**< sole instance */
};

// initialize static variable
template <class T> T *Singleton<T>::m_singleton = nullptr;

template <class T> Singleton<T>::Singleton() {}

template <class T> Singleton<T>::~Singleton() {
  // debugging purpose
  std::cout << "We are in the Singleton dtor\n";
}

template <class T> T *Singleton<T>::instance() {
  if (m_singleton == nullptr) {
    m_singleton = new T;
  }

  return m_singleton;
}

template <class T> void Singleton<T>::release() {
  if (m_singleton == nullptr)
    return;

  delete m_singleton;
  m_singleton = nullptr;
}
} // namespace dbpp
