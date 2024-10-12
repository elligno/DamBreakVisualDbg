#include "dambreakvisualdbg.h"
#include "ui_dambreakvisualdbg.h"
// QT include
#include <QFileInfo>
// Boost include
#include <boost/range/adaptors.hpp>
// App include
#include "VisualDbgUtilities.hpp"
// App includes
//#include "GraphVisualizer.h"
#include "DataLoader.h"
// lib API
#include "Utility/dbpp_AppConstant.hpp"

#if 0 // original code
DamBreakVisualDbg::DamBreakVisualDbg(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DamBreakVisualDbg)
{
    ui->setupUi(this);
}

DamBreakVisualDbg::~DamBreakVisualDbg()
{
    delete ui;
}
#endif

// C++ includes
#include <fstream>
#include <iostream>
#include <iterator>
//#include <istream>
#include <array>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <thread>
// Qt includes
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
// QPlot library
#include "QCustomplot.h"

namespace qplot {
// some implementation
DamBreakVisualDbg::DamBreakVisualDbg(QWidget *parent /*= 0*/)
    : QMainWindow(parent), m_yAxisLabel{QString{"H (Water Depth)"}},
      m_yAxisRng{0., 12.},   // default H-value with E. McNeil data
      m_xAxisRng{0., 1200.}, // default H-value with E. McNeil data
                             // m_dataType{ eDataType::EMcNeil },
      m_plot2d{nullptr}, m_centralWidget{new QWidget}, m_bigEditor{nullptr},
      m_fileFmtItem{nullptr}, m_graphFmtItem{nullptr}, m_stepGraphCounter{} {

  m_centralWidget->setLayout(singleLayout()); // prototyping
  setGeometry(30, 50, 1200, 800);             // layout geometry size
  // set window title of main window
  setWindowTitle(QObject::tr("DamBreak++ Visual Debugger"));
  // takes ownership of the widget pointer and deletes it
  setCentralWidget(m_centralWidget);
}

DamBreakVisualDbg::~DamBreakVisualDbg() {
  if (m_plot2d) {
    delete m_plot2d;
    m_plot2d = nullptr;
  }
}

// variables layout
// X         H          U1(A)             U2(Q)           V
void DamBreakVisualDbg::extractProfileFromFullRes(int aListIndex) {

  // reserve memory for storing values, here we use the bracket
  // operator [i] not push_back, expect memory allocated
  auto i = 0; // init
  if (!m_curvData.m_xVar1.isEmpty()) {
    m_curvData.m_xVar1.clear();
  }
  m_curvData.m_xVar1.reserve(dbpp::EMCNEILNbSections::value);

  // sanity check
  // auto tplSiz = std::tuple_size_v<dbltpl>;
  assert(5 == std::tuple_size_v<dbltpl>); // 5 columns 0 to 4 format

  // graph format set in loadFromFile()
  // m_vecTimeValues vector of pair (time value, tuple(variable values))
  switch (m_graph2D.m_graphFmt) {
  case eGraphFmt::XH:
    // copy values in m_xVar
    {
      // Simulation data from full result file format
      // current version contains 5 colums: X|H|A|Q|V
      for (auto &val : m_curvData.m_vecTimeValues[aListIndex].second) {
        // auto tplSiz = std::tuple_size_v<dbltpl>;
        // if (5 == tplSiz) { // 5 columns 0 to 4 format
        // re-ordering tuple index, since when parsing file it
        // returns the line element in reverse order (end to begin)
        dbltpl w_revertOrder = tuple_inverse_idx_order<dbltpl, 4, 3, 2, 1, 0>(
            val, std::index_sequence<4, 3, 2, 1, 0>{});
        // swap tuple element
        val.swap(w_revertOrder);
        //}  if

        double x{};
        double h{};
        // fill the plot2D data profile container
        std::tie(x, h, std::ignore, std::ignore, std::ignore) = val;
        m_curvData.m_xVar1.push_back({x, h});
        ++i; // next
      }
      break;
    }
  case eGraphFmt::XQ: {
    // Simulation data from full result file format
    // current version contains 5 colums: X|H|A|Q|V
    for (auto &val : m_curvData.m_vecTimeValues[aListIndex].second) {
      // re-ordering tuple index, since when parsing file it
      // returns the line element in reverse order (end to begin)
      dbltpl w_revertOrder = tuple_inverse_idx_order<dbltpl, 4, 3, 2, 1, 0>(
          val, std::index_sequence<4, 3, 2, 1, 0>{});
      // swap tuple element now bring it back from begin to end
      val.swap(w_revertOrder);

      double x{};
      double q{};
      // fill the plot2D data profile container
      std::tie(x, std::ignore, std::ignore, q, std::ignore) = val;
      m_curvData.m_xVar1.push_back({x, q});
      ++i; // next
    }
    break;
  }
  case eGraphFmt::XV:
    // Simulation data from full result file format
    // current version contains 5 colums: X|H|A|Q|V
    for (auto &val : m_curvData.m_vecTimeValues[aListIndex].second) {
      // re-ordering tuple index, since when parsing file it
      // returns the line element in reverse order (end to begin)
      dbltpl w_revertOrder = tuple_inverse_idx_order<dbltpl, 4, 3, 2, 1, 0>(
          val, std::index_sequence<4, 3, 2, 1, 0>{});
      // swap tuple element now bring it back from begin to end
      val.swap(w_revertOrder);

      double x{};
      double v{};
      // fill the plot2D data profile container
      std::tie(x, std::ignore, std::ignore, std::ignore, v) = val;
      m_curvData.m_xVar1.push_back({x, v});
      ++i; // next
    }
    break;
  default:
    break;
  }
}

void DamBreakVisualDbg::plot2dButton() {
  // switch case to handle different case such as: final profile/X/H
  if (eGraphFmt::XH == m_graph2D.m_graphFmt &&
      eFileFormat::var_profile == m_graph2D.m_filFmt) // this is the default??
  {
    // for this case we want to plot in the main window not in separate
    plot2DProfile(m_graph2D.m_graphFmt);
  } else if (eGraphFmt::XH == m_graph2D.m_graphFmt &&
             eFileFormat::full_result == m_graph2D.m_filFmt) {
    plot2DProfile(m_graph2D.m_graphFmt);
  } else if (eGraphFmt::XQ == m_graph2D.m_graphFmt &&
             eFileFormat::full_result == m_graph2D.m_filFmt) {
    plot2DProfile(m_graph2D.m_graphFmt);
  } else if (eGraphFmt::XV == m_graph2D.m_graphFmt &&
             eFileFormat::full_result == m_graph2D.m_filFmt) {
    plot2DProfile(m_graph2D.m_graphFmt);
  } else if ((eGraphFmt::XQ == m_graph2D.m_graphFmt ||
              eGraphFmt::XH == m_graph2D.m_graphFmt) &&
             eFileFormat::dbgFormat == m_graph2D.m_filFmt) {
    // just prototyping
    GraphSettingsPrm w_graphSetPrms;
    w_graphSetPrms.m_graphName = "TestGraph";
    w_graphSetPrms.m_varProfile = eVariableProfile::XH;
    w_graphSetPrms.m_qvecX =
        w_graph2Plot.generateXcoord(eDataTypes::EMcNeilDataType);
    w_graphSetPrms.m_graphType = eGraphType::multiple;
    w_graphSetPrms.m_xaxisLabel = QString{"x-coordinate"};

    // user selection from GUI
    if (m_graph2D.m_graphFmt == eGraphFmt::XQ) {
      w_graphSetPrms.m_varProfile = eVariableProfile::XQ;
      w_graphSetPrms.m_yaxisLabel = QString{"Q (Water Discharge)"};
      // just testing (preprocessing stuff) under construction
      w_graphSetPrms.m_yMaxValue = *std::max_element(
          m_curvData.m_mapValU[m_iterNo[0]->value()].second.begin(),
          m_curvData.m_mapValU[m_iterNo[0]->value()].second.end());

      // default Q-value with E. McNeil data
      w_graphSetPrms.m_yAxisRng = {0., w_graphSetPrms.m_yMaxValue + 1.75};
      w_graphSetPrms.m_xAxisRng = {0., 1010.};
    } else { // XH
      w_graphSetPrms.m_varProfile = eVariableProfile::XH;
      w_graphSetPrms.m_yaxisLabel = QString{"H (Water Depth)"};
      w_graphSetPrms.m_yMaxValue = *std::max_element(
          m_curvData.m_mapValU[m_iterNo[0]->value()].first.begin(),
          m_curvData.m_mapValU[m_iterNo[0]->value()].first.end());

      // default H-value with E. McNeil data
      w_graphSetPrms.m_yAxisRng = {0., w_graphSetPrms.m_yMaxValue + 0.25};
      w_graphSetPrms.m_xAxisRng = m_curvData.m_EMcNeilXRng; //{0., 1010.};
    }

    // check if any plottable
    //  if (w_graph2Plot.hasAnyPlottable())
    w_graph2Plot.clearPlottable();

    // loop on the number of graph
    for (auto i = 0; i < nbGraph; ++i) {
      if (m_activate[i]->isChecked()) {
        w_graph2Plot.AddGraph(w_graphSetPrms.m_graphType);
        if (w_graphSetPrms.m_varProfile == eVariableProfile::XH) {
          w_graph2Plot.setDbgData( // H-profile
              w_graphSetPrms.m_qvecX,
              m_curvData.m_mapValU[m_iterNo[i]->value()].first);
        } else {
          w_graph2Plot.setDbgData( // Q-profile
              w_graphSetPrms.m_qvecX,
              m_curvData.m_mapValU[m_iterNo[i]->value()].second);
        }
      }
    } // for-loop

    // ready to plot 2D profile
    w_graph2Plot.setPlotParams(w_graphSetPrms);
    w_graph2Plot.plot2DProfile();

    // debug format (this format we plot in separate window not in the main)
    // setMultipleGraph();
  } else if (eGraphFmt::XH == m_graph2D.m_graphFmt &&
             eFileFormat::exact_computed == m_graph2D.m_filFmt) {
    // setMultipleGraph();
    // just prototyping
    GraphSettingsPrm w_graphSetPrms;
    w_graphSetPrms.m_graphName = "TestGraph";
    w_graphSetPrms.m_varProfile = eVariableProfile::XH;
    w_graphSetPrms.m_qvecX =
        w_graph2Plot.generateXcoord(eDataTypes::EMcNeilDataType);
    w_graphSetPrms.m_graphType = eGraphType::compare;
    w_graphSetPrms.m_varProfile = eVariableProfile::XH;
    w_graphSetPrms.m_yaxisLabel = QString{"H (Water Depth)"};

    // avoid calling begin on temporary
    auto w_xvar1 = m_curvData.getxVar1().second;
    const auto w_yMaxValue1 = *std::max_element(w_xvar1.begin(), w_xvar1.end());
    // ditto
    auto w_xvar2 = m_curvData.getxVar2().second;
    const auto w_yMaxValue2 = *std::max_element(w_xvar2.begin(), w_xvar2.end());

    // take the biggest one
    w_graphSetPrms.m_yMaxValue =
        (w_yMaxValue1 > w_yMaxValue2) ? w_yMaxValue1 : w_yMaxValue2;

    // default H-value with Hudson data
    w_graphSetPrms.m_yAxisRng = m_curvData.m_HudsonYRng; //{0.45, 1.1};
    // m_xAxisRng {0., 1.};     temporary fix};
    w_graphSetPrms.m_xAxisRng = m_curvData.m_HudsonXRng; //{0., 1.};

    w_graphSetPrms.m_xaxisLabel = QString{"x-coordinate"};

    // Only 2 graphs since compare exact/computed
    w_graph2Plot.AddGraph(w_graphSetPrms.m_graphType);
    w_graph2Plot.setCmpData(m_curvData.getxVar1());
    w_graph2Plot.AddGraph(w_graphSetPrms.m_graphType);
    w_graph2Plot.setCmpData(m_curvData.getxVar2());

    // ready to plot 2D profile
    w_graph2Plot.setPlotParams(w_graphSetPrms);
    w_graph2Plot.plot2DProfile();
  }
}

void DamBreakVisualDbg::saveToFile() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Graph Image"), "",
      tr("Simulation Image (*.jpg);;All Files (*)"));

  // if empty it means we are not in multiple graph or compare
  auto w_graphInWIndow = w_graph2Plot.customPlot()->graphCount();

  if (!fileName.isEmpty()) // set default name as ...
  {
    if (w_graphInWIndow != 0) {
      w_graph2Plot.customPlot()->saveJpg(fileName, 640, 400);
    } else {
      m_plot2d->saveJpg(fileName, /*+ QString(".jpg"),*/ 640, 400);
    }
  } else {
    if (w_graphInWIndow != 0) {
      // name set from GUI ...
      w_graph2Plot.customPlot()->saveJpg(
          QString{"DefaultGraphName"} /*+ QString(".jpg")*/, 640, 400);
    } else {
      m_plot2d->saveJpg(m_graph2D.m_graphName /*+ QString(".jpg")*/, 640, 400);
    }
  }
}

void qplot::DamBreakVisualDbg::loadDebugAndSetFmt() {
  auto w_simFileName = QFileDialog::getOpenFileName(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  QFile w_file2Load(w_simFileName);
  std::optional<QFile> w_no2ndFile{std::nullopt};
  DataLoader w_dat2Load;
  m_curvData = // data to display
      w_dat2Load.loadData2Show(w_file2Load, w_no2ndFile,
                               eFileResultFormat::dbgFormat);

  // set ... why we need file format to show data??
  m_graph2D.m_filFmt = eFileFormat::dbgFormat;

  //   auto check = m_graphFmtItem->currentText();
  if (m_graphFmtItem->currentText() ==
      "X/H") // debug: X/H (flat bed==X/A) and X/Q
  {
    m_graph2D.m_graphFmt = eGraphFmt::XH;
  } else if (m_graphFmtItem->currentText() == "X/Q") {
    m_graph2D.m_graphFmt = eGraphFmt::XQ;
  }
}

void qplot::DamBreakVisualDbg::loadFinalProfileAndSetFmt() {
  auto w_simFileName = QFileDialog::getOpenFileName(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  m_bigEditor->append("You have selected the following file: " + w_simFileName);

  QFile w_file2Load(w_simFileName);
  std::optional<QFile> w_no2ndFile{std::nullopt};
  DataLoader w_dat2Load;
  m_curvData = // data to display
      w_dat2Load.loadData2Show(w_file2Load, w_no2ndFile,
                               eFileResultFormat::var_profile);

  m_graph2D.m_filFmt = eFileFormat::var_profile; // X/H is default one

  // the only supported in the current version
  if (m_graphFmtItem->currentText() ==
      "X/H") // debug: X/H (flat bed==X/A) and X/Q
  {
    m_graph2D.m_graphFmt = eGraphFmt::XH;
  }
}

void qplot::DamBreakVisualDbg::loadFullResultAndSetFmt() {
  auto w_simFileName = QFileDialog::getOpenFileName(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  m_bigEditor->append("You have selected the following file: " + w_simFileName);

  QFile w_file2Load(w_simFileName);
  std::optional<QFile> w_no2ndFile{std::nullopt};
  DataLoader w_dat2Load;
  m_curvData = // data to display
      w_dat2Load.loadData2Show(w_file2Load, w_no2ndFile,
                               eFileResultFormat::full_result);

  m_simTimesList = m_curvData.getTimesFromTimeValuesVec();

  // GUI drop-down box that list all times
  // user select a time to visualize simulation result
  m_simTimesItem->addItems(m_simTimesList);

  m_graph2D.m_filFmt = eFileFormat::full_result;

  //   auto check = m_graphFmtItem->currentText();
  if (m_graphFmtItem->currentText() == "X/H") {
    m_graph2D.m_graphFmt = eGraphFmt::XH;
  } else if (m_graphFmtItem->currentText() == "X/Q") {
    m_graph2D.m_graphFmt = eGraphFmt::XQ;
  } else if (m_graphFmtItem->currentText() == "X/V") {
    m_graph2D.m_graphFmt = eGraphFmt::XV;
  }
}

void qplot::DamBreakVisualDbg::loadExactCompAndSetFmt() {
  auto w_simFileNames = QFileDialog::getOpenFileNames(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  m_bigEditor->append("You have selected the following file: " +
                      w_simFileNames[0] + " " + w_simFileNames[1]);

  QFile w_file1(w_simFileNames[0]);
  std::optional<QFile> w_file2(w_simFileNames[1]);
  DataLoader w_dataLoadr;
  m_curvData = w_dataLoadr.loadData2Show(w_file1, w_file2,
                                         eFileResultFormat::exact_computed);

  m_graph2D.m_filFmt = eFileFormat::exact_computed;
}

void DamBreakVisualDbg::loadFromFile() {
  m_bigEditor->append("Starting to load data...");

  m_bigEditor->append(" File format selected is: " +
                      m_fileFmtItem->currentText()); // this is the default one

  if (m_fileFmtItem->currentText() == "Debug") {
    // Open file to read data and set graph fmt
    loadDebugAndSetFmt();
  } else if (m_fileFmtItem->currentText() ==
             "Final Profile") { // variable profile
    // Open file to read data
    loadFinalProfileAndSetFmt();
  } else if (m_fileFmtItem->currentText() == "Full Result") {
    // Open file to read data
    loadFullResultAndSetFmt();
  } else if (m_fileFmtItem->currentText() == "Exact/Computed") {
    // list if files name (user selection)
    loadExactCompAndSetFmt();
  } else { // for some reason no file was loaded
    m_bigEditor->append(
        QString("Couldn't load any file, something went wrong"));
  }

  m_bigEditor->append(QString("Finished to load data"));
  m_bigEditor->append(
      QString("Select the graph format to plot in the menu bar below"));
  m_bigEditor->append(QString{"Finished preparing data to view"});
  m_bigEditor->append(QString("Once you finished with graph config, click Ok "
                              "to validate your selection"));
}

void DamBreakVisualDbg::setAllLineTypeComboBox() {
  for (auto i = 0; i < nbGraph; ++i) {
    QListView *w_listView = new QListView(m_colorCurve[i]);
    if (!m_colorCurve[i]->isEditable()) {
      // by setting editable, when user change the value
      // can be retrieved with method currentText().
      m_colorCurve[i]->setEditable(true);
    }

    // graph #i
    m_colorCurve[i]->addItem("red");
    m_colorCurve[i]->addItem("blue");
    m_colorCurve[i]->addItem("green");
    m_colorCurve[i]->addItem("magenta");
    m_colorCurve[i]->addItem("cyan");

    w_listView->setStyleSheet("QListView::item {               \
                                 border-bottom: 5px solid white; margin:3px; }  \
                                 QListView::item:selected {                     \
                                 border-bottom: 5px solid black; margin:3px;    \
                                 color: black;                                  \
                                 }                                              \
                                 ");

    // wrap it up!
    m_colorCurve[i]->setView(w_listView); // takes the ownership of the view
                                          // (responsible to delete w_listView1)
  }                                       // for-loop
}

QGridLayout *DamBreakVisualDbg::singleLayout() {
  // what for??
  std::initializer_list<std::string> w_listGraphNo{
      std::string{"Graph #1"}, std::string{"Graph #2"}, std::string{"Graph #3"},
      std::string{"Graph #4"}, std::string{"Graph #5"}};

  // Main layout prototype
  QGridLayout *w_mainGridLayout = new QGridLayout;

  // Layout policy (horizontally adjustable policy)
  QSizePolicy w_firstColumnSizPolicy;
  w_firstColumnSizPolicy.setHorizontalPolicy(QSizePolicy::Policy::Expanding);

  QPushButton *w_plotButton = new QPushButton(tr("Plot Graph"));
  w_plotButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_plotButton, SIGNAL(clicked()), this, SLOT(plot2dButton()));

  QPushButton *w_closeButton = new QPushButton(tr("Close App"));
  w_closeButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QPushButton *w_saveButton = new QPushButton(tr("Save Graph"));
  w_saveButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_saveButton, SIGNAL(clicked()), this, SLOT(saveToFile()));

  QPushButton *w_loadButton = new QPushButton(tr("Load File"));
  w_loadButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_loadButton, SIGNAL(clicked()), this, SLOT(loadFromFile()));

  QPushButton *w_stepButton = new QPushButton(tr("Plot-by-Step"));
  w_stepButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_stepButton, SIGNAL(clicked()), this,
                   SLOT(plot2DProfile()));

  // first column layout buttons and Combo Box
  QVBoxLayout *w_dropDownFileFmt = new QVBoxLayout;
  QLabel *w_fileFmtLabel = new QLabel{tr("File Format")};
  m_fileFmtItem = new QComboBox;
  m_fileFmtItem->addItem(QString{"Debug"});
  m_fileFmtItem->addItem(QString{"Final Profile"});
  m_fileFmtItem->addItem(QString{"Full Result"});
  m_fileFmtItem->addItem(QString{"Exact/Computed"});
  m_fileFmtItem->setEditable(false); // can't be edited by user
  w_dropDownFileFmt->addWidget(w_fileFmtLabel);
  w_dropDownFileFmt->addWidget(m_fileFmtItem);
  //  QObject::connect(m_fileFmtItem, SIGNAL(currentTextChanged(QString)),
  //  this,
  //                   SLOT(getFileFmt(QString))); next version

  QVBoxLayout *w_dropDownGraphFmt = new QVBoxLayout;
  QLabel *w_graphFmtLabel = new QLabel{tr("Graph Format")};
  m_graphFmtItem = new QComboBox;
  m_graphFmtItem->addItem(QString{"X/H"});
  m_graphFmtItem->addItem(QString{"X/Q"});
  m_graphFmtItem->addItem(QString{"X/V"});
  m_graphFmtItem->setEditable(false); // can't be edited by user
  w_dropDownGraphFmt->addWidget(w_graphFmtLabel);
  w_dropDownGraphFmt->addWidget(m_graphFmtItem);
  QObject::connect(m_graphFmtItem, SIGNAL(currentTextChanged(QString)), this,
                   SLOT(getGraphFmt(QString)));

  // NOTE not adding item, will be added in the extractProfileFromFullRes()
  QVBoxLayout *w_dropDownSimTimes = new QVBoxLayout;
  QLabel *w_simTimesLabel = new QLabel{tr("Simulation Time")};
  m_simTimesItem = new QComboBox;
  QObject::connect(m_simTimesItem, SIGNAL(currentTextChanged(const QString &)),
                   this, SLOT(getProfileTimes(const QString &)));
  m_simTimesItem->setEditable(false);
  w_dropDownSimTimes->addWidget(
      w_simTimesLabel); // takes the ownership of the widget
  w_dropDownSimTimes->addWidget(
      m_simTimesItem); // takes the ownership of the widget

  QVBoxLayout *w_firstColumn = new QVBoxLayout;
  w_firstColumn->setDirection(QBoxLayout::Direction::TopToBottom);
  w_firstColumn->addWidget(
      w_plotButton); // take the ownership (layout responsible to delete it)
  w_firstColumn->addWidget(w_saveButton);       // ...
  w_firstColumn->addWidget(w_loadButton);       // ...
  w_firstColumn->addWidget(w_closeButton);      // ...
  w_firstColumn->addWidget(w_stepButton);       // ...
  w_firstColumn->addLayout(w_dropDownFileFmt);  // take the ownership (layout
                                                // responsible to delete it)
  w_firstColumn->addLayout(w_dropDownGraphFmt); // take the ownership (layout
                                                // responsible to delete it)
  w_firstColumn->addLayout(w_dropDownSimTimes); // take the ownership (layout
                                                // responsible to delete it)
  w_firstColumn->insertSpacerItem(1, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(3, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(5, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(7, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(9, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(11, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(13, new QSpacerItem(30, 20));
  w_firstColumn->addStretch();

  QVBoxLayout *w_secondLayout = new QVBoxLayout(/*w_plotFrame*/);

  // plotter
  m_plot2d = new QCustomPlot;

  // 		QSizePolicy w_plotGrapPolicy;
  // 		w_plotGrapPolicy.setHorizontalStretch(50);
  // 		m_plot2d->setSizePolicy(w_plotGrapPolicy);

  m_plot2d->xAxis->setLabel("X Coordinate");
  m_plot2d->yAxis->setLabel("H (Water Depth)");

  // set axes ranges, so we see all data: (default values E. McNeil)
  m_plot2d->xAxis->setRange(m_curvData.m_EMcNeilXRng.first,
                            m_curvData.m_EMcNeilXRng.second); // x-coordinate
  m_plot2d->yAxis->setRange(m_curvData.m_EMcNeilYRng.first,
                            m_curvData.m_EMcNeilYRng.second); // H-value
  m_plot2d->addGraph();

  // adding QCustomPlot plot to layout
  w_secondLayout->addWidget(m_plot2d); // take the ownership of the widget plot

  m_bigEditor = new QTextEdit;
  m_bigEditor->setOverwriteMode(false);
  m_bigEditor->setText("Ready to plot graph from simulation data");
  m_bigEditor->append("Press Load Button to proceed and select file");
  QVBoxLayout *w_textEdit = new QVBoxLayout;
  // m_bigEditor->setMaximumSize(QSize(1.9 * m_centralWidget->width(), 150));
  w_textEdit->addWidget(m_bigEditor);

  auto w_hboxLayout = setAllHboxLayout(w_listGraphNo);
  setAllColorComboBox();
  setAllLineTypeComboBox();

  // adding all layout to the main layout
  w_mainGridLayout->addLayout(w_firstColumn, 0, 0);
  w_mainGridLayout->addLayout(w_secondLayout, 0, 1);
  w_mainGridLayout->addLayout(w_textEdit, 1, 1);
  w_mainGridLayout->addLayout(w_hboxLayout[0], 2, 1);
  w_mainGridLayout->addLayout(w_hboxLayout[1], 3, 1);
  w_mainGridLayout->addLayout(w_hboxLayout[2], 4, 1);
  w_mainGridLayout->addLayout(w_hboxLayout[3], 5, 1);
  w_mainGridLayout->addLayout(w_hboxLayout[4], 6, 1);

  return w_mainGridLayout;
}

std::array<QHBoxLayout *, 5> DamBreakVisualDbg::setAllHboxLayout(
    std::initializer_list<std::string> aListGraphName) {

  // some initialization
  auto w_listGraphName = aListGraphName.begin();
  // QHBoxLayout* w_allBoxLayout[5] { nullptr, nullptr, nullptr, nullptr,
  // nullptr };
  std::array<QHBoxLayout *, 5> w_hboxLayoutArray{nullptr, nullptr, nullptr,
                                                 nullptr, nullptr};
  for (auto i = 0; i < nbGraph; ++i) {
    w_hboxLayoutArray[i] = new QHBoxLayout;

    QLabel *w_GraphLabel = new QLabel(tr("Graph Name"));
    m_graphName[i] = new QLineEdit(QString(*w_listGraphName->c_str())); // ???
    QHBoxLayout *w_firstBox = new QHBoxLayout;
    w_firstBox->addWidget(w_GraphLabel);
    w_firstBox->addWidget(m_graphName[i]);
    w_hboxLayoutArray[i]->addLayout(w_firstBox);

    QLabel *w_iterLabel = new QLabel(tr("Iteration No"));
    m_iterNo[i] = new QSpinBox; // setting the iteration no for data
    QHBoxLayout *w_secondBox = new QHBoxLayout;
    w_secondBox->addWidget(w_iterLabel);
    w_secondBox->addWidget(m_iterNo[i]);
    w_hboxLayoutArray[i]->addLayout(w_secondBox);

    QLabel *w_checkLabel = new QLabel(tr("Graph Active"));
    m_activate[i] = new QCheckBox;
    QHBoxLayout *w_thirdBox = new QHBoxLayout;
    w_thirdBox->addWidget(w_checkLabel);
    w_thirdBox->addWidget(m_activate[i]);
    w_hboxLayoutArray[i]->addLayout(w_thirdBox);

    QLabel *w_typeCurve1Label = new QLabel(tr("Curve Type"));
    m_lineType[i] = new QComboBox;
    QHBoxLayout *w_fourthBox = new QHBoxLayout;
    w_fourthBox->addWidget(w_typeCurve1Label);
    w_fourthBox->addWidget(m_lineType[i]);
    w_fourthBox->minimumSize();
    w_hboxLayoutArray[i]->addLayout(w_fourthBox);

    QLabel *w_curveLabel = new QLabel(tr("Select Color"));
    m_colorCurve[i] = new QComboBox;
    QHBoxLayout *w_fifthBox = new QHBoxLayout;
    w_fifthBox->addWidget(w_curveLabel);
    w_fifthBox->addWidget(m_colorCurve[i]);
    w_fifthBox->minimumSize();
    w_hboxLayoutArray[i]->addLayout(w_fifthBox);

    w_hboxLayoutArray[i]->insertSpacerItem(1, new QSpacerItem{150, 10});
    w_hboxLayoutArray[i]->insertSpacerItem(3, new QSpacerItem{75, 10});
    w_hboxLayoutArray[i]->insertSpacerItem(5, new QSpacerItem{75, 10});
    w_hboxLayoutArray[i]->insertSpacerItem(7, new QSpacerItem{75, 10});

    ++w_listGraphName;
  } // for-loop

  return w_hboxLayoutArray;
}

// Plot profile variable
void DamBreakVisualDbg::plot2DProfile(const eGraphFmt aGraphFmt) {
  // set data (index set 0 by default) shall we call addGraph()??
  QPen w_pen1;
  w_pen1.setStyle(Qt::SolidLine);
  w_pen1.setWidth(3);
  w_pen1.setColor(QColor(Qt::darkBlue));
  m_plot2d->graph()->setPen(w_pen1);
  // m_plot2d->graph()->setPen(QColor(Qt::darkRed)); //
  m_plot2d->xAxis->setLabel("X Coordinate");
  m_plot2d->graph()->setLineStyle(
      QCPGraph::lsLine); // lsline(each point attached by line), lsnone (no
                         // point attached similar to dot)
  if (eFileFormat::var_profile == m_graph2D.m_filFmt ||
      eFileFormat::full_result == m_graph2D.m_filFmt) {
    if (eGraphFmt::XH == aGraphFmt) // under construction
    {
      // m_plot2d->xAxis->setLabel("X Coordinate");
      m_plot2d->yAxis->setLabel("H (Water Depth)");
      m_plot2d->xAxis->setRange(0., 1.);
      m_plot2d->yAxis->setRange(0.43, 1.1);
    } else if (eGraphFmt::XQ == aGraphFmt) {
      // m_plot2d->xAxis->setLabel("X Coordinate");
      m_plot2d->yAxis->setLabel("Q (Discharge)");
      m_plot2d->xAxis->setRange(0.3, 0.7);
      m_plot2d->yAxis->setRange(0., 0.8);
    } else if (eGraphFmt::XV == aGraphFmt) {
      // m_plot2d->xAxis->setLabel("X Coordinate");
      m_plot2d->yAxis->setLabel("V (Velocity)");
      m_plot2d->xAxis->setRange(0.3, 0.7);
      m_plot2d->yAxis->setRange(0., 0.8);
    } else {
      m_bigEditor->append("You have selected wrong graph format");
    }

    // Design Note
    //  this is the main window (setdata()) with different data from user
    // auto w_profileData = m_graph2D.m_xVar1;   QVector of double, need to
    // extract
    QVector<double> w_xCoord;
    w_xCoord.reserve(m_curvData.m_xVar1.size());
    std::transform(
        m_curvData.m_xVar1.cbegin(), m_curvData.m_xVar1.cend(),
        std::back_inserter(w_xCoord),
        [](std::pair<double, double> aPairVal) { return aPairVal.first; });

    QVector<double> w_varValues;
    w_varValues.reserve(m_curvData.m_xVar1.size());
    std::transform(
        m_curvData.m_xVar1.cbegin(), m_curvData.m_xVar1.cend(),
        std::back_inserter(w_varValues),
        [](std::pair<double, double> aPairVal) { return aPairVal.second; });

    // find max element in both axes
    const auto w_xmax = std::max_element(w_xCoord.cbegin(), w_xCoord.cend());
    const auto w_valmax =
        std::max_element(w_varValues.cbegin(), w_varValues.cend());
    const auto w_valmin =
        std::min_element(w_varValues.cbegin(), w_varValues.cend());

    // just a test, already set in SingleLayout
    m_plot2d->xAxis->setRange(0. /*min*/, *w_xmax /*max*/);
    // x-coordinate
    m_plot2d->yAxis->setRange(*w_valmin /*min*/, *w_valmax);
    //    *w_valmax/*max*/);   //

    // NOTE
    //  for the current version all hard coded, fix in future version
    //  now put all effort on the a first version of the plotting feature
    // set axes ranges, so we see all data:
    // m_plot2d->xAxis->setRange(0., 1.);
    // m_plot2d->yAxis->setRange(0.43, 1.1);

    // ready to plot
    m_plot2d->graph()->rescaleAxes();
    m_plot2d->graph()->setData(w_xCoord, w_varValues);
  }

  // call replot() it sure is!!
  m_plot2d->replot();
}

// return a lambda (copy of a variable) can modify
// local variable safely
std::function<int(int)> make_offseter(int aOffset) {
  return [=](int j) { return aOffset + j; };
}

// Eventually, user should be able to set the step value (default=5)
// DESIGN NOTE display in the main window
// IMPORTANT use only for Debug format result file
// user have selected 'File Format' = Debug contains values
// at mid step and final step in a 2-columns format (A|Q)
// This version support (display) only values of the final step.
void DamBreakVisualDbg::plot2DProfile() // m_plot2d window (SLOT)
{
  // set first iteration at 5 (returns a lambda)
  std::function<int(int)> offset_5 = make_offseter(m_graph2D.iterationStep);

  m_bigEditor->append(
      "******* Wave Profile at Iteration: " +
      QString(std::to_string(offset_5(m_stepGraphCounter)).c_str()));

  m_plot2d->xAxis->setLabel("X Coordinate");

  // NOTE should set from user selection (GUI drop down box)
  // again for debugging purpose (maybe a refactoring is needed)
  // generateXcoord(eDataType::EMcNeil);

  // GraphVisualizer version
  auto w_qvecX = w_graph2Plot.generateXcoord(eDataTypes::EMcNeilDataType);

  // NOTE using lambda (offset_5) to modified the step counter variable
  // returns a copy of the incremented value without modifying original
  // m_stepGraphCounter is not modified, return a copy of the modified
  const auto w_maxH = std::max_element( // copy of udated variable
      m_curvData.m_mapValU[offset_5(m_stepGraphCounter)].first.cbegin(),
      m_curvData.m_mapValU[offset_5(m_stepGraphCounter)].first.cend());

  const auto w_maxQ = std::max_element(
      m_curvData.m_mapValU[offset_5(m_stepGraphCounter)].second.cbegin(),
      m_curvData.m_mapValU[offset_5(m_stepGraphCounter)].second.cend());

  // debug file format and stepping mode
  if (eGraphFmt::XH == m_graph2D.m_graphFmt) {
    // Lambda capture local variable by value
    // Copies of local variable returned by the function (5 is the step value
    // by default) Access copies of local variables at the time the lambda was
    // created.
    //  std::function<int(int)> offset_5 =
    //  make_offseter(m_graph2D.iterationStep);

    // NOTE using lambda (offset_5) to modified the step counter variable
    // returns a copy of the incremented value without modifying original
    // m_stepGraphCounter is not modified, return a copy of the modified
    //    const auto w_maxH = std::max_element( // copy of udated variable
    //        m_curvData.m_mapValU[offset_5(m_stepGraphCounter)].first.cbegin(),
    //        m_curvData.m_mapValU[offset_5(m_stepGraphCounter)].first.cend());

    //  m_plot2d->xAxis->setLabel("X Coordinate");
    m_plot2d->yAxis->setLabel("H (Water Depth)");
    m_plot2d->xAxis->setRange(0., 1010.);   // x-coordinate (E. McNeil data)
    m_plot2d->yAxis->setRange(0., *w_maxH); // H-value

    // NOTE since the local variable by the lambda (working on a copy of
    // modified variable) changing m_stepGraphCounter, increment by 5 ready
    // for next iteration
    m_plot2d->graph()->setData(         // m_stepGraphCounter still
        w_qvecX,                        // x-coord
        QVector<double>::fromStdVector( // variable values
            m_curvData.m_mapValU[m_stepGraphCounter += m_graph2D.iterationStep]
                .first));
  } else if (eGraphFmt::XQ == m_graph2D.m_graphFmt) {
    // Copies of local variable returned by the function (5 is the step value
    // by default) Access copies of local variables at the time the lambda was
    // created.
    // std::function<int(int)> offset_5 =
    // make_offseter(m_graph2D.iterationStep);

    //    const auto w_maxQ = std::max_element(
    //        m_graph2D.m_mapValU[offset_5(m_stepGraphCounter)].second.cbegin(),
    //        m_graph2D.m_mapValU[offset_5(m_stepGraphCounter)].second.cend());

    // set labels
    // m_plot2d->xAxis->setLabel("X Coordinate");
    m_plot2d->yAxis->setLabel("Q (Water Discharge)");

    // need to find maximum value
    m_plot2d->xAxis->setRange(0., 1010.);   // x-coordinate
    m_plot2d->yAxis->setRange(0., *w_maxQ); // Q-value

    // set data for plotting
    m_plot2d->graph()->setData(
        w_qvecX,                        // x-coord
        QVector<double>::fromStdVector( // variable values 'U2'
            m_curvData.m_mapValU[m_stepGraphCounter += 5].second));
  } else {
    QMessageBox w_msg;
    w_msg.setText("Graph Profile Not Supported. Select X/H or X/Q for this "
                  "type of graph");
    w_msg.exec();
  }

  // call replot() it sure is!!
  m_plot2d->replot();
}

void DamBreakVisualDbg::getFileFmt(QString aFileFmt) {
  if (aFileFmt == "Debug") {
    m_graph2D.m_filFmt = eFileFormat::dbgFormat;
  } else if (aFileFmt == "Final Profile") {
    m_graph2D.m_filFmt = eFileFormat::var_profile;
  } else if (aFileFmt == "Full Result") {
    m_graph2D.m_filFmt = eFileFormat::full_result;
  } else if (aFileFmt == "Exact/Computed") {
    m_graph2D.m_filFmt = eFileFormat::exact_computed;
  } else {
    m_bigEditor->append("Not supported file format");
  }
}

void DamBreakVisualDbg::getGraphFmt(QString aGraphFmt) {
  if (aGraphFmt == "X/H") {
    m_graph2D.m_graphFmt = eGraphFmt::XH;
    // re-initialize when graph format changed (initialy set to 0)
    if (m_stepGraphCounter != 0)
      m_stepGraphCounter = 0;
  } else if (aGraphFmt == "X/Q") {
    m_graph2D.m_graphFmt = eGraphFmt::XQ;
    // re-initialize when graph format changed (initialy set to 0)
    if (m_stepGraphCounter != 0)
      m_stepGraphCounter = 0;
  } else if (aGraphFmt == "X/V") {
    m_graph2D.m_graphFmt = eGraphFmt::XV;
  } else if (aGraphFmt == "Exact/Computed") {
    //   m_graph2D.m_graphFmt = eFileFormat::exact_computed;
  } else {
    m_bigEditor->append("Not supported graph format");
  }
}

void DamBreakVisualDbg::getProfileTimes(const QString &aTimeVal) {
  auto w_checkTimeIdx = m_simTimesItem->currentIndex();

  // QStringListIterator w_timeListIter{m_graph2D.m_simTimesList};
  QStringListIterator w_timeListIter{m_simTimesList};
  while (w_timeListIter.hasNext()) {
    if (w_timeListIter.next() == aTimeVal) {
      extractProfileFromFullRes(w_checkTimeIdx);
      break;
    }
  } // while-loop
}

void DamBreakVisualDbg::setAllColorComboBox() {
  for (auto i = 0; i < nbGraph; ++i) {
    QListView *w_lineTypeView = new QListView(m_lineType[i]);
    // add item to the ... to be completed
    m_lineType[i]->addItem(
        QString("Dash Line")); // lsimpulse?? no i don't think so
    m_lineType[i]->addItem(QString("Dot Line"));   // lsnone
    m_lineType[i]->addItem(QString("Solid Line")); // lsline

    // configure font for graph1
    w_lineTypeView->setStyleSheet("QListView::item {               \
                                       border-bottom: 5px solid white; margin:3px; }  \
                                       QListView::item:selected {                     \
                                       border-bottom: 5px solid black; margin:3px;    \
                                       color: black;                                  \
                                       }                                              \
                                       ");

    m_lineType[i]->setView(w_lineTypeView);
  }
}
} // namespace qplot
