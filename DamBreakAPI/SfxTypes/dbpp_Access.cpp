#include "dbpp_Access.h"
//#include "BaseNumGuids.h"
// some basic guids for testing (unsigned)
#define IUpdateable_IID       1111
#define IDisplayable_IID      2222
#define IConservative_IID     3333
#define IType_IID             4444
#define IBaseNumTreatment_IID 1212

namespace dbpp 
{
// 	Access::Access()
// 	: m_strRep("Access"), 
// 		m_intVal(0) 
// 	{ 
// 		std::cout << "Default ctor jbTestA\n"; ++m_intVal;
// 	}

	// DESIGN NOTE
  // Use Boost string library ("split" could be written in 2-lines)
	Access::Access( std::string alistOfInterface) 
	: m_strRep(alistOfInterface),
		m_intVal(1)
	{ 
		std::cout << "Access string ctor\n";
		// split the string in parts (find method)
		while( m_strRep.find(" ")!=std::string::npos)  // go through the string
		{
			// retrieve substring
			std::string::size_type w_first=m_strRep.find_first_of(" ");
			if (w_first!=std::string::npos) // sanity check for empty string
			{
				// we found one, then extract the interface name (start to last index)
				std::string w_intefaceName=m_strRep.substr( 0, w_first);
				if (!w_intefaceName.empty())
				{
					// push each string (list of string)
					m_listofinterfaces.push_back(w_intefaceName);
					// remove substring from the whole string
					m_strRep.erase(0,w_first+1); // now we have a modified string
				}
			}
		}
		if( !m_strRep.empty() && (m_strRep.length()!=0))
		{
			// there is one left (interface name)
			m_listofinterfaces.push_back(m_strRep);
		}
	}

	// copy ctor
// 	Access::Access( const Access& aOther )
// 	{
// 		m_strRep = aOther.m_strRep;
// 		m_intVal = aOther.m_intVal;
//     m_listofinterfaces = aOther.m_listofinterfaces; // is it ok? as simple as that!!

#if 0
    // what we do the list attributes, check if it is empty
		if ( !m_listofinterfaces.empty()) // if not 
		{
			// empty the list
			m_listofinterfaces.clear(); // now ready to copy new elements in the list
      std::copy( aOther.m_listofinterfaces.begin(),aOther.m_listofinterfaces.end(),
				m_listofinterfaces.begin());
		}
		else
		{
			std::copy( aOther.m_listofinterfaces.begin(),aOther.m_listofinterfaces.end(),
				m_listofinterfaces.begin());
		}
#endif
//	}
#if 0

	// 
	Access& Access::operator= (const Access& aOther)
	{
		// check for self assignment
		if( this == &aOther)
		{
      m_listofinterfaces.clear();
      m_strRep = aOther.m_strRep;
      m_intVal = aOther.m_intVal;
      m_listofinterfaces = aOther.m_listofinterfaces; // is it ok? as simple as that!!
		//	return *this;
		}

		// now empty the list
		return *this;
	}
#endif

// 	Access::~Access()
// 	{
// 		// empty the container (list)
// 		std::cout << "TestAcces dtor\n";
// 	}

	// search for the supported interface
// 	bool Access::hasInterface() const
// 	{
		// check for supported interface
// 		using dist = std::iterator_traits<std::list<std::string>::iterator>::difference_type;
// 		dist w_distInterface = std::distance( m_listofinterfaces.cbegin(), m_listofinterfaces.cend());
// 		if( w_distInterface == 0)
// 		{
// 			return false;
// 		}
// 		else
// 		{
// 			return true;
//		}

//     return std::distance( m_listofinterfaces.cbegin(), m_listofinterfaces.cend());
// 	}

	void* Access::getInterface( const unsigned aGuid )
	{
    if (aGuid == IConservative_IID)
      return getInterface("IConservative");
    else if (aGuid == IDisplayable_IID)
      return getInterface("IDisplayable");
    else if (aGuid == IUpdateable_IID)
      return getInterface("IUpdateable");
    else if (aGuid == IBaseNumTreatment_IID)
      return getInterface("IBaseNumTreatment");
		else
			return nullptr;
	}
} // End of namespace 

