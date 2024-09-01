
#pragma once

// Stl include
#include <vector>
// export/import
//#include "PlugIn.h"

// forward declaration (BaseType library)
namespace dbpp {
class ListSectFlow;
}
// namespace jb {
// class scalarField;
//}

namespace dbpp {
// Design Note
// ------------
//    This a new version of the RHS (Right-Hand-Side discretization)
//    More prototyping at this stage, i am just trying to express
//    the signature of the methods in terms of the programming env. type.
//
// An implementation of the RHS(Right-Hand-Side) terms discretization
class BaseRhsDiscretization {
public:
  typedef std::vector<double> vecD;

public:
  virtual ~BaseRhsDiscretization() {}
  // flux algorithm based on Nujic discretization
  // Reference: Nujic (1995) "Essentially Non-Oscillatory ... to be completed"
  virtual void calculFF() = 0;
  virtual void CalculS0() {}
  // source term discretization (... to be completed)
  // in this case, H is provided  by the list of section flow
  //		virtual void TraitementTermeSource2( vecD& aS,
  //emcil::ListSectFlow* alistSectF, 			const jb::scalarField* aA, const
  //jb::scalarField* aQ) {}
  virtual void TraitementTermeP() = 0;
  virtual void TraitementTermeSource() = 0;
};
// Used for hooks into shared libraries
typedef BaseRhsDiscretization *(*Create_t)(void);
typedef bool (*Destroy_t)(BaseRhsDiscretization *);
} // namespace dbpp

// NOTE: in windows platform we need to implement the C interface
// as mentioned in the very good article "How to Implement C interface."
// Used for hooks into shared libraries
// typedef sfxnum::BaseRhsDiscretization* (*Create_t)(void);
// typedef bool (*Destroy_t)(sfxnum::BaseRhsDiscretization*)
