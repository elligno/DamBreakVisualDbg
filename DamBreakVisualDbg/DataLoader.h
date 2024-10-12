
#pragma once

// C++17 includes
#include <filesystem>
#include <optional>
// Qt includes
#include <QFile>
#include <QObject>
// App include
#include "GraphSettingsPrm.h"

namespace qplot {

// Should I inherit from QObject? yes if you want to be managed by Qt framework
class DataLoader : public QObject {
  Q_OBJECT
public:
  // usefull typedef (actually we don't really need it, just the "auto" keyword)
  using mapValU =
      std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;

  using mapStrListU =
      std::map<QStringList::size_type, std::pair<QStringList, QStringList>>;

public:
  // any ctor??
  //  void loadData2Show(GraphSettingsPrm aGraphSettings);
  CurvesData2Show loadData2Show(QFile &aFile2Read,
                                std::optional<QFile> &aOptFile,
                                const eFileResultFormat aFileFmt);
  void setWorkingFolder(const std::filesystem::path &aPath);
  void setFileName(std::filesystem::path &aFilepath) { m_filePath = aFilepath; }
  std::filesystem::path getFileName() const { return m_filePath; }

private:
  // file contains data????
  std::filesystem::path m_filePath;

  //
  // Helpers (under construction)
  //

  /** */
  //  void PrepareSimRes2View(mapValU &aMapU12p, mapValU &aMapU12,
  //                          const std::filesystem::path &aFilename);
  /** */
  //  void readDataFromFile(QFile &aFile2read, GraphSettingsPrm
  //  &aGraphSettings);
  /** */
  //  void dambreak2Curve(GraphSettingsPrm &aGraphSettings);
  // void readFiles2Cmp();  deprecated

  // prototyping functions (return by copy since C++17 copy elison is mandatory)
  CurvesData2Show readDbgDataFromFile(QFile &aFile2read);
  CurvesData2Show readTwoColumnsDataFromFile(QFile &aFile2read);
  CurvesData2Show readDataFullResultFromFile(QFile &aFile2read);
  CurvesData2Show readTwoCurveDataFromFiles(QFile &aFile1, QFile &aFile2);
};
} // namespace qplot
