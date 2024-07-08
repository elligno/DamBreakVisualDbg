// C++ includes
#include <utility>
#include <tuple>
// Boost includes
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
// App include
#include "dbpp_Hydro1DLogger.h"
#include "SfxTypes/dbpp_Simulation.h"
#include "Utility/dbpp_SimulationUtility.h"

namespace dbpp 
{
	// initialize the static variable 
//	Hydro1DLogger* Hydro1DLogger::m_sHlogger=nullptr;

	// default ctor
	DbgLogger::DbgLogger()
  : m_opened{ false },
    m_fileName{ Simulation::instance()->getAlgorithmName() },
    m_currrentWrkDirectory{}
	{
    namespace bfs = boost::filesystem;

    // retrieve the current date
    //auto w_currDate = getDateFormat();
    m_currrentWrkDirectory = std::string{ "AlgoValidation/Result" } + getDateFormat();

    // create a working directory
    if( !bfs::exists(m_currrentWrkDirectory))
    {
      if( bfs::create_directory(m_currrentWrkDirectory))
      {
        // created successfully (add code here)
      }
    }
	}

	// closing file
	void DbgLogger::close()
	{
		// Close FichierDEBUG if it is not NULL 
		if( FichierDEBUG)
		{
			errno_t err = fclose( FichierDEBUG);
			if ( err == 0 )
			{
				printf( "The logger file 'HLL RK2 Debug' was closed\n" );
				FichierDEBUG=nullptr;
				m_opened=false;
        m_fileName.clear();
			}
			else
			{
				printf( "The logger file 'HLL RK2 Debug' was not closed\n" );
			}
		}
	}
    // 
	void DbgLogger::open( const std::string& aFilename/*="LoggerFile.txt"*/)
	{
     namespace bfs = boost::filesystem;
     m_currrentWrkDirectory /= m_fileName;
     m_currrentWrkDirectory.normalize();
//     if( bfs::exists(m_currrentWrkDirectory))
//     {
//       // increment version number
//     }

     auto w_filename = m_currrentWrkDirectory.string();

		// initializing file creation 
		::strcpy_s( NomFichierDEBUG, 256, w_filename.c_str());

		// Open for write 
		errno_t err = fopen_s( &FichierDEBUG, NomFichierDEBUG, "w" );
		if( err == 0 )
		{
			printf( "The logger file 'HLL RK2 Debug' was opened\n");
			m_opened=true;
      m_fileName = aFilename;
		}
		else
		{
			printf( "The logger file 'HLL RK2 Debug' was not opened\n");
		}
	}

	void DbgLogger::write2file( const std::string& aMsg, const int aVal)
	{
		if( aMsg.empty())
		{
			//	Evaluation de F1 et F2
			fprintf( FichierDEBUG, "++++Iteration number: %d\n", aVal);

			// 		for (j = 0; j < NbSections; j++)
			// 			fprintf( FichierDEBUG, "U1[%d]: %lf   U2[%d]: %lf\n", j, U1[j], j, U2[j]);

			fprintf( FichierDEBUG, "\n");
		}
	}

  void DbgLogger::write2file( const std::string& aMsg/*=""*/)
  {
    fprintf( FichierDEBUG, aMsg.c_str());
  }

  void DbgLogger::write2file( std::string&& aMsg2Write)
  {
    fprintf( FichierDEBUG, std::move(aMsg2Write).c_str());
    fprintf(FichierDEBUG, "\n");
  }

  void DbgLogger::setDbgWorkingFile()
  {
    // create/initialize filename 
    if( bfs::exists(m_currrentWrkDirectory) && bfs::is_directory(m_currrentWrkDirectory))
    {
      // contains file?
      const bfs::directory_iterator end;
      const bfs::directory_iterator dir_iter{ m_currrentWrkDirectory };
      auto folder_dist = std::distance(dir_iter, end);
      if( folder_dist == 0) // empty, no files
      {
        // corresponding file name to simulation result
        //m_fileName = "EMcNeil1D_mod"; //m_activeAlgo;
        m_fileName.append("_algo");
        m_fileName.append("_");
        m_fileName.append("1"); // default 
        m_fileName += ".txt"; // file extension
      }
      else // not empty, contains 
      {
        bfs::directory_entry& w_file = *dir_iter; // last file in directory
        auto strName = w_file.path().filename().string(); // file name
        std::vector<std::string> w_nameExt;
        w_nameExt.reserve(2);
        boost::split(w_nameExt, strName, boost::is_any_of(".")); // without extension
        auto w_lastDigit = w_nameExt[0].back(); // last digit of file name
        auto w_toInt = std::atoi(&w_lastDigit); // char to integer
        w_toInt += 1;
        auto& w_updatdInt = w_nameExt[0].back();
        w_updatdInt = *const_cast<char*>(std::to_string(w_toInt).c_str());
        auto w_newName = w_nameExt[0]; // update version number
        w_newName += std::string{ "." } + w_nameExt[1];
        // avoid copying long string without SSO (Small String Optimization)
        m_fileName = std::move(w_newName); // swap resources
      }//else
    }//if bfs exist
  }

  void DbgLogger::write2file_p(const tuplevec& aTuple)
	{
		for( unsigned j = 0; j < std::get<0>(aTuple); j++) 
			fprintf( FichierDEBUG, "U1p[%d]: %lf   U2p[%d]: %lf\n", j, std::get<1>(aTuple)[j], 
			          j, std::get<2>(aTuple)[j]);
		
		fprintf( FichierDEBUG, "\n");
	}
	void DbgLogger::write2file( const tuplevec& aTuple)
	{
		for( unsigned j = 0; j < std::get<0>(aTuple); j++) 
			fprintf( FichierDEBUG, "U1[%d]: %lf   U2[%d]: %lf\n", j, std::get<1>(aTuple)[j], 
			j, std::get<2>(aTuple)[j]);

		fprintf( FichierDEBUG, "\n");
	}

} // End of namespace

