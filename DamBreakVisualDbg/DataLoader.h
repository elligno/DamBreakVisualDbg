
#pragma once

// C++17 includes
#include <filesystem>
// Qt include
#include <QFile>

namespace qplot {

// Should I inherit from QObject?
class DataLoader {
public:
  // any ctor??
  void loadData2Show(/*GraphSettingsPrm*/) {
    // all 4 functions to load data according to user selection
  }
  void setWorkingFolder(const std::filesystem::path &aPath);
  void setFileName(std::filesystem::path &aFilepath) { m_filePath = aFilepath; }
  std::filesystem::path getFileName() const { return m_filePath; }

private:
  // file contains data????
  std::filesystem::path m_filePath;

  // need a data store?? not sure

  //
  // Helpers (under construction)
  //
  void readDataFromFile(QFile &aFile2read);
  void dambreak2Curve(/*GraphSettingsPrm*/);
};
} // namespace qplot
