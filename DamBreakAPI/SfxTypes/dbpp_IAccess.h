
#pragma once

#include <iostream>

// some basic guids for testing (unsigned)
// #define IID_IUpdateable   1111
// #define IID_IDisplayable  2222
// #define IID_IConservative 3333
// #define IID_IType         4444

// some basic macro
//#define SFX_IDENTITY(aType) IID_##aType

namespace dbpp 
{
		// interface for COM mechanism support
		//
		class IAccess 
		{
		public:
			virtual void* getInterface( const unsigned aGuid)=0;
			virtual void* getInterface( const std::string& aInterface)=0;
		};

		// some interface
		class IUpdateable
		{
		public:
			virtual void update()=0;
			// if you don't make it virtual and add it to the collection 
			// of the stl for example, you run through an error (derived 
			// class dtor call then the base class dtor is call, but if 
			// don't exist you have an error).
			virtual ~IUpdateable() { std::cout << "IUpdateable dtor\n";}
		protected:
		private:
		};

		class IDisplayable 
		{
		public:
			virtual void display()=0; 
		private:
		};
} // End of namepsace