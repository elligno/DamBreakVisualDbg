
// C++ includes
#include <fstream>
#include <iostream>
#include <sstream> //istringstream
// Qt includes
#include <QDebug>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextStream>
// Boost string algorithm
#include <boost/algorithm/string.hpp> // contains()
// App includes
#include "DataLoader.h"
#include "Enumerators.h"
#include "VisualDbgUtilities.hpp"
// lib API
#include "Utility/dbpp_AppConstant.hpp"

namespace qplot { // forward declaration
void PrepareSimRes2View(mapValU12 &aMapU12p, mapValU12 &aMapU12,
                        const std::string &aFilename);
}

namespace qplot {

// Support 1 or 2 files as input by using C++17 optional
// 1 file:  loadData2Show(file1,std::nullopt,filefmt)
// 2 files: loadData2Show(file1,file2,filefmt)
CurvesData2Show // return by copy since C++17 "Copy Elison is Mandatory"
DataLoader::loadData2Show(QFile &aFile2Read, std::optional<QFile> &aOptFile,
                          const eFileResultFormat aFileFmt) {
  CurvesData2Show w_curvData;
  switch (aFileFmt) {
  case eFileResultFormat::dbgFormat:
    // extract data from result file
    w_curvData = readDbgDataFromFile(aFile2Read);
    break;
  case eFileResultFormat::var_profile:
    w_curvData = readTwoColumnsDataFromFile(aFile2Read); // format: X/H, X/V
    break;
  case eFileResultFormat::full_result: {
    w_curvData =                                // vector<pair<Time, values>>,
        readDataFullResultFromFile(aFile2Read); // X/H or X/Q or X/V user select
    break;
  }
  case eFileResultFormat::exact_computed:
    // Open files for reading data (files opened in 'loadFromFile')
    // but it shouldn't be, all files shall be opened in this function
    // fill data structure 'GraphPrm' to be used to plot graph
    if (!aOptFile.has_value())
      return CurvesData2Show{};
    w_curvData = readTwoCurveDataFromFiles(aFile2Read, *aOptFile);
    break;
  default: // nothing to do for now (not supported format)
    break;
  }
  return w_curvData;
}

// Possible format
// x Var format
// 0     1
//       0.01     1
//       0.02     0.9999999
//       0.03     0.9999998
//       0.04     0.9999995
//       0.05     0.9999987
// for now this function target
CurvesData2Show DataLoader::readTwoColumnsDataFromFile(QFile &aFile2read) {
  // sanity check
  if (!aFile2read.isOpen()) {
    if (!aFile2read.open(QIODevice::ReadOnly)) {
      qDebug() << "Unable to open file" << aFile2read.errorString();
    }
  }

  // some utility to read file
  QTextStream w_data2Read(&aFile2read);
  QString line;
  CurvesData2Show w_curvData;
  // if you don't do that, first time push_back is call
  // vector resize, not really efficient
  w_curvData.m_xVar1.reserve(
      dbpp::EMCNEILNbSections::value); // debugging purpose

  // reading line after one
  while (w_data2Read.readLineInto(&line)) {
    if (line.isEmpty()) // check if line is empty
    {
      continue; // go to next line
    }

    // split about white space () QStringList
    auto w_listDbl = line.split(" ");
    // push in a map or vector (move semantic)
    w_curvData.m_xVar1.push_back(
        {w_listDbl.front().toDouble(), w_listDbl.back().toDouble()});
  } // while

  // acquisition and release resource (symetric )
  if (aFile2read.isOpen())
    aFile2read.close();

  return w_curvData;
}

CurvesData2Show DataLoader::readDbgDataFromFile(QFile &aFile2read) {
  CurvesData2Show w_curvData;
  qplot::PrepareSimRes2View(w_curvData.m_mapValUp, w_curvData.m_mapValU,
                            aFile2read.fileName().toStdString());

  // data type need to display
  w_curvData.setDataType(eDataTypeFmt::DebugFmt);

  return w_curvData;
}

// Compare 2-graph (e.g. exact/computed)
// file format: two-columns x-H,Q,...
CurvesData2Show DataLoader::readTwoCurveDataFromFiles(QFile &aFile1,
                                                      QFile &aFile2) {
  // open file for reading
  aFile1.open(QFileDevice::ReadOnly);
  if (!aFile1.isOpen())
    return CurvesData2Show{};

  // some utility to read file
  QTextStream w_data2Read1(&aFile1);
  QString line;
  CurvesData2Show w_curvData;
  // if you don't do that, first time push_back is call
  // vector resize, not really efficient
  w_curvData.m_xVar1.reserve(
      dbpp::EMCNEILNbSections::value); // debugging purpose

  // reading line after one
  while (w_data2Read1.readLineInto(&line)) {
    if (line.isEmpty()) // check if line is empty
    {
      continue; // go to next line
    }

    // split about white space () QStringList
    auto w_listDbl = line.split(" ");

    // push in a map or vector (move semantic)
    // not sure if we should use move? because return a temporary
    // since C++17 copy elison, return (prvalue: pure reading value),
    // by calling move we prevent this feature
    // here we push a pair ...
    w_curvData.m_xVar1.push_back({w_listDbl.front().toDouble() /*x*/,
                                  w_listDbl.back().toDouble() /*variable*/});
  } // while-loop

  // open file for reading
  aFile2.open(QFileDevice::ReadOnly);
  if (!aFile2.isOpen())
    return CurvesData2Show{};
  QTextStream w_data2Read2(&aFile2);
  // if you don't do that, first time push_back is call
  // vector resize, not really efficient
  w_curvData.m_xVar2.reserve(dbpp::EMCNEILNbSections::value);

  // reading line after one
  while (w_data2Read2.readLineInto(&line)) {
    if (line.isEmpty()) // check if line is empty
    {
      continue; // go to next line
    }

    // split about white space () QStringList
    auto w_listDbl = line.split(" ");

    // push in a map or vector (move semantic)
    w_curvData.m_xVar2.push_back({w_listDbl.front().toDouble() /*x*/,
                                  w_listDbl.back().toDouble() /*variable*/});
  } // while-loop

  QVector<double> w_xCoord1;
  w_xCoord1.reserve(w_curvData.m_xVar1.size());
  std::transform(
      w_curvData.m_xVar1.cbegin(), w_curvData.m_xVar1.cend(),
      std::back_inserter(w_xCoord1),
      [](std::pair<double, double> aPairVal) { return aPairVal.first; });

  QVector<double> w_xCoord2;
  w_xCoord2.reserve(w_curvData.m_xVar2.size());
  std::transform(
      w_curvData.m_xVar2.cbegin(), w_curvData.m_xVar2.cend(),
      std::back_inserter(w_xCoord2),
      [](std::pair<double, double> aPairVal) { return aPairVal.first; });

  // sanity check??
  assert(w_xCoord1.size() == w_xCoord2.size());
  assert(w_xCoord1 == w_xCoord2); // make sense

  if (!w_curvData.m_twoCurveData2View.isEmpty()) {
    w_curvData.m_twoCurveData2View.clear();
  }
  w_curvData.m_twoCurveData2View.reserve(w_xCoord2.size());
  for (auto i = 0; i < w_curvData.m_xVar1.size(); ++i) // prepare dat to be view
  {
    // tuple of 4 element with the following format:
    // X|Var1|X|Var2 two-curves format
    w_curvData.m_twoCurveData2View.push_back(
        {std::tuple_cat(w_curvData.m_xVar1[i], w_curvData.m_xVar2[i])});
  }

  // acquisition and release resource (symetric )
  if (aFile1.isOpen())
    aFile1.close();

  if (aFile2.isOpen())
    aFile2.close();

  return w_curvData;
}

// Read full result file X:
//   ====================
//   Time: 0.0000 sec.
//   ====================
//
// X         H          U1             U2                 V
// 0.02    0.02         0.02          0.02              0.02
CurvesData2Show DataLoader::readDataFullResultFromFile(QFile &aFile2read) {
  bool w_bStartReadVal{false}; // initialize

  // maybe should use std vector because we have the shrinktofit
  // reduce memory usage (capacity)
  CurvesData2Show w_curvData;
  if (!w_curvData.m_vecTimeValues.isEmpty()) {
    w_curvData.m_vecTimeValues.clear(); // should be empty!!!
  }
  w_curvData.m_vecTimeValues.reserve(50); // for now will do

  if (aFile2read.open(QFile::ReadOnly)) { // open file for reading
    QTextStream w_fileStream{&aFile2read};
    // loop on line
    QString line;
    double w_timeVal{}; // time at each iteration

    // reading line after one
    while (!w_fileStream.atEnd()) {
      w_fileStream.readLineInto(&line);
      if (w_bStartReadVal) {
        auto w_vecOfTpl = // extract line values (vector of tuple)
            extractLineValuesTmplt<double, double, double, double, double>(
                w_fileStream, std::move(line));

        w_bStartReadVal = false; // according to the current format

        // add it to vector of pair pair<Time, values>
        w_curvData.m_vecTimeValues.push_back(
            {w_timeVal, std::move(w_vecOfTpl)});
        continue; // ready to continue to next Time: 0.0034 for example
      }

      if (line.isEmpty()) // check if line is empty
      {
        continue; // go to next line
      }
      auto w_lineTrimmed = line.trimmed();
      if (!w_lineTrimmed.contains('X') && w_bStartReadVal == false) {
        // Time: 0.0019 sec.   (file format)
        if (w_lineTrimmed.contains("Time")) {
          // split about ":"
          auto w_timeStr = w_lineTrimmed.split(QString{":"});
          w_timeStr.back().remove(
              QRegularExpression{"[sec]"}); // remove all ocurence of "sec"
          w_timeStr.back().remove(w_timeStr.back().size() - 1,
                                  1); // remove dot at the end (sec.)
          w_timeVal = w_timeStr.back().toDouble(); // contains value + sec.
        }
        continue; // next line
      } else {
        // flag to specify we start reading values
        w_bStartReadVal = true;
        continue;
      }
    } // while-loop
  }   // if

  // acquisition and release resource (symtric)
  if (aFile2read.isOpen())
    aFile2read.close();

  return w_curvData;
}

#if 0
// i do not think this is needed anymore
void DataLoader::readFiles2Cmp() {
  //m_bigEditor->append("You have selected a comparison graph");
  CurvesData2Show w_curvData2Show;

  auto w_simFileName1 = QFileDialog::getOpenFileNames(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  // read format 2-columns with first column x-coordinate
  // and second column variable
  m_graph2D.m_file1.setFileName(w_simFileName1);
  // readDataFromFile(m_graph2D.m_file1);

  auto w_simFileName2 = QFileDialog::getOpenFileName(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  // read format 2-columns with first column x-coordinate
  // and second column variable
  m_graph2D.m_file1.setFileName(w_simFileName2);

//  m_bigEditor->append("You have selected the following file (first): " +
//                      m_graph2D.m_file1.fileName());
//  m_bigEditor->append("You have selected the following file (first): " +
//                      m_graph2D.m_file2.fileName());
}
#endif
} // End namespace qplot
