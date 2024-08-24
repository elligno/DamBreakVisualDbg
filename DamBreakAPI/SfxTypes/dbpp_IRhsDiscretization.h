
#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// stl include
#include <vector>

// include of the export/import
//#include "PlugIn.h"
#include "dbpp_Access.h"
//#include "IConservative.h"

namespace dbpp 
{
	// Abstract virtual pure (interface) 
	// Design Note
	//  to be exported by the library No, we are using a COM (Component Object Mdel)
	//  We provide as the interface the 
	//  Important
	//   This is just a test, the only purpose of this interface is to validate the 
	//   COM mechanism (instead of exporting the whole class, we only return a pure 
	//   virtual class nothing that a pointer to the virtual table).
  //
	class IRhsDiscretization 
	{
  public:
    // useful typedef
    typedef std::vector<double> vecdbl;

	public:

    // DESIGN NOTE
    // This is the numerical treatment, numerical discretization.
    //  No more std vector in the signature

    //
    // Review all the comment below, some of the method do more thn they should.
    // 

    // Design Note
    virtual void getNumericalTreatment() = 0;
 #if 1
   
    // pressure term discretization 
    virtual void TraitementTermeP ( vecdbl& PF2, vecdbl& P2, const vecdbl& U1, const int NbSections, const double B=1.) = 0;
    
    // bathymetry source term discretization
    virtual void CalculS0 (vecdbl& S0, const vecdbl& Z, const double dx, const int NbSections) = 0 ;
    
    // another discretization of the bottom source term
    virtual void TraitementTermeSource (vecdbl& S, const vecdbl& Q, const vecdbl& A, const vecdbl& Z,
                             const vecdbl& n, const double dx, const int NbSections, const double B=1.) = 0;

    // discretization of the friction and bottom source term
    virtual void TraitementTermeSource2 (vecdbl& S, const vecdbl& Q, const vecdbl& A, const vecdbl& H,
      const vecdbl& n, const double dx, const int NbSections, double B=1.) = 0;

    // numerical flux at the interface
    virtual void CalculFF (vecdbl& FF1, vecdbl& FF2, const vecdbl& U1, 
      const vecdbl& U2, vecdbl& dU1, vecdbl& dU2,
      const int NbSections, double B=1.) = 0;
#endif
    
    // release resource
    virtual void Release()=0;
	};
} // End of namespace

//#endif !irhsdiscretization_H

#ifdef __cplusplus
// Handle type. In C++ language the interface type is used.
typedef dbpp::IRhsDiscretization* RHSHANDLE;
#else   // C
// Handle type. In C language there are no classes, so empty struct is used
// in order to ensure type safety.
typedef struct tagRHSHANDLE {} * RHSHANDLE;
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// 
#ifdef __cplusplus
#   define EXTERN_C     extern "C"
#else
#   define EXTERN_C
#endif // __cplusplus

// Used for hooks into shared libraries
// typedef sfxnum::IRhsDiscretization* (*CreatePOM)(void);
// typedef bool (*DeletePOM)(sfxnum::IRhsDiscretization*);

// Factory function that creates instances of the RhsDiscretization object.
EXTERN_C
  RHSHANDLE
  WINAPI
  GetRhsDiscretization(
  VOID);

EXTERN_C
  VOID
  APIENTRY
  RhsRelease(
  RHSHANDLE handle);