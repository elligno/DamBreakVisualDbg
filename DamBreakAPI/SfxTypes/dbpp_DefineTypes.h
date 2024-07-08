
////////////////////////////////////////////////////////////////////////////
//
// FILE: Sfx_DefineTypes.h
//
// Copyright (C) 2004, ELLIGNO Inc. All rights reserved.
//
//	This data is unpublished proprietary information of ELLIGNO Inc., and is
//	protected by federal copyright law. It may not be disclosed to third
//	parties or copied or duplicated in any form, in whole or in part,
//without 	the prior written consent of ELLIGNO Inc.
//
//   Author: Jean Belanger (Elligno Inc.)
//   Date of creation: 16 September 2004
//   Revision history:

#pragma once

// STL includes
#include <iterator>
#include <valarray>
#include <vector>
// Sfx Package includes
//#include "Sfx_Config.h"
//#include "Sfx_ExceptionUtils.h"
//#include <cmath>

// Define symbol for DLL (Dynamic Link Library) support. These symbols are used
// in the header file of the Plug-In library
//

// #ifdef WIN32
// #define SFX_EXPORT __declspec(dllexport)
//
// #define SFX_IMPORT __declspec(dllimport)
// #endif

// _________________________________________________________________________
//
// Description:
//   Defined type used in the "Elligno Virtual Physics Studio".

//------------------------------------------------------------
//                  Important data types
//------------------------------------------------------------

typedef int int32;
//
// typedef unsigned char  byte;
//
// typedef unsigned short ushort;

typedef float float32; // floating value with 32 bits precision

typedef double float64; // floating point with 64 bits precision

typedef unsigned char uint8; // unsigned integer value with 8 bits precision

typedef unsigned short uint16; // unsigned integer value with 16 bits precision

typedef unsigned int uint32; // unsigned integer value with 16 bits precision

typedef unsigned long long
    uint64; // unsigned integer value with 64 bits precision

typedef signed char sint8; // signed integer value with 8 bits precision

typedef signed short sint16; // signed integer value with 16 bits precision

typedef signed int sint32; // signed integer value with 32 bits precision

typedef signed long long sint64; // signed integer value with 64 predision
// deprecated (will be removed in the next version)
typedef std::vector<float64>
    ArrayType; // container used of numerical computation

// Pointer-to-Function (numerical flux computation)
// typedef void (*CalculFF)(       ArrayType& aFF1,      ArrayType& aFF2,
// 								  const ArrayType& aU1, const
// ArrayType& aU2);

typedef std::pair<float64, float64>
    fluxcomp; // FF1, FF2 flux of the state variables
// typedef std::pair<float64,float64> statevec;          state variables (A,Q)
// vector of cell face state variables reconstructed using MUSCL
// typedef std::pair<std::vector<statevec>,std::vector<statevec> > vec_ulur;
// node id, node coord, A,Q,H (save mechanism)
// boost::tuple<int,float64,float64,float64,float64> physicalMeasurement;

enum efaceType { leftFace = 0, rightFace = 1 };

//------------------------------------------------------------
//                    Define Types
//------------------------------------------------------------
//
// Some compilers do not give an implementation of <limits> yet.
// For the code to be portable these macros should be used instead
// of the corresponding method of std::numeric_limits or the corresponding
// macro defined in <limits.h>.

// Defines the value of the largest representable negative integer value
#define SFX_MIN_INT ((std::numeric_limits<int32>::min)())

// Defines the value of the largest representable integer value
#define SFX_MAX_INT ((std::numeric_limits<int32>::max)())

// Defines the value of the largest representable negative floating-point value
#define SFX_MIN_REAL4 -((std::numeric_limits<float64>::max)())

// Defines the value of the smallest representable positive double value
#define SFX_MIN_POSITIVE_REAL4 ((std::numeric_limits<float64>::min)())

// Defines the value of the largest representable floating-point value
#define SFX_MAX_REAL4 ((std::numeric_limits<float64>::max)())

// Defines the machine precision for operations over doubles
#define SFX_EPSILON ((std::numeric_limits<float64>::epsilon)())

// to be completed
#define SFX_NULL_INTEGER ((std::numeric_limits<int32>::max)())

// to be completed
#define SFX_NULL_REAL ((std::numeric_limits<float32>::max)())

// mathematical types
#define SFX_SIN (std::sin)
#define SFX_COS (std::cos)
#define SFX_LOG (std::log)
#define SFX_EXP (std::exp)
#define SFX_FABS (std::fabs)
#define SFX_SQRT (std::sqrt)

// =============================================
//
//                 MACROS
//
// =============================================

// Doc to be completed
// #define SFX_ASSERT(aCondition, aDescription)                 \
// 	do {                                                      \
// 	if (!(aCondition))                                        \
// 	throw Sfx::AssertionFailedError(aDescription);            \
// 	} while(false)
//
// // Doc to be completed
// #define SFX_REQUIRE(aCondition, aDescription)                \
// 	do {                                                      \
// 	if (!(aCondition))                                        \
// 	throw Sfx::PreConditionNotSatisfiedError(aDescription);   \
// 	} while(false)
//
// // Doc to be completed
// #define SFX_ENSURE(aCondition, aDescription)                 \
// 	do {                                                      \
// 	if (!(aCondition))                                        \
// 	throw Sfx::PostConditionNotSatisfiedError(aDescription);  \
// 	} while(false)

// ========================================================
//
//                   Container Interface
//
// ========================================================

#define SFX_DECLARE_CONTAINER(type)                                            \
  typedef type value_type;                                                     \
  typedef value_type *pointer;                                                 \
  typedef const value_type *const_pointer;                                     \
  typedef value_type &reference;                                               \
  typedef const value_type &const_reference;                                   \
  typedef pointer iterator;                                                    \
  typedef const_pointer const_iterator;

#define SFX_DECLARE_STL_CONTAINER_TYPES(type)                                  \
  using value_type = type;                                                     \
  using pointer = value_type *;                                                \
  using const_pointer = const value_type *;                                    \
  using reference = value_type &;                                              \
  using const_reference = const value_type &;                                  \
  using iterator = pointer;                                                    \
  using const_iterator = const_pointer;

#if !defined(BROKEN_TEMPLATE_SPECIALIZATION)
#define SFX_DECLARE_TEMPLATE_SPECIALIZATIONS
#endif

#if !defined(BROKEN_TEMPLATE_METHOD_CALLS)
#define SFX_ALLOW_TEMPLATE_METHOD_CALLS 1
#else
#define SFX_ALLOW_TEMPLATE_METHOD_CALLS 0
#endif

#if defined(CHOKES_ON_TYPENAME)
#define SFX_TYPENAME
#else
#define SFX_TYPENAME typename
#endif

#if defined(HAVE_EXPRESSION_TEMPLATES)
#define SFX_EXPRESSION_TEMPLATES_WORK 1
#else
// Play it safe
#define SFX_EXPRESSION_TEMPLATES_WORK 0
#endif

#if defined(REQUIRES_DUMMY_RETURN)
#define SFX_DUMMY_RETURN (x) return x;
#else
#define SFX_DUMMY_RETURN (x)
#endif

//#if defined HAVE_CSTDLIB
//#include  <cstdlib>
//#elif defined HAVE_STDLIB_H
//#include <stdlib.h>
//#else
//#error Neither <cstdlib> or <stdlib.h> found
//#endif

//#if defined HAVE_CMATH
//#include <cmath>

//#elif defined HAVE_MATH_H
//#include <math.h>
//#else
//#error Neither <cmath> or <math.h> found
//#endif
