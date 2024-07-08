#ifndef ENVVARMGR_H
#define ENVVARMGR_H

// STL includes
#include <string>
#include <vector>
#include <map>

namespace dbpp 
{
	/** Brief This is a simple wrapper around putenv() and getenv() proving 
	 persistent storage for the values passed to putenv() (in the form of “name=value”). */
	class EnvVariablesManager 
	{
	public:
		/** Some useful typedef for clarity 
    */
		typedef std::vector<char> VariableContainer;
		typedef std::map<std::string, VariableContainer> Variables;

		/** Default ctor 
    */
		EnvVariablesManager() {}

		/** Get/set method ... to be completed 
		*/ 
		void put( const std::string& name, const std::string& value);
		std::string get( const std::string& name) const;
		void del( const std::string& name);

		/** Get/set method ... to be completed 
		*/
		static void PutOSVariable( char* value);
		static std::string GetOSVariable( const char* name);
		static bool IsOSVariableSet( const char* name);

	private:
		/**  ... to be completed 
		*/
		VariableContainer PairToContainer( const std::string& name,
			const std::string& value) const;
		Variables m_vars; /**< environment variable*/

		/** This class is not copyable (copy and assignment access denied). */
		EnvVariablesManager( const EnvVariablesManager&);
		void operator=( const EnvVariablesManager&);
	};
}
#endif