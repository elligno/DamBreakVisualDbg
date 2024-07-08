
#pragma once

// Global variable of the application
// #include "Sfx_Global.h" temporary put in comment, bug with Global class
//#include "Sfx_Sfxguids.h"

// ======================================================================
//
//                        SFX Framework Macros
//
// ======================================================================

// _____________________________________________________________
//
// Macro: SFX_GUID
// Group:
//
// Description:
//
//   Give a unique number to a COM object and Interface.
//   IID is simply a special GUID. A universally unique identifier
//   (UUID) that defines a COM component. Each COM component has its
//   CLSID in the Windows Registry so that it can be loaded by other
//   applications.
//
//#define SFX_GUID( name) name##_IID

// ======================================================
//
// Macro: SFX_IDENTITY
///
/// Create global unique symbols
///

//
//  Macro: SFX_GUID
//  Group: Identity type
//  Summary: Create the name of a GUID
//
//  Arguments:
//    name - Name of the identified element (literal)
//
#define SFX_GUID(name) name##_IID
// ======================================================

//
//  Macro: SFX_IDENTITY
//  Group: Identity type
//  Summary: Create the identity name (literal)
//
//  Arguments:
//    name - Name of the identified element (literal)
//
#define SFX_IDENTITY(name) SFX_GUID(name)

// _____________________________________________________________
//
// Macro: SFX_TYPE_CAST_RETURN
//
// Group:
//
// Description:
//
//    Used in the "getInterface" method for returning a pointer to the
//    interface. I am not sure of this macro. Need to be revised.
//
//    getInterface(aInterfaceGuid)

#define SFX_TYPE_CAST_RETURN(aType)                                            \
        if ( aInterfaceGuid == SFX_GUID( aType)  \
    static_cast<aType*>(this)

// _____________________________________________________________
//
// Macro: FORM_CANONICAL
// Group: Singleton Concept
// Description:  Voir le livre de James Coplien
//    "Advanced C++: programming style and idioms"
//

#define SFX_CANONICAL_FORM(aTypeName)                                          \
private:                                                                       \
  aTypeName(const aTypeName &aTypeName) = delete;                              \
  aTypeName &operator=(const aTypeName &aTypeName) = delete;

// _____________________________________________________________
//
// Macro:SFX_GET_PROVIDER
// Group: Provider Concept
// Summary: lookup a provider by its interface
//
//  Description:
//

#define SFX_GET_PROVIDER(aInterface)                                           \
  (Sfx::aInterface *)Dsn::g_pDsnGlobal->lookupProvider(SFX_GUID(aInterface))

// _____________________________________________________________
//
// Macro: SFX_ACCESS_PROVIDER
// Group: Provider Concept
//
// Description:
//

#define SFX_ACCESS_PROVIDER(aInterface, aProvider)                             \
  aInterface *aProvider = SFX_GET_PROVIDER(aInterface);

// _____________________________________________________________
//
// Macro: SFX_SETUP_PROVIDER
// Group:
//
// Description: TO BE COMPLETED

// #define SFX_SETUP_PROVIDER( ...)       \
//   protected:                           \
//   static Provider * mProvider;         \
//   private:                             \
//   void * getProvider()                 \
// 		{                                 \
// 		if( mProvider == NULL)            \
// 		/* to be completed */             \
// 		}                                 \
// 		void clearProvider()              \
// 		{                                 \
// 		if (mProvider != NULL)            \
// 		gGlobal->deleteIt();              \
// 		}                                 \
//      void lookupProvider()             \
// 	    {                                 \
// 		 return mProvider;                \
// 	    }

// .....
#define SFX_ASSERT                                                             \
  (aCondition, aDescription) do {                                              \
    if (!(aCondition))                                                         \
      throw Sfx::AssertionFailedError(aDescription);                           \
  }                                                                            \
  while (false)

// ....
#define SFX_REQUIRE(aCondition, aDescription)                                  \
  do {                                                                         \
    if (!(aCondition))                                                         \
      throw Sfx::PreConditionNotSatisfiedError(aDescription);                  \
  } while (false)

// ...........
#define SFX_ENSURE                                                             \
  (aCondition, aDescription) do {                                              \
    if (!(aCondition))                                                         \
      throw Sfx::PostConditionNotSatisfiedError(aDescription);                 \
  }                                                                            \
  while (false)

// ========================================================
//
//                        Container
//
// ========================================================
#define SFX_DECLARE_CONTAINER(aType)                                           \
  typedef aType value_type;                                                    \
  typedef value_type *pointer;                                                 \
  typedef const value_type *const_pointer;                                     \
  typedef value_type &reference;                                               \
  typedef const value_type &const_reference;                                   \
  typedef pointer iterator;                                                    \
  typedef const_pointer const_iterator;

// _____________________________________________________________
//
// Macro: SFX_CANONIC_FORM
// Group:
// Description: NOT COMPLETED
//
#define SFX_CANONIC_FORM(aTypeName)                                            \
public:                                                                        \
  aTypeName();                                                                 \
  aTypeName(const aTypeName &);                                                \
  aTypeName &operator=(const aTypeName &);                                     \
  ~aTypeName();

// _____________________________________________________________
//
// Macro: SFX_PRIVATE_CTOR
// Group:
// Description:
//    Define copy constructor and the operator = private ....
//    instead of using the compilator one (default if the user
//	  don't define one) the description will be completed ......
//
#define SFX_PRIVATE_CTOR(aTypeName)                                            \
private:                                                                       \
  aTypeName(const aTypeName &);                                                \
  aTypeName &operator=(const aTypeName &);
// _____________________________________________________________
//
// Macro: SFX_GET_INTERFACE
//
// Group: Interface Concept
//
// Description:
//
#define SFX_GET_INTERFACE(aInterface, aIAccess)                                \
  (aInterface *)aIAccess->getInterface(SFX_STRID(aInterface))

// ======================================================
//
// Macro: SFX_ACCESS_INTERFACE
//
// Group: Interface Concept
//
// Description:
// ======================================================
#define SFX_ACCESS_INTERFACE(aVariable, aInterface, aIAccess)                  \
  aIAccess = dynamic_cast<Sfx::IAccess *>(aInterface);                         \
  if (aIAccess != NULL) {                                                      \
    aVariable = SFX_GET_INTERFACE(aInterface, aIAccess);                       \
  }
// ======================================================
//
// Macro: SFX_INSTANTIATE
//
// Description:
//
//     TO BE COMPLETED ( i think it has something to do with the
//     factory pattern, i am not sure).
// ======================================================
#define SFX_INSTANTIATE(aTypeName, aInterface)                                 \
        ((aInterface*)aTypeName->createInstance(SFX_IDENTITY( aInterface))
// ======================================================
//
// Macro: SFX_SETUP_PROVIDER
//
// Description:
// ======================================================
#define SFX_SETUP_PROVIDER(aProvider)                                          \
private:                                                                       \
  static aProvider *m_Instance;                                                \
                                                                               \
protected:                                                                     \
  aProvider();                                                                 \
  aProvider(const aProvider &aOther);                                          \
  aProvider &operator=(const aProvider &aOther);                               \
                                                                               \
public:                                                                        \
  static aProvider *getInstance();
// ======================================================
//
// Macro: SFX_IMPLEMENT_PROVIDER
//
// Description:
// #define IMPLEMENT_INSTANCECOUNTER( cls, name ) \
//  CSDInstancesCounter cls##::s_instanceCounter( name );
//
// ======================================================
#define SFX_IMPLEMENT_PROVIDER(aProvider)                                      \
  aProvider *aProvider::m_Instance = NULL;                                     \
  aProvider *aProvider::getInstance() {                                        \
    if (m_Instance == nullptr) {                                               \
      m_Instance = new aProvider();                                            \
    }                                                                          \
    return m_Instance;                                                         \
  }

// ====================================
//          Factory Macros
// ====================================
#define SFX_IMPLEMENT_FACTORY(aName) aName s_myFactory;
