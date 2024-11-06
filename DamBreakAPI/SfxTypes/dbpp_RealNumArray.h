#pragma once

// c++  includes
#include <iostream>
// stl include
#include <vector>
// Library include
#include "dbpp_DefineTypes.h"

namespace dbpp {
/**
 * @brief Class RealNumArray is a templated 1D or 2D array,
 *  specialized for containing numerical data.
 *
 *  Provides fast-floating point operation and indexing
 *  mechanism to avoid deferencing (faster).
 *
 *  Numerical array is indexed from 1,..,N, pointer offset.
 *
 *  Numerical library provides other that support some very
 *  fast floating operation such as template expression and
 *  SAXPY operations.
 *
 *  Array provides a STL interface to make compatibility
 *  with STL algorithm. Boost Range library can be used
 *  to make it more close of STL standard.
 *
 * @tparam T type
 */
template <typename T> class RealNumArray {
public:
  /** Macro that declare STL-like container*/
  SFX_DECLARE_STL_CONTAINER_TYPES(T)

  // some alias
  //     using value_type = T;
  //     using pointer = value_type*;
  //     using reference = value_type&;
  //     using iterator = pointer;
  //     using const_iterator = const pointer;

  /**
   * @brief Services to position at he beginning/end of the collection
   *
   * @return iterator
   */
  iterator begin() { return A; }
  iterator end() { return A + size(); }
  const_iterator cbegin() const { return A; }
  const_iterator cend() const { return A + size(); }
  iterator rbegin() { return std::reverse_iterator<iterator>(end()); }
  iterator rend() { return std::reverse_iterator<iterator>(begin()); }
  const_iterator crbegin() const {
    return std::reverse_iterator<const_iterator>(cend());
  }
  const_iterator crend() const {
    return std::reverse_iterator<const_iterator>(begin());
  }

private:
  T *A;           /**< the data */
  int dimensions; /**< number of dimensions (max is 2) */
  int length[2];  /**< lengths of each dimension */

  // needed for optimizations
  int length0;   /**< */
  int length0p1; /**< */

  /**
   * @brief these three functions are used by constructors, destructor and
   * redim:
   *
   * @param length1
   * @return T*
   */
  [[nodiscard]] T *allocate(int length1);
  [[nodiscard]] T *allocate(int length1, int length2);
  void deallocate();

  /**
   * @brief transform fake indice to real
   *
   * @param i row index
   * @param j column index
   * @return int index of
   */
  int transform_index(int i, int j) const;

  // Design Note
  // this is shall be done when creating the array in allocate
  // new T[N] {} call default ctor for each wich set to zero
  // this method is deprecated, make no sense, always initialize
  void initArray1D() // initialize array
  {
    // initial value (instead of garbage)
    for (int i = 0; i < length[0]; ++i) {
      A[i] = 0.;
    }
  }

public:
  // deprecated
  // RealNumArray();  don't need it, just use the ctor below with default value

  /**
   * @brief Construct a new Real Num Array object
   *
   * @param n1
   */
  explicit RealNumArray(int n1 = 5); // constructor for 1D array
  /**
   * @brief Construct a new Real Num Array object (initialize with initial
   * values)
   *
   * @param n1 number of element
   * @param aValues data to fill array
   */
  RealNumArray(int n1, T *aValues); // initialize with initial values
  /**
   * @brief Construct a new Real Num Array object
   *
   * @param n1
   * @param n2
   */
  RealNumArray(int n1, int n2); // constructor for 2D array
  /**
   * @brief Construct a new Real Num Array object
   *
   * @param array
   */
  RealNumArray(const RealNumArray<T> &array); // copy constructor
  /**
   * @brief
   *
   * @param v
   * @return RealNumArray&
   */
  RealNumArray &operator=(const RealNumArray<T> &v); // assignment operator

  /**
   * @brief Destroy the Real Num Array object
   *
   */
  ~RealNumArray();
  /**
   * @brief redim 1D array
   *
   * @param n1 new dimension
   */
  void redim(int n1);
  /**
   * @brief redim 2D array
   *
   * @param n1 new dimension (first)
   * @param n2 new dimension (second)
   */
  void redim(int n1, int n2);

  // return the size of the arrays dimensions
  /**
   * @brief length of 1D array
   *
   * @return number of elements
   */
  int size() const;
  /**
   * @brief length 2D array
   *
   * @param n1 dimension
   * @return  number of elements
   *
   */
  int size(int dimension) const;
  /**
   * @brief check if index is ok to use
   *
   * @param n1 new dimension (first)
   * @return true/false
   */
  bool indexOk(int i) const;
  /**
   * @brief check if indices are ok to use
   *
   * @param n1 new dimension (first)
   * @param n2 new dimension (second)
   * @return true/false
   */
  bool indexOk(int i, int j) const;
  /**
   * @brief operator() for 1D array
   *
   * @param n1
   * @return
   */
  const T &operator()(int i) const;
  T &operator()(int i);
  /**
   * @brief operator() for 2D array
   *
   * @param n1
   * @param n2
   */
  const T &operator()(int i, int j) const;
  T &operator()(int i, int j);

  // returns pointers to the data
  const T *getPtr() const;
  T *getPtr();

  // return value optimization
  std::vector<double> to_stdVector() const {
    return std::vector<double>(A, A + size());
  }

  // check if size is zero
  [[nodiscard]] bool isEmpty() const noexcept { return A == nullptr; }

  void print(std::ostream &os);

  // print one-dimensional array
  void print1D(std::ostream &os) {
    for (int i = 1; i < size(); ++i)
      os << (*this)(i) << " ";
    os << "\n";
  }
};

template <typename T> dbpp::RealNumArray<T>::RealNumArray(int n1, T *aValues) {
  // memory allocation
  A = allocate(n1);
  for (int i = 0; i < length[0]; ++i) {
    A[i] = *aValues++; // set array values
  }
}

template <typename T> RealNumArray<T>::RealNumArray(int n1 /*=5*/) {
  A = allocate(n1);
  // initArray1D(); useless since we initialize the array to zero in allocate
}

template <typename T> RealNumArray<T>::RealNumArray(int n1, int n2) {
  A = allocate(n1, n2);
}

template <typename T>
RealNumArray<T>::RealNumArray(const RealNumArray<T> &aOther) {
  if (aOther.dimensions == 2) {
    A = allocate(aOther.length[0], aOther.length[1]);

    for (int i = 0; i != (length[0] * length[1]); ++i)
      A[i] = aOther.A[i];
  } else if (aOther.dimensions == 1) {
    A = allocate(aOther.length[0]);

    for (int i = 0; i != length[0]; ++i)
      A[i] = aOther.A[i];
  } else {
    std::cerr << "MyArray::MyArray(const MyArray<T>&) -- "
              << "illegal dimension " << aOther.dimensions << std::endl;
    exit(1);
  }
}

template <typename T>
RealNumArray<T> &RealNumArray<T>::operator=(const RealNumArray<T> &aOther) {
  if (aOther.dimensions == 2) {
    // delete first
    delete[] A;
    A = allocate(aOther.length[0], aOther.length[1]);
    for (int i = 0; i != (length[0] * length[1]); ++i)
      A[i] = aOther.A[i];
  } else if (aOther.dimensions == 1) {
    delete[] A;
    A = allocate(aOther.length[0]);
    for (int i = 0; i != length[0]; ++i)
      A[i] = aOther.A[i];
  } else {
    // do something a log message
  }
  return *this;
}

template <typename T> RealNumArray<T>::~RealNumArray() { deallocate(); }

template <typename T> void RealNumArray<T>::redim(int n1) {
  if (length[0] == n1)
    return;

  // when resizing and copying the original value back
  auto w_Length{0};
  // Create a temp buffer for storing intermediate data
  double *w_Temp = new double[n1];
  if (n1 < length[0])
    w_Length = n1;
  else
    w_Length = length[0];

  // Preserve data in a temporary array before resizing
  for (auto i{0}; i < w_Length; i++)
    w_Temp[i] = A[i];

  deallocate();
  A = allocate(n1);

  // Restore data in the original new sized array and delete temp buffer
  for (auto j{0}; j < w_Length; j++)
    A[j] = w_Temp[j];

  delete[] w_Temp;
  // 		}
  // 		else
  // 		{
  // 			std::cout << "Negative Array size.\n";
  // 			return;
  // 		}

  // Design Note: it should copy the element
  // of the array in a tmp array and then
  // resize the array and finally copy back
  // the original elements
  // I have an array in some of my old project
  // where it save the original element
  // 		deallocate();
  // 		A = allocate(n1);
}

template <typename T> void RealNumArray<T>::redim(int n1, int n2) {
  if (length[0] == n1 && length[1] == n2)
    return;

  deallocate();
  A = allocate(n1, n2);
}

template <typename T> int RealNumArray<T>::size() const { return length[0]; }

template <typename T> int RealNumArray<T>::size(int dimension) const {
  return (dimension == 1) ? length[0] : length[1];
}

template <typename T> T *RealNumArray<T>::allocate(int n1) {
  T *ptr = nullptr;
  length[0] = n1;
  length[1] = 0;
  dimensions = 1;

  if (n1 < 0) {
    std::cerr << "MyArray::allocate -- illegal length " << n1 << std::endl;

    exit(1);
  } else if (n1 == 0) {
    ptr = nullptr;
    return ptr;
  }

  try {
    ptr = new T[n1]{}; // init to zero values
  } catch (std::bad_alloc &ba) {
    std::cerr << "RealNumArray::allocate -- unable to allocate array of length "
              << n1 << ba.what() << std::endl;
    exit(1); // shall throw an exception, not sure but not sure also to call
             // exit, hard to debug
  }

  return ptr;
}

template <typename T> T *RealNumArray<T>::allocate(int n1, int n2) {
  T *ptr = 0;
  length[0] = n1;
  length[1] = n2;
  length0 = n1;       // special variable for efficient indexing
  length0p1 = n1 + 1; // special variable for efficient indexing
  dimensions = 2;

  if (n1 < 0 || n2 < 0) {
    std::cerr << "MyArray::allocate -- illegal length " << n1 << ", " << n2
              << std::endl;

    return 0;
  }

  if (n1 == 0) {
    ptr = 0;
    return ptr;
  }

  try {
    ptr = new T[n1 * n2];
  } catch (std::bad_alloc &) {
    std::cerr << "MyArray::allocate -- unable to allocate array of length "
              << length[0] << ", " << length[1] << std::endl;
    exit(1);
  }

  return ptr;
}

template <typename T> void RealNumArray<T>::deallocate() {
  delete[] A;
  A = nullptr;

  length[0] = 0;
  length[1] = 0;
}

template <typename T> T *RealNumArray<T>::getPtr() { return A; }

template <typename T> const T *RealNumArray<T>::getPtr() const { return A; }

template <typename T> bool RealNumArray<T>::indexOk(int i) const {
  if (A == 0) {
    std::cerr << "vector index check; current array is of length 0,"
              << " call redim!" << std::endl;

    return false;
  } else if (i < 1 || i > length[0]) {
    std::cerr << "vector index check; index " << i
              << "out of bounds in array (1:" << length[0] << ")" << std::endl;

    return false;
  } else
    return true; // valid index!
}

template <typename T> bool RealNumArray<T>::indexOk(int i, int j) const {
  if (A == 0) {
    std::cerr << "vector index check; current array is of length 0,"
              << " call redim!" << std::endl;

    return false;
  } else if (i < 1 || i > length[0] || j < 1 || j > length[1]) {
    std::cerr << "vector index check; index " << i << "," << j
              << "out of bounds in array (" << 1 << ":" << 1 + length[0] << ")("
              << 1 << ":" << 1 + length[1] << std::endl;

    return false;
  } else
    return true; // valid index!
}

template <typename T> void RealNumArray<T>::print(std::ostream &os) {
  for (int i = 1; i != size(1); ++i) {
    for (int j = 1; j != size(2); ++j)
      os << (*this)(i, j) << " ";
    os << "\n";
  }
}

template <typename T>
std::ostream &operator<<(std::ostream &os, RealNumArray<T> &arr) {
  arr.print(os);
  return os;
}

template <typename T>
inline int RealNumArray<T>::transform_index(int i, int j) const {
  return j + i * length0 - length0p1;
}

template <typename T> inline T &RealNumArray<T>::operator()(int i) {
#ifdef SAFETY_CHECKS
  indexOk(i);
#endif

  return A[i - 1];
}

template <typename T> inline const T &RealNumArray<T>::operator()(int i) const {
#ifdef SAFETY_CHECKS
  indexOk(i);
#endif

  return A[i - 1];
}

template <typename T> inline T &RealNumArray<T>::operator()(int i, int j) {
#ifdef SAFETY_CHECKS
  indexOk(i, j);
#endif

#ifdef NO_NESTED_INLINES
  return A[j + i * length0 - length0p1];
#else
  return A[transform_index(i, j)];
#endif
}

template <typename T>
inline const T &RealNumArray<T>::operator()(int i, int j) const {
#ifdef SAFETY_CHECKS
  indexOk(i, j);
#endif

#ifdef NO_NESTED_INLINES
  return A[j + i * length0 - length0p1];
#else
  return A[transform_index(i, j)];
#endif
}
} // namespace dbpp

// since this is a template class, we must make the body of the
// functions available in this header file:
//#include "dbpp_RealNumArray.cpp"
