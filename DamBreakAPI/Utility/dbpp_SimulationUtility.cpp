#include "dbpp_SimulationUtility.h"
#include "../Utility/dbpp_TestLogger.h"
#include <cstdlib> // EXIT_FAILURE

namespace dbpp {
bool FindFile11(
    const std::filesystem::path &directory, /* root directory to start with */
    std::filesystem::path &path, /* location path of the searched file */
    const std::string &filename) // file name to look for
{
  bool found = false;

  const std::filesystem::path file = filename;
  const std::filesystem::recursive_directory_iterator end;
  const std::filesystem::recursive_directory_iterator dir_iter(directory);

  // use auto and lambda function &file input argument of the lambda
  const auto it = std::find_if(
      dir_iter, end, [&file](const std::filesystem::directory_entry &e) {
        return e.path().filename() == file;
      });

  if (it != end) {
    path = it->path();
    found = true;
  }

  return found;
}

std::string currentDateTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  localtime_s(&tstruct, &now);

  // for more information about date/time format
  strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

  return std::string(buf);
}

std::string getDateFormat() {
  // retrieve the current date and time for simulation result
  const std::string wDate = currentDateTime();
  // position of the search character, want to strip the time for now
  std::size_t w_Found = wDate.find_first_of(".");
  std::string w_DateStr(wDate);
  std::string::iterator w_ConstIter = w_DateStr.begin();
  w_ConstIter += w_Found;
  // erase time part of the string
  w_DateStr.erase(w_ConstIter, w_DateStr.end());

  return w_DateStr;
}

std::string CreateAlgoWrkFile(const std::string &aCurDirStr /*=""*/) {
  std::cout << "Created the new directory to save file\n";
  // std::string w_activeAlgo("JeanB");
  if (aCurDirStr.empty()) {
    std::string w_save_file_name = "EMcNeil1D_mod"; // m_activeAlgo;
    w_save_file_name.append("_algo");
    w_save_file_name.append("_");
    w_save_file_name.append("1"); // default
    w_save_file_name += ".txt";   // file extension
    return w_save_file_name;
  } else {
    std::string w_save_file_name =
        aCurDirStr + "\\" + "EMcNeil1D_mod"; // m_activeAlgo;
    w_save_file_name.append("_algo");
    w_save_file_name.append("_");
    w_save_file_name.append("1"); // default
    w_save_file_name += ".txt";   // file extension
    return w_save_file_name;
  }
}

std::filesystem::path setDbgWorkingDir() {
  namespace fs = std::filesystem;

  // const std::string w_valiadationDir{};
  // retrieve the current date
  auto w_currDate = getDateFormat();

  // Debugging purpose: print the string to make sure is what we expect
  std::cout << "Date format is: " << w_currDate << "\n";

  auto currentPath = fs::current_path();
  // currentPath = fs::current_path();
  // auto w_curDirString = fs::canonical(currentPath).string();
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Current path for this simulation is: %s"}.data(),
      currentPath.string().c_str());

  // std::string w_dir2look(std::string("\\") + w_valiadationDir);
  // w_curDirString += w_dir2look + std::string("\\Result") + w_currDate;

  return currentPath; // w_curDirString;
}

dbpp::fs::path createDbgFolderWithFile(/*const std::string& aValidationDir*/) {
  // create a directory
  namespace fs = std::filesystem;

  auto w_curDirPath = setDbgWorkingDir();

  // bfs::path DirContainingFile11{};  store founded file

  // corresponding file to simulation result
  // auto w_fileName = CreateAlgoWrkFile(w_curDirString);

  // directory format is the following "Result2023-10-26"
  // fs::path w_destination{w_curDirString};
  //  const fs::path file = CreateAlgoWrkFile(w_curDirString);

  try {
    // create directories DBppGuiDev\DBppGuiAppl15\result
    //    auto w_algoFileName = CreateAlgoWrkFile();
    //    w_curDirPath /= w_algoFileName;
    fs::path testDir{w_curDirPath};
    create_directories(testDir);
  } catch (const fs::filesystem_error &e) {
    std::cerr << "EXCEPTION" << e.what() << "\n";
    std::exit(EXIT_FAILURE); // exit program with failure
  }

  // const bfs::directory_iterator end;
  // const bfs::directory_iterator dir_iter(DirContainingFile11 /*directory*/);
  // return number of file in the directory
  // auto dirDist = std::distance(dir_iter, end);
  // auto checkType = *dir_iter;
  // auto okHasFileName = checkType.path().has_filename();

#if 0  // temporary fix for debugging
  if (bfs::exists(w_destination)) // check if destination exist?
  {
//#if 0   temporary fix for debugging
    // let's put it in comment for now, one thing i should do
    // refactor the signature of this method with const char*?
    dbpp::Logger::instance()->OutputSuccess(
        "Destination directory: %s %s", w_destination.string().c_str(),
        std::string{" already exists."}.c_str());
//#endif  0
    if (w_destination.empty()) {
      const auto w_save_file_name = CreateAlgoWrkFile(w_curDirString);
      return bfs::path{w_save_file_name};
    } else {
      // find last file (distance of dir_iter)
      // split file about '.'
      // last character of file name is the number
      // incr by 1 std::atoi +1
      // replace back() by the number
      // done

      // find file by recursive procedure
      if (FindFile11(w_destination, DirContainingFile11, w_fileName)) {
        // actually it's not what we want, format of our file is with underscore
        // with a number, so we want to retrieve that number and increment it
        size_t w_foundIt = w_fileName.find_last_of("_");
        if (w_foundIt != std::string::npos) {
          // found one "_"
          auto w_beg = w_fileName.begin();
          std::advance(w_beg, w_foundIt + 1);  // version number location
          const char w_char2look = *w_beg;     // what we are looking for
          auto versionNo = atoi(&w_char2look); // convert to an integer
          do                                   // search for last version saved
          {
            // replace version number by upgraded one
            ++versionNo; // next version
            w_fileName.replace(w_foundIt + 1, 1, std::to_string(versionNo));
          } while (FindFile11(w_destination, DirContainingFile11, w_fileName));
        }

        // testing ... ???
        // std::string w_save_file_name = w_curDirString + "\\" + w_fileName;
        return DirContainingFile11;
      }
    }
  } // directory exist
  return bfs::path{};
#endif // 0
}

} // namespace dbpp
