#ifndef dbappENVVARMGR_H
#define dbappENVVARMGR_H

#include <string>
#include <vector>
#include <map>

namespace dbpp
{
	/** Brief Class to manage environment variable. 
	*   This is a simple wrapper around putenv() and getenv() proving 
	*   persistent storage for the values passed to putenv() (in the form of “name=value”)
	*/
	class EnvVariablesManager 
	{
	public:
		/**
		*   Useful typedef to clarify code 
		*/
		typedef std::vector<char> VariableContainer;                 /**< variable for counting*/
		typedef std::map<std::string, VariableContainer> Variables;  /**< container of all variables*/

		/** 
		* default ctor
		*/
		EnvVariablesManager() {}
		/** 
		 * mapping between variable name and its value 
		 * @param name a character 
		 * @param value a character  
		 */
		void put( const std::string& name, const std::string& value);
		/** 
		 * mapping between variable name and its value 
		 * @param name a character 
		 */
		std::string get( const std::string& name) const;
		/** 
		 * mapping between variable name and its value 
		 * @param name a character 
		 */
		void del( const std::string& name);
		/** 
		 * mapping between variable name and its value 
		 * @param name a character 
		 */

		static void PutOSVariable( char* value);
		/** 
		 * mapping between variable name and its value 
		 * @param name a character 
		 */
		static std::string GetOSVariable( const char* name);
		/** 
		 * mapping between variable name and its value 
		 * @param name a character 
		 */
		static bool IsOSVariableSet( const char* name);

	private:
		/** 
		*  not sure what it is
		*/
		VariableContainer PairToContainer( const std::string& name,
			             const std::string& value) const;
		Variables m_vars; /**< variable*/
		
		/** 
		* copy ctor (This class is not copyable)
		*/
		EnvVariablesManager( const EnvVariablesManager&);
		/** 
		* assignment ctor (This class is not copyable)
		*/
		void operator=( const EnvVariablesManager&);
	};
}
#endif