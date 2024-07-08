
//  Author: Jean Belanger (Elligno Inc.)
//  Date of creation: July 27, 2010
//  Revision history:

#pragma once

// C++ includes
#include <exception>
#include <string>

namespace Sfx 
{
	// _______________________________________________
	//
	class Error : public std::exception
	{
	public:
		// default ctor (empty string)
		explicit Error( const std::string & what = "")
		: m_Message(what)
		{
			// nothing to do
		}

		// destructor
		~Error() throw () {}

		// override the method from exception class?
		const char* what() const throw () { return m_Message.c_str();}

	private:
		std::string m_Message;
	};

	// _______________________________________________
	//
	class AssertionFailedError : public Error
	{
	public:
		// ctor 
		explicit AssertionFailedError( const std::string & what = "")
		: Error(what)
		{}
	};

	// _______________________________________________
	//
	class PreConditionNotSatisfiedError : public Error
	{
	public:
		// ctor 
		explicit PreConditionNotSatisfiedError( const std::string & what = "")
			: Error(what)
		{}
	};

	// _______________________________________________
	//
	class PostConditionNotSatisfiedError : public Error
	{
	public:
		// ctor 
		explicit PostConditionNotSatisfiedError( const std::string & what = "")
			: Error(what)
		{}
	};

	// _______________________________________________
	//
	class IndexError : public Error
	{
	public:
		// ctor
		explicit IndexError(const std::string & what = "")
			: Error(what)
		{}
	};

	// _______________________________________________
	//
	class IllegalArgumentError : public Error
	{
	public:
		// ctor
		explicit IllegalArgumentError( const std::string & what = "")
			: Error(what)
		{}
	};

	// _______________________________________________
	//
	class OutOfMemoryError : public Error
	{
	public:
		// ctor
		explicit OutOfMemoryError( const std::string & whatClass = "Unknown class")
			: Error(whatClass+ ":Out Of Memory")
		{}
	};
}  // End of namespace