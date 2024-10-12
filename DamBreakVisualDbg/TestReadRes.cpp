#include <stdlib.h> // getenv_s
// C++ includes
#include <filesystem> // boost file system utility
#include <fstream>
#include <iostream>
#include <sstream> //istringstream
// stl include
#include <map>
#include <vector>
// Boost string algorithm
#include <boost/algorithm/string.hpp> // contains()
// Qt includes
#include <QFile>
#include <QTextStream>
// App include
//#include "VisualDbgUtilities.hpp"
// lib API
#include "Utility/dbpp_AppConstant.hpp"

namespace qplot {
// usefull typedef (actually we don't really need it, just the "auto" keyword)
using mapValU =
    std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;

using mapStrListU =
    std::map<QStringList::size_type, std::pair<QStringList, QStringList>>;

#if 0
// NOTE use std FileSystem and remove boost FileSystem
// Recursively find the location of a file on a given directory
// make use of some of the c++11 features : auto keyword and lambda function
bool FindFile(
    const boost::filesystem::path &directory, // root directory to start with
    boost::filesystem::path &path, // location path of the searched file
    const std::string &filename)   // file name to look for
{
  bool found = false;

  const boost::filesystem::path file = filename;
  const boost::filesystem::recursive_directory_iterator end;
  const boost::filesystem::recursive_directory_iterator dir_iter(directory);

  // use auto and lambda function &file input argument of the lambda
  const auto it = std::find_if(
      dir_iter, end, [&file](const boost::filesystem::directory_entry &e) {
        return e.path().filename() == file;
      });

  if (it != end) {
    path = it->path();
    found = true;
  }

  return found;
}

// Usage: std::remove_if stl algorithm
bool check4StrEmpty(const std::string &aStr2check) {
  if (aStr2check.empty()) {
    return true;
  }
  return false;
}
#endif // 0

// output of the simulator:
// U1p[0]: 10.000000   U2p[0]: 0.000000
// this kind of string that we need extract number for plotting
// IMPORTANT: map key which is the iteration number start at 1m NOT 0!!
// when retrieving the values from key, map[0] is not valid!!
// Simplified version for reading simulation result from file and extract to
// prepare for visualizing
void PrepareSimRes2View(
    mapValU &aMapU12p, mapValU &aMapU12,
    const std::string &aFilename) { // shall use std::FileSystem::Path!!!
                                    // not a string!!! "Easy to use correctly,
                                    //  Hard to use incorrectly"!!
  using namespace std;

  // read data source file
  ifstream w_file2read(aFilename);
  if (!w_file2read.is_open()) {
    std::cerr << "Cannot open file for reading simulation data\n";
    return exit(EXIT_FAILURE);
  }
  string w_lineRed;
  w_lineRed.reserve(50); // SSO Short String Optimization
  int w_iterNb = 1;      // initialization

  // store data for future use
  vector<double> w_vU1;
  w_vU1.reserve(dbpp::EMCNEILNbSections::value); // A
  vector<double> w_vU2;
  w_vU2.reserve(dbpp::EMCNEILNbSections::value); // Q
  vector<double> w_vU1p;
  w_vU1p.reserve(dbpp::EMCNEILNbSections::value); // Ap
  vector<double> w_vU2p;
  w_vU2p.reserve(dbpp::EMCNEILNbSections::value); // Qp

  // line-by-line
  while (!w_file2read.eof()) {
    getline(w_file2read, w_lineRed);
    w_lineRed.shrink_to_fit();
    // w_lineRed.start_with("++++"); C++20 could replace boost::contains which
    // available std C++23
    // Can try std::search();
    // const std::string w_textSearch = "Iteration"; instead of contains
    if (boost::contains(w_lineRed,
                        "Iteration number")) { // w_lineRed.start_with("++++");
      // last element shall be iteration number  Iteration number x

      std::istringstream w_iss{w_lineRed}; // white space tokens as default
      std::vector<std::string> w_splitVec{
          std::istream_iterator<std::string>{w_iss}, // range begin
          std::istream_iterator<std::string>{}};     // range end

      auto currNb = stoi(     // first line
          w_splitVec.back()); //++++Iteration Number: 1 ->  back() returns 1
      if (w_iterNb == 1 && currNb == 1)
        continue; // go to next line
      // clearing data to map values for future use
      aMapU12p.insert(
          make_pair(w_iterNb, // current iteration number
                    make_pair(move(w_vU1p), move(w_vU2p)))); // values extracted
      aMapU12.insert(
          make_pair(w_iterNb, // current iteration number
                    make_pair(move(w_vU1), move(w_vU2)))); // values extracted
      w_iterNb = currNb;
      continue;
    }
    if (w_lineRed.empty()) // second line after iteration number
      continue;

    if (w_lineRed.find('p') != std::string::npos) {
      // NOTE std::istringstream operator>> has an interesting property
      // ...
      std::istringstream w_iss{w_lineRed}; // white space tokens as default
      std::vector<std::string> w_splitVec{
          std::istream_iterator<std::string>{w_iss}, // range begin
          std::istream_iterator<std::string>{}};     // range end
      w_lineRed.clear();
      w_lineRed.reserve(50);
      // remove all empty element
      // 				w_strs.erase( std::remove_if(
      // w_strs.begin(), w_strs.end(),  // logical end
      // std::bind( &std::string::empty, std::placeholders::_1)), w_strs.end());
      // physical end
      //				BOOST_ASSERT(w_strs.size() == 4 &&
      //"Removed empty string successfully");
      w_vU1p.push_back(std::stod(
          *(std::next(w_splitVec.begin())))); // assume second element is value
      w_vU2p.push_back(
          std::stod(w_splitVec.back())); // assume last element is value
                                         //			continue;
    } else                               // U1,2
    {
      // 				std::vector<std::string> w_strs;
      // 				boost::split(w_strs, w_lineRed,
      // boost::is_any_of("  "));
      // w_strs.erase(std::remove_if(w_strs.begin(), w_strs.end(),
      // std::bind(&check4StrEmpty, std::placeholders::_1)),
      // w_strs.end()); BOOST_ASSERT(w_strs.size()
      // == 4 && "Removed empty string successfully");
      std::istringstream w_iss{w_lineRed};
      std::vector<std::string> w_splitVec{
          std::istream_iterator<std::string>{w_iss}, // range begin
          std::istream_iterator<std::string>{}};     // range end
      w_lineRed.clear();
      w_lineRed.reserve(50);
      w_vU1.push_back(std::stod(*(std::next(w_splitVec.begin()))));
      w_vU2.push_back(std::stod(w_splitVec.back()));
      //			continue;
    }
  } // while-loop

  //	auto currNb = stoi(w_splitVec[2]);
  //	if (w_iterNb == 1 && currNb == 1) continue;
  // clearing data to map values for future use
  aMapU12p.insert(
      make_pair(w_iterNb, // current iteration number
                make_pair(move(w_vU1p), move(w_vU2p)))); // values extracted
  aMapU12.insert(
      make_pair(w_iterNb, // current iteration number
                make_pair(move(w_vU1), move(w_vU2)))); // values extracted

  // acquisition and release resource (symetric)
  if (w_file2read.is_open())
    w_file2read.close();
}
} // namespace qplot
