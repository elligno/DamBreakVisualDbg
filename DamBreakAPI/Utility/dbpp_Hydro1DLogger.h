#pragma once

// C++ includes
#include <iostream>
// STL include
#include <vector>
// Boost include
#include <boost/filesystem.hpp> // boost file system utility
// ...
#include "../SfxTypes/dbpp_Singleton.hpp"

namespace dbpp {
namespace bfs = boost::filesystem;

/** This class is just a wrapper around E. McNeil debugging
 * and result file (write to file information to be used for
 * debugging and visualizing). In E. McNeil code, there is lot
 * of those fprintf() which make hard to read and follow.
 * Actually it's more a helper to make code a little more
 * readable. In another version of this logger it would be
 * nice to use the template base Singleton from you inherit
 * by passing as template argument the class logger for example.
 * Usage:
 *  as global instance of the program
 * copy and assignment ctor access denied
 * since we inherit from the boost::noncopyable.
 */
class DbgLogger : public Singleton<DbgLogger> {
  // i am not sure what for?? ok, some of our writing
  /** method use a tuple to store data.*/
  using tuplevec =
      std::tuple<unsigned, std::vector<double>, std::vector<double>>;
  // typedef std::tuple<int, double, double> tupleidd;

public:
  /** open file for writing (file name as default)*/
  void open(/*const std::string &aFilename = "LoggerFile.txt"*/);
  bool isOpen() { return m_opened; }             /*< check file status*/
  void close();                                  /*< close when finish writing*/
  void write2file(const std::string &aMsg = ""); // writing to file
  void write2file(std::string &&aMsg2Write);
  void write2file(const std::string &aMsg, const int aVal);
  void write2file_p(const tuplevec &aTuple);
  void write2file(const tuplevec &aTuple);
  // i am not sure about this one?? do not think so!!
  void write2file(const std::tuple<int, double, double> &aTuple);
  bfs::path getWorkingDirectory() const { return m_currrentWrkDirectory; }
  void setDbgWorkingFile(); // create the name debug working file

  /** First time we call instance() it call the logger default ctor.
   * But the problem is that ctor is protected which cannot be call
   * outside of the method or client. The method instance is static,
   * no this pointer, that's why compiler is complaining about ctor
   * protected attribute?? i am not sure i need to find out!!!
   */
  friend class Singleton<DbgLogger>;

protected:
  /** don't want to be public, because client could make many instance
   * of the logger, and since we want the logger to have a sole instance
   * no choice to make it protected. Client have no choice to use the
   * static method instance() which manage the creation of the sole instance.
   */
  DbgLogger(); // logger creation

private:
  bool m_opened; /*<File open status*/
  // from E. McNeil code
  FILE *FichierDEBUG;        /*<File pointer*/
  char NomFichierDEBUG[256]; /*<File name*/
  // file name, in this case it is set to a default ??
  std::string m_fileName; /*<File name*/
  boost::filesystem::path m_currrentWrkDirectory;
};
} // namespace dbpp
