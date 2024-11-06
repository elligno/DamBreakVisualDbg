
// boost include
#include <boost/filesystem.hpp> // boost file system utility
// ...
#include "../SfxTypes/dbpp_ListSectFlow.h"
#include "dbpp_Simulation.h"
//#include "../Utility/dbpp_SimulationUtility.h"
#include "../Utility/dbpp_EnvVarMgr.h"

// namespace dbpp {
// ListSectFlow *g_ListSectFlow;
//}
namespace dbpp {
// default
// NOTE: algo name has changed since last month
// EMcNeaeil1D is now an abstract class, below we
// algoname to EMcNeil is probably no more valid
Simulation::Simulation()
    : m_dbData{DamBreakData::DiscrTypes::emcneil}, // default,
      m_dbDataType{DamBreakData::DiscrTypes::emcneil}, m_dt{0.}, m_time{0.},
      m_algoName{"EMcNeil1D"}, m_running{false}, m_iterNumber{1},
      m_nbIterMax{1}, m_cfl{0.6}, m_NbSections{101} {

  // check if we use of the pre-defined discretization
  // for validation purpose, if so set to ..
  if (m_dbDataType == DamBreakData::DiscrTypes::emcneil ||
      m_dbDataType == DamBreakData::DiscrTypes::hudson) {
    m_NbSections = m_dbData.nbSections();
  }
}

// something we will be using when interacting with GUI
// we might put all algorithm supported in a file and then
// read it in the GUI routine, or something like this
void Simulation::loadAlgorithm() {
  // set namespace for clarity
  namespace bfs = boost::filesystem;
  // ???
  std::string w_nameAlgo;

  // read from file implemented algorithm
  // retrieve environment variable for this project
  std::string w_testvs12var;
  if (dbpp::EnvVariablesManager::IsOSVariableSet("TESTVS12_ROOT")) {
    w_testvs12var = dbpp::EnvVariablesManager::GetOSVariable("TESTVS12_ROOT");
    std::cout << "Original TESTVS12_ROOT variable is:" << w_testvs12var << "\n";
  }

  bfs::path w_currentPath;
  w_currentPath = bfs::current_path();
  const std::string w_pathStr = w_currentPath.string();

  // using Boost string library to

  //	const std::string w_rootDir(w_testvs12var);
  // current dir??
  //	std::string w_pathStr( w_rootDir+std::string("SimTestJun15"));
  const std::string w_fileName("AdvanceAlgo.txt");
  //	bfs::path w_dir(w_pathStr);

  // create it
  // 	if( bfs::create_directory(w_dir))
  // 		std::cout << "Success" << "\n";

  // find recursively a file in a directory
  bfs::path w_SearchDir(w_currentPath);
  bfs::path w_DirContainingFile; // contains what??
  bfs::path w_SearchFile("AdvanceAlgo.txt");

#if 0
		if( FindFile11( w_SearchDir, w_DirContainingFile, w_fileName))  
		{  
			std::cout << "File location:" << std::endl;  
			std::cout << w_DirContainingFile.string();  
		}  

		// check if we found something
		if( w_DirContainingFile.empty())
		{
			// default file to read or algorithm as default 
			// to run the simulation (to be completed) 
			std::cerr << "Problem with config file for algo\n";
		}
		else
		{
			// otherwise just 
			std::ifstream w_algoNameFile( w_DirContainingFile.string(), 
				std::ifstream::basic_ios::in);
			// now 
			while( !w_algoNameFile.eof())
			{
				w_algoNameFile >> w_nameAlgo;

				// create list of numerical treatment algorithm 
				// add it to the list
				m_listOfAlgorithm.push_back( w_nameAlgo);
			}
		}
#endif
}

} // namespace dbpp
