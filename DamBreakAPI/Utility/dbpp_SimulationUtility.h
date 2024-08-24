
#pragma once

// C++ includes
#include <string>
#include <iostream>
// boost include
#include <boost/filesystem.hpp> // file system utility
// App include
//#include "dbpp_SomeUtilities.hpp"
//#include "../Utility/dbpp_TestLogger.h"

namespace dbpp 
{
  namespace bfs = boost::filesystem;

	/** Brief Recursively find the location of a file on a given directory.
	 *make use of some of the c++11 features : auto keyword and lambda function
	 *@param root directory to start with
	 *@param location path of the searched file
	 *@param file name to look for
	 */
	bool FindFile11( const bfs::path& directory,  // root directory to start with
		bfs::path& path,             // location path of the searched file
		const std::string& filename );              // file name to look for

  // helper function used for saving (result)
  // Get current date/time, format is YYYY-MM-DD.HH:mm:ss
  // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
  std::string currentDateTime();
  std::string getDateFormat();
  std::string CreateAlgoWrkFile( const std::string& aCurDirStr="");
  std::string setDbgWorkingDir();
  bfs::path createDbgFolderWithFile( /*const std::string& aValidationDir*/);
} //End of namespace
