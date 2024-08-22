#include "dambreakvisualdbg.h"
#include "ui_dambreakvisualdbg.h"

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
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <thread>
// boost includes
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
// Boost string algorithm
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/filesystem.hpp> // boost file system utility
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
// test includes
#include "GenNumber.hpp"

// QPlot library
#include "qcustomplot.h"

namespace {

#if 0
  template <typename T> T read( std::istringstream& is)
  {
    T t; is >> t; return t;
  }

  template <typename... Args>
  std::tuple<Args...> parse( std::istringstream& is)
  {
    return std::make_tuple(read<Args>(is) ...);
  }

  // Access copies of local variables at the time the lambda was created.
  // Every call to make_offseter returns a new lambda function object through
  // the std::function< > function wrapper. This is the safest form of local
  // variable capture.
  std::function<int(int)> make_offseter(int aOffset)
  {
    return [=](int j) { return aOffset + j; };
  }
#endif
} // namespace

namespace qplot {
// useful typedef (actually we don't really need it, just the "auto" keyword)
using mapValU =
    std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;
using mapValUp =
    std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;

bool FindFile(const boost::filesystem::path &directory,
              boost::filesystem::path &path, const std::string &filename);

void strAlgor4SimRes(mapValU &aMapU12p, mapValU &aMapU12,
                     const std::string &aFilename);

void PrepareSimRes2View(mapValU &aMapU12p, mapValU &aMapU12,
                        const std::string &aFilename);
} // namespace qplot

namespace qplot {
// some implementation
DamBreakVisualDbg::DamBreakVisualDbg(QWidget *parent /*= 0*/)
    : QMainWindow(parent), m_yAxisLabel{QString{"H (Water Depth)"}},
      m_yAxisRng{0., 12.},   // default H-value with E. McNeil data
      m_xAxisRng{0., 1200.}, // default H-value with E. McNeil data
                             // m_dataType{ eDataType::EMcNeil },
      m_customPlot{nullptr}, m_plot2d{nullptr}, m_centralWidget{new QWidget},
      m_bigEditor{nullptr}, m_fileFmtItem{nullptr}, m_graphFmtItem{nullptr} {

  m_centralWidget->setLayout(singleLayout()); // prototyping
  setGeometry(30, 50, 1200, 800);             // layout geometry size
  // set window title of main window
  setWindowTitle(QObject::tr("DamBreak++ Visual Debugger"));
  setCentralWidget(
      m_centralWidget); // takes ownership of the widget pointer and deletes it

  // mapToGlobal()
  // create plotter 2D from file data
#if 1
//     setPlotParams();
#endif
}

void DamBreakVisualDbg::setPlotParams(const QString &aYLabel, double aYmax) {
  m_yAxisLabel = aYLabel;
  m_yAxisRng.second = aYmax;

  try {
    // 2d plot (plot x-axis and y-axis)
    //     m_customPlot = new QCustomPlot;
    m_customPlot->setGeometry(
        200, 100, 640, 400); // window size when poping-up with offset in x/y
    m_customPlot->setWindowTitle(QString{"DamBreak Wave Propagation"});

    // first we create and prepare a text layout element:
    QCPTextElement *title = new QCPTextElement(m_customPlot);
    title->setText("HLL Riemann Solver");
    title->setFont(QFont("sans", 12, QFont::Bold));
    // then we add it to the main plot layout:
    m_customPlot->plotLayout()->insertRow(
        0); // insert an empty row above the axis rect
    m_customPlot->plotLayout()->addElement(
        0, 0, title); // place the title in the empty cell we've just created

    m_customPlot->xAxis->setLabel("X Coordinate");
    m_customPlot->yAxis->setLabel(m_yAxisLabel);
    // set axes ranges, so we see all data:
    m_customPlot->xAxis->setRange(m_xAxisRng.first /*min*/,
                                  m_xAxisRng.second /*max*/); // x-coordinate
    m_customPlot->yAxis->setRange(m_yAxisRng.first /*min*/,
                                  m_yAxisRng.second /*max*/); //

    // Try to add a legend to the graph
    m_customPlot->setLocale(
        QLocale(QLocale::English,
                QLocale::UnitedStates)); // period as decimal separator and
                                         // comma as thousand separator
    m_customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    m_customPlot->legend->setFont(legendFont);
    //	 m_customPlot->legend->setSelectedFont(legendFont);

    //	 m_customPlot->legend->setVisible(true);
    //	 QFont legendFont = font();  // start out with MainWindow's font..
    //	 legendFont.setPointSize(9); // and make a bit smaller for legend
    //	 customPlot->legend->setFont(legendFont);
    m_customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    // by default, the legend is in the inset layout of the main axis rect. So
    // this is how we access it to change legend placement:
    m_customPlot->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignTop | Qt::AlignRight);
    //		 m_customPlot->legend->addElement(0, 0, ...);
    // ui->customPlot->legend->setSelectableParts(QCPLegend::spItems);  legend
    // box

    // make top right axes clones of bottom left axes
    m_customPlot->axisRect()->setupFullAxesBox();
  } catch (std::bad_alloc &ba) {
    std::cerr << "Caught in a exception in TestQtPlot ctor " << ba.what()
              << "\n";
  }
}

DamBreakVisualDbg::~DamBreakVisualDbg() {
  if (m_customPlot) {
    delete m_customPlot;
    m_customPlot = nullptr;
  }
  if (m_plot2d) {
    delete m_plot2d;
    m_plot2d = nullptr;
  }
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
void DamBreakVisualDbg::readDataFromFile(QFile &aFile2read) {
  // sanity check
  if (!aFile2read.isOpen()) {
    if (!aFile2read.open(QIODevice::ReadOnly)) {
      QMessageBox::information(this, tr("Unable to open file"),
                               aFile2read.errorString());
    }
  }

  // some utility to read file
  QTextStream w_data2Read(&aFile2read);
  QString line;
  // if you don't do that, first time push_back is call
  // vector resize, not really efficient
  m_graph2D.m_xVar1.reserve(101); // debugging purpose

  // reading line after one
  while (w_data2Read.readLineInto(&line)) {
    if (line.isEmpty()) // check if line is empty
    {
      continue; // go to next line
    }

    // split about white space () QStringList
    auto w_listDbl = line.split(" ");
    // push in a map or vector (move semantic)
    m_graph2D.m_xVar1.push_back(
        {w_listDbl.front().toDouble(), w_listDbl.back().toDouble()});
  } // while
}

// Read full result file X:
//   ====================
//   Time: 0.0000 sec.
//   ====================
//
// X         H          U1             U2                 V
// 0.02    0.02         0.02          0.02              0.02
void DamBreakVisualDbg::readFullResultFile(QFile &aFile2read) {
  bool w_bStartReadVal{false}; // initialize

  // maybe should use std vector because we have the shrinktofit
  // reduce memory usage (capacity)
  // QVector<std::pair<double, QVector<dbltpl>>> w_vecTimeValues;
  if (!m_graph2D.m_vecTimeValues.isEmpty()) {
    m_graph2D.m_vecTimeValues.clear(); // should be empty!!!
  }
  m_graph2D.m_vecTimeValues.reserve(50); // for will do

  //     QVector<dbltpl> w_vecOfTpl;
  //     w_vecOfTpl.reserve(101); // be careful with hard coding

  QTextStream w_fileStream{&aFile2read};
  // loop on line
  QString line;
  double w_timeVal{}; // time at each iteration

  // reading line after one
  while (!w_fileStream.atEnd()) {
    w_fileStream.readLineInto(&line);
    if (w_bStartReadVal) {
      //         do
      //         {
      //           auto w_checkRetype = extractLineValues(line/*, w_vecOfTpl*/);
      //           w_vecOfTpl.push_back(std::move(w_checkRetype));
      //         } while ( w_fileStream.readLineInto(&line) && !line.isEmpty());

      // just a test (compile)
      auto w_vecOfTpl =
          extractLineValuesTmplt<double, double, double, double, double>(
              w_fileStream, std::move(line));
      w_bStartReadVal = false; // according to the current format

      // add it to vector of pair pair<Time, values>
      m_graph2D.m_vecTimeValues.push_back({w_timeVal, std::move(w_vecOfTpl)});
      continue; // ready to continue to next Time: 0.0034 for example
    }

    if (line.isEmpty()) // check if line is empty
    {
      continue; // go to next line
    }
    auto w_lineTrimmed = line.trimmed();
    //    auto dbgBool = w_lineTrimmed.contains('X');
    //    auto w_firstChar = line.at(0); // front

    if (!w_lineTrimmed.contains('X') && w_bStartReadVal == false) {
      // Time: 0.0019 sec.   (file format)
      if (w_lineTrimmed.contains("Time")) {
        // split about ":"
        auto w_timeStr = w_lineTrimmed.split(QString{":"});
        w_timeStr.back().remove(
            QRegularExpression{"[sec]"}); // remove all ocurence of "sec"
        w_timeStr.back().remove(w_timeStr.back().size() - 1,
                                1);              // remove dot at the end (sec.)
        w_timeVal = w_timeStr.back().toDouble(); // contains value + sec.
      }
      continue; // next line
    } else {
      // flag to specify we start reading values
      w_bStartReadVal = true;
      continue;
    }
  } // while-loop
}

// variables
// X         H          U1(A)             U2(Q)           V
void DamBreakVisualDbg::extractProfileFromFullRes(int aListIndex) {
  using namespace std;

  // reserve memory for storing values, here we use the bracket
  // operator [i] not push_back, expect memory allocated
  auto i = 0; // init
  if (!m_graph2D.m_xVar1.isEmpty()) {
    m_graph2D.m_xVar1.clear();
  }
  m_graph2D.m_xVar1.reserve(101); // again hard coded, prototyping phase
  switch (m_graph2D.m_graphFmt) {
  case eGraphFmt::XH:
    // copy values in m_xVar
    {
      for (auto &val : m_graph2D.m_vecTimeValues[aListIndex]
                           .second) // should be initial condition
      {
        // re-ordering tuple index, since in
        dbltpl w_revertOrder = tuple_inverse_idx_order<dbltpl, 4, 3, 2, 1, 0>(
            val, std::index_sequence<4, 3, 2, 1, 0>{});
        // swap tuple element
        val.swap(w_revertOrder);

        double x{};
        double h{};
        // fill the plot2D data profile container
        std::tie(x, h, ignore, ignore, ignore) = val;
        m_graph2D.m_xVar1.push_back({x, h});
        ++i; // next
      }
      break;
    }
  case eGraphFmt::XQ:
    // auto i = 0;  init
    for (const auto &val : m_graph2D.m_vecTimeValues[aListIndex]
                               .second) // should be initial condition
    {
      double x{};
      double q{};
      // fill the plot2D data profile container
      std::tie(x, ignore, ignore, q, ignore) = val;
      m_graph2D.m_xVar1.push_back({x, q});
      ++i; // next
    }
    break;
  case eGraphFmt::XV:
    //   auto i = 0;  init
    for (const auto &val : m_graph2D.m_vecTimeValues[aListIndex]
                               .second) // should be initial condition
    {
      double x{};
      double v{};
      // fill the plot2D data profile container
      std::tie(x, ignore, ignore, ignore, v) = val;
      m_graph2D.m_xVar1.push_back({x, v});
      ++i; // next
    }
    break;
  default:
    break;
  }
}

void DamBreakVisualDbg::plot2dButton() {
  // IMPORTANT key is the iteration number which start at 1
  // 		m_x.reserve(
  // static_cast<int>(m_graph2D.m_mapValU[1].first.size()));
  // // be careful hard coded 		m_h.reserve(
  // static_cast<int>(m_graph2D.m_mapValU[1].first.size()));  // shall retrieve
  // value from ???

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
             eFileFormat::dbgFormat == m_graph2D.m_filFmt) {
    // debug format (this format we plot in separate window not in the main)
    setMultipleGraph();
  } else if (eGraphFmt::XH == m_graph2D.m_graphFmt &&
             eFileFormat::exact_computed == m_graph2D.m_filFmt) {
    setMultipleGraph();
  }
}

void DamBreakVisualDbg::saveToFile() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Graph Image"), "",
      tr("Simulation Image (*.jpg);;All Files (*)"));

  if (!fileName.isEmpty()) // set default name as ...
  {
    if (m_customPlot) {
      m_customPlot->saveJpg(fileName + QString(".jpg"), 640, 400);
    } else {
      m_plot2d->saveJpg(fileName + QString(".jpg"), 640, 400);
    }
  } else {
    if (m_customPlot) {
      // name set from GUI ...
      m_customPlot->saveJpg(m_graph2D.m_graphName + QString(".jpg"), 640, 400);
    } else {
      m_plot2d->saveJpg(m_graph2D.m_graphName + QString(".jpg"), 640, 400);
    }
  }
}

void DamBreakVisualDbg::loadFromFile() {
  m_bigEditor->append("Starting to load data...");

  auto w_simFileName1 = QFileDialog::getOpenFileName(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  m_graph2D.m_file1.setFileName(w_simFileName1);
  if (!m_graph2D.m_file1.isOpen()) {
    auto check = m_graph2D.m_file1.open(QIODevice::ReadOnly);
  }

  // 			if( !m_graph2D.m_file1.isOpen())
  // 			{
  // 				QMessageBox::information( this, tr("Unable to
  // open file"),
  //           m_graph2D.m_file1.errorString());
  // 				return;
  // 			}

  // debug purpose
  //     auto w_file = m_fileFmtItem->currentText();

  m_bigEditor->append(" File format selected is: " +
                      m_fileFmtItem->currentText()); // this is the default one
  m_bigEditor->append(
      "You have selected the following file: " /*+ m_graph2D.m_simFileName*/);
  m_bigEditor->append(m_graph2D.m_file1.fileName());

  if (m_fileFmtItem->currentText() == "Debug") {
    m_graph2D.m_filFmt = eFileFormat::dbgFormat;

    //   auto check = m_graphFmtItem->currentText();
    if (m_graphFmtItem->currentText() ==
        "X/H") // debug: X/H (flat bed==X/A) and X/Q
    {
      m_graph2D.m_graphFmt = eGraphFmt::XH;
    } else if (m_graphFmtItem->currentText() == "X/Q") {
      m_graph2D.m_graphFmt = eGraphFmt::XQ;
    }
  } else if (m_fileFmtItem->currentText() ==
             "Final Profile") // variable profile
  {
    m_graph2D.m_filFmt = eFileFormat::var_profile; // X/H is default one

    //  auto check = m_graphFmtItem->currentText();
    if (m_graphFmtItem->currentText() ==
        "X/H") // debug: X/H (flat bed==X/A) and X/Q
    {
      m_graph2D.m_graphFmt = eGraphFmt::XH;
    }
  } else if (m_fileFmtItem->currentText() == "Full Result") {
    m_graph2D.m_filFmt = eFileFormat::full_result;
  } else if (m_fileFmtItem->currentText() == "Exact/Computed") {
    // readFiles2Cmp();
    auto w_simFileName2 = QFileDialog::getOpenFileName(
        this, tr("Open Graph Data"), "",
        tr("Simulation Result (*.txt);;All Files (*)"));

    m_graph2D.m_file2.setFileName(w_simFileName2);
    if (!m_graph2D.m_file2.isOpen()) {
      auto check = m_graph2D.m_file2.open(QIODevice::ReadOnly);
    }

    m_graph2D.m_filFmt = eFileFormat::exact_computed;
  }

  // DESIGN NOTE
  // Thread this function (start it in a independent thread, it doesn't block
  // the GUI) user can continue (std::future data returned by the thread)
  // std::thread w_testThread{}; // use variadic template ctor

  // IMPORTANT:
  //   we are using E. McNeil data discretization for this graph
  //   sequence of 101 numbers starting from 0, grid nodes coordinate
  loadData2Show();

  // symetry open/close (close in the same method that opened it!!)
  // see article of Kevin ... "Six Of The Best"
  if (m_graph2D.m_file1.isOpen())
    m_graph2D.m_file1.close();

  m_bigEditor->append(QString("Finished to load data"));
  m_bigEditor->append(
      QString("Select the number of graph to plot in the menu bar below"));
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

  QVBoxLayout *w_dropDownGraphFmt = new QVBoxLayout;
  QLabel *w_graphFmtLabel = new QLabel{tr("Graph Format")};
  m_graphFmtItem = new QComboBox;
  m_graphFmtItem->addItem(QString{"X/H"});
  m_graphFmtItem->addItem(QString{"X/Q"});
  m_graphFmtItem->addItem(QString{"X/V"});
  m_graphFmtItem->setEditable(false); // can't be edited by user
  w_dropDownGraphFmt->addWidget(w_graphFmtLabel);
  w_dropDownGraphFmt->addWidget(m_graphFmtItem);

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
  m_plot2d->xAxis->setRange(0., 1200.); // x-coordinate
  m_plot2d->yAxis->setRange(0., 12.);   // H-value
  m_plot2d->addGraph();

  // adding QCustomPlot plot to layout
  w_secondLayout->addWidget(m_plot2d); // take the ownership of the widget plot

  m_bigEditor = new QTextEdit(this);
  m_bigEditor->setOverwriteMode(false);
  m_bigEditor->setText("Ready to plot graph from simulation data");
  m_bigEditor->append("Press Load Button to proceed and select file");
  QHBoxLayout *w_textEdit = new QHBoxLayout;
  m_bigEditor->setMaximumSize(QSize(1.75 * m_centralWidget->width(), 150));
  w_textEdit->addWidget(m_bigEditor);

  // Add spacer item with 50px width and 1px height
  w_textEdit->addSpacerItem(new QSpacerItem(30, 50));

  // Graph settings box (shall create a group box)
  // 		auto w_graphSettingsBox1 = setHboxLayout( QString("Graph #1"));
  // 		auto w_graphSettingsBox2 = setHboxLayout( QString("Graph #2"));
  // 		auto w_graphSettingsBox3 = setHboxLayout( QString("Graph #3"));
  // 		auto w_graphSettingsBox4 = setHboxLayout( QString("Graph #4"));
  // 		auto w_graphSettingsBox5 = setHboxLayout( QString("Graph #5"));

  // 		std::initializer_list<std::string> w_listGraphNo{ std::string{
  // "Graph
  // #1" }, std::string{ "Graph #1=2" }, 			std::string{
  // "Graph #3"
  // }, std::string{ "Graph #4" }, std::string{ "Graph #5" } };

  // 		for (auto i=0;i<nbGraph;++i)
  // 		{
  auto w_hboxLayout = setAllHboxLayout(w_listGraphNo);
  //		}
  //	setColorComboBox();
  setAllColorComboBox();
  //		setLineTypeComboBox();
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
  //  std::initializer_list<std::string> w_listGraphNo{
  //      std::string{"Graph #1"}, std::string{"Graph #2"}, std::string{"Graph
  //      #3"}, std::string{"Graph #4"}, std::string{"Graph #5"}};

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

void DamBreakVisualDbg::readFiles2Cmp() {
  m_bigEditor->append("You have selected a comparison graph");

  auto w_simFileName1 = QFileDialog::getOpenFileName(
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

  m_bigEditor->append("You have selected the following file (first): " +
                      m_graph2D.m_file1.fileName());
  m_bigEditor->append("You have selected the following file (first): " +
                      m_graph2D.m_file2.fileName());
}

// Plot profile variable
void DamBreakVisualDbg::plot2DProfile(const eGraphFmt aGraphFmt) {
  // set data (index set 0 by default) shall we call addGraph()??
  m_plot2d->graph()->setPen(QColor(Qt::darkRed)); //
  m_plot2d->graph()->setLineStyle(
      QCPGraph::lsLine); // lsline(each point attached by line), lsnone (no
                         // point attached similar to dot)

  if (eFileFormat::var_profile == m_graph2D.m_filFmt ||
      eFileFormat::full_result == m_graph2D.m_filFmt) {
    if (eGraphFmt::XH == aGraphFmt) // under construction
    {
      m_plot2d->xAxis->setLabel("X Coordinate");
      m_plot2d->yAxis->setLabel("H (Water Depth)");
      m_plot2d->xAxis->setRange(0., 1.);
      m_plot2d->yAxis->setRange(0.43, 1.1);
    }

    // Design Note
    //  this is the main window (setdata()) with different data from user
    // auto w_profileData = m_graph2D.m_xVar1;   QVector of double, need to
    // extract
    QVector<double> w_xCoord;
    w_xCoord.reserve(m_graph2D.m_xVar1.size());
    std::transform(
        m_graph2D.m_xVar1.cbegin(), m_graph2D.m_xVar1.cend(),
        std::back_inserter(w_xCoord),
        [](std::pair<double, double> aPairVal) { return aPairVal.first; });

    QVector<double> w_varValues;
    w_varValues.reserve(m_graph2D.m_xVar1.size());
    std::transform(
        m_graph2D.m_xVar1.cbegin(), m_graph2D.m_xVar1.cend(),
        std::back_inserter(w_varValues),
        [](std::pair<double, double> aPairVal) { return aPairVal.second; });

    // Design Note
    //  call dambreak0Curve() which does exactly (shall be renamed by
    //  dambreak1Curve()) Should have a bunch of those function for different
    //  graph type dambreak2Curve() --> 2 curves (compare) dambreak3Curve() -->
    //  3 curves (compare at different time) multigraph() can plot 5 graphs
    //  (need to set parameters)

    // find max element in both axes
    const auto w_xmax = std::max_element(w_xCoord.cbegin(), w_xCoord.cend());
    const auto w_valmax =
        std::max_element(w_varValues.cbegin(), w_varValues.cend());
    const auto w_valmin =
        std::min_element(w_varValues.cbegin(), w_varValues.cend());

    // just a test, already set in SingleLayout
    //       m_plot2d->xAxis->setRange(0./*min*/, *w_xmax/*max*/); //
    //       x-coordinate m_plot2d->yAxis->setRange(*w_valmin/*min*/,
    //       *w_valmax/*max*/);   //

    // NOTE
    //  for the current version all hard coded, fix in future version
    //  now put all effort on the a first version of the plotting feature
    // set axes ranges, so we see all data:
    m_plot2d->xAxis->setRange(0., 1.);
    m_plot2d->yAxis->setRange(0.43, 1.1);

    // ready to plot
    m_plot2d->graph()->setData(w_xCoord, w_varValues);
  }
  //     else // debug file format and stepping mode (under construction)
  //     {
  //       // this prototype version we load data from file, but in the
  //       simulator
  //       // environment data will be fetched from the simulator
  //       (dbsim->getWaterDepthProfile())
  //       m_plot2d->graph()->setData(m_graph2D.m_qvecX,
  //         QVector<double>::fromStdVector(m_graph2D.m_mapValU[m_stepGraphCounter
  //         += 5].first));
  //     }

  // call replot() it sure is!!
  m_plot2d->replot();
}

// Eventually, user should be able to set the step value (default=5)
// DESIGN NOTE display in the main window
void DamBreakVisualDbg::plot2DProfile() // m_plot2d window
{
  m_bigEditor->append("******* Wave Profile at Iteration: " +
                      QString(std::to_string(m_stepGraphCounter).c_str()));

  // debug file format and stepping mode
  if (eGraphFmt::XH == m_graph2D.m_graphFmt) {
    // Lambda capture local variable by value
    // Copies of local variable returned by the function (5 is the step value by
    // default) Access copies of local variables at the time the lambda was
    // created.
    std::function<int(int)> offset_5 = make_offseter(m_graph2D.iterationStep);

    const auto w_maxH = std::max_element(
        m_graph2D.m_mapValU[offset_5(m_stepGraphCounter)].first.cbegin(),
        m_graph2D.m_mapValU[offset_5(m_stepGraphCounter)].first.cend());

    m_plot2d->xAxis->setLabel("X Coordinate");
    m_plot2d->yAxis->setLabel("H (Water Depth)");
    m_plot2d->xAxis->setRange(0., 1100.);   // x-coordinate
    m_plot2d->yAxis->setRange(0., *w_maxH); // H-value

    m_plot2d->graph()->setData(
        m_graph2D.m_qvecX,
        QVector<double>::fromStdVector(
            m_graph2D.m_mapValU[m_stepGraphCounter += 5].first));
  } else if (eGraphFmt::XQ == m_graph2D.m_graphFmt) {
    // Copies of local variable returned by the function (5 is the step value by
    // default) Access copies of local variables at the time the lambda was
    // created.
    std::function<int(int)> offset_5 = make_offseter(m_graph2D.iterationStep);

    const auto w_maxQ = std::max_element(
        m_graph2D.m_mapValU[offset_5(m_stepGraphCounter)].second.cbegin(),
        m_graph2D.m_mapValU[offset_5(m_stepGraphCounter)].second.cend());

    // set labels
    m_plot2d->xAxis->setLabel("X Coordinate");
    m_plot2d->yAxis->setLabel("Q (Water Discharge)");

    // need to find maximum value
    m_plot2d->xAxis->setRange(0., 1010.);   // x-coordinate
    m_plot2d->yAxis->setRange(0., *w_maxQ); // Q-value

    // set data for plotting
    m_plot2d->graph()->setData(
        m_graph2D.m_qvecX,
        QVector<double>::fromStdVector(
            m_graph2D.m_mapValU[m_stepGraphCounter += 5].second));
  } else {
    QMessageBox w_msg;
    w_msg.setText("Graph Profile Not Supported. Select X/H or X/Q for this "
                  "type of graph");
    w_msg.exec();
  }

  // call replot() it sure is!!
  m_plot2d->replot();
}

void DamBreakVisualDbg::getProfileTimes(const QString &aTimeVal) {
  auto w_checkTimeIdx = m_simTimesItem->currentIndex();

  // int w_timeIdx{}; // initialize to zero
  QStringListIterator w_timeListIter{m_graph2D.m_simTimesList};
  while (w_timeListIter.hasNext()) {
    if (w_timeListIter.next() == aTimeVal) {
      extractProfileFromFullRes(w_checkTimeIdx);
      break;
    }
  } // while-loop
}

// this version was used to validate the GUI but i don't it is needed anymore
// may think to remove it
// DEPRECATED!!
void DamBreakVisualDbg::dambreak0Curve() {
  // set x and h values (coordinate and water level)
  //		readDataFromFile(
  // w_wpathflename/*std::string("ComputedValues.txt")*/);

  // 2d plot (plot x-axis and y-axis)
  m_customPlot = new QCustomPlot;
  m_customPlot->setGeometry(0, 0, 640, 400); // window size when poping-up
  m_customPlot->setWindowTitle(QString("DamBreak Wave Propagation"));
  // color graph
  // create graph and assign data to it:
  m_customPlot->addGraph();
  //		m_customPlot->graph(0)->setData(m_x, m_h);

  // give the axes some labels:
  m_customPlot->xAxis->setLabel("x coordinate");
  m_customPlot->yAxis->setLabel("h (Water Depth)");

  // set axes ranges, so we see all data:
  m_customPlot->xAxis->setRange(0., 1.05);
  m_customPlot->yAxis->setRange(0.45, 1);
}

void DamBreakVisualDbg::setMultipleGraph() {
  // auto w_graphId = 0;

  if (nullptr != m_customPlot) {
    delete m_customPlot;
    m_customPlot = nullptr;
  }

  // does it make sense? not sure
  m_customPlot = new QCustomPlot;

  auto jj = 0;
  for (auto i = 0; i < nbGraph; ++i) {
    if (m_activate[i]->isChecked()) {
      m_customPlot->addGraph();
      auto w_graphLineType = m_lineType[i]->currentText();
      if (w_graphLineType == QString{"Dash Line"}) {
        m_customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsLine);
      } else if (w_graphLineType == QString{"Dot Line"}) {
        m_customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsNone);
      } else {
        m_customPlot->graph(i)->setLineStyle(QCPGraph::lsLine);
      }

      // data may belong to different file such as: exact/computed
      // we have to curve but data extract form files.
      //   m_graph2D.m_xVar

      //   m_customPlot->graph(i)->setData(m_graph2D.m_qvecX, // increment graph
      //   no by 1 for legend display and next graph
      //     QVector<double>::fromStdVector(m_graph2D));

      // ======================== DEBUG FORMAT
      if (eFileFormat::dbgFormat == m_graph2D.m_filFmt) {
        // m_customPlot->addGraph();
        m_customPlot->graph(i)->setPen(QColor(m_colorCurve[i]->currentText()));
        // m_customPlot->graph(i)->setLineStyle(QCPGraph::lsLine); lsline(each
        // point attached by line), lsnone (no point attached similar to dot)
        m_customPlot->graph(i)->setName(QString("Time_") +
                                        QString::number(jj++));
        if (eGraphFmt::XH == m_graph2D.m_graphFmt) // X/H profile
        {
          m_customPlot->graph(i)->setData(
              m_graph2D.m_qvecX, // increment graph no by 1 for legend display
                                 // and next graph
              QVector<double>::fromStdVector(
                  m_graph2D.m_mapValU[m_iterNo[i]->value()].first));

          // just testing (preprocessing stuff) under construction
          auto w_ymaxValue = std::max_element(
              m_graph2D.m_mapValU[m_iterNo[0]->value()].first.begin(),
              m_graph2D.m_mapValU[m_iterNo[0]->value()].first.end());

          setPlotParams(QString{"H(Water Depth)"},
                        *w_ymaxValue + 2.); // be careful with hard coding 2.?
        } else                              // X/Q profile
        {
          m_customPlot->graph(i)->setData(
              m_graph2D.m_qvecX, // increment graph no by 1 for legend display
                                 // and next graph
              QVector<double>::fromStdVector(
                  m_graph2D.m_mapValU[m_iterNo[i]->value()].second));

          // just testing (preprocessing stuff) under construction
          auto w_ymaxValue = std::max_element(
              m_graph2D.m_mapValU[m_iterNo[0]->value()].second.begin(),
              m_graph2D.m_mapValU[m_iterNo[0]->value()].second.end());

          setPlotParams(QString{"Q(Water Discharge)"},
                        *w_ymaxValue + 2.); // be careful with hard coding 2.?
        }
      } // dbgFormat
      else if (eFileFormat::exact_computed == m_graph2D.m_filFmt) {
        m_yAxisRng = {0.45, 1.1}; // temporary fix
        m_xAxisRng = {0., 1.};    // temporary fix

        // default graph format is X/H (it's the only one )
        if (i == 0) // temporary fix
        {
          m_customPlot->graph(i)->setPen(
              QColor(m_colorCurve[i]->currentText()));
          m_customPlot->graph(i)->setLineStyle(
              QCPGraph::lsLine); // lsline(each point attached by line), lsnone
                                 // (no point attached similar to dot)
          // legend name (first curve)
          m_customPlot->graph(i)->setName(QString{"Exact Solution"});
          setPlotParams(
              QString{"H(Water Depth)"},
              m_graph2D.m_yMaxValue); // be careful with hard coding 2.?
          // ... to be completed
          m_customPlot->graph(i)->setData(m_graph2D.m_qvecX,
                                          m_graph2D.m_ValuesCurve1);
          continue;
        } else // second graph
        {
          m_customPlot->graph(i)->setPen(
              QColor(m_colorCurve[i]->currentText()));
          m_customPlot->graph(i)->setLineStyle(
              QCPGraph::lsImpulse); // lsline(each point attached by line),
                                    // lsnone (no point attached similar to dot)
          // legend name (second curve)
          m_customPlot->graph(i)->setName(QString("Computed Solution"));
          setPlotParams(
              QString{"H(Water Depth)"},
              m_graph2D.m_yMaxValue); // be careful with hard coding 2.?
          m_customPlot->graph(i)->setData(m_graph2D.m_qvecX,
                                          m_graph2D.m_ValuesCurve2);
        }
      } // exact/computed
      else {
        continue;
      }
    } // for-loop

#if 0 // not really need it (done in m_lineType with setGeometry(...))
    // ...
      QPoint posWgtGlobalPos = m_customPlot->mapToGlobal(QPoint(0, 0));

      // Split screen into quadrants
      QDesktopWidget* desktop = QApplication::desktop();
      QRect screenRect = desktop->availableGeometry(posWgtGlobalPos);

      // desktop coordinate
      int screenMidX = screenRect.x() + (screenRect.width() / 2);
      int screenMidY = screenRect.y() + (screenRect.height() / 2);

      // Position the popup widget based on the quadrant posWgt is in
      int popX = posWgtGlobalPos.x();
      int popY = posWgtGlobalPos.y();

      popX += screenMidX / 2; //  m_customPlot->width();
      popY += screenMidY / 2; // m_customPlot->height();

      QPoint popPnt = m_customPlot->mapFromGlobal(QPoint(popX, popY));
      m_customPlot->setGeometry(popX, popY - 50,
        width() / 2,
        height() / 2);
#endif
    // plot selected graph
    m_customPlot->replot();

    // pop-up window (separate widget)
    m_customPlot->show();
  }
}

// Compare 2-graph (e.g. exact/computed)
// file format: two-columns x-H,Q,...
void DamBreakVisualDbg::dambreak2Curve() {
  // some utility to read file
  QTextStream w_data2Read1(&m_graph2D.m_file1);
  QString line;
  // if you don't do that, first time push_back is call
  // vector resize, not really efficient
  m_graph2D.m_xVar1.reserve(101); // debugging purpose

  // reading line after one
  while (w_data2Read1.readLineInto(&line)) {
    if (line.isEmpty()) // check if line is empty
    {
      continue; // go to next line
    }

    // split about white space () QStringList
    auto w_listDbl = line.split(" ");

    // push in a map or vector (move semantic)
    m_graph2D.m_xVar1.push_back({w_listDbl.front().toDouble() /*x*/,
                                 w_listDbl.back().toDouble() /*variable*/});
  } // while-loop

  QTextStream w_data2Read2(&m_graph2D.m_file2);
  // if you don't do that, first time push_back is call
  // vector resize, not really efficient
  m_graph2D.m_xVar2.reserve(101); // debugging purpose

  // reading line after one
  while (w_data2Read2.readLineInto(&line)) {
    if (line.isEmpty()) // check if line is empty
    {
      continue; // go to next line
    }

    // split about white space () QStringList
    auto w_listDbl = line.split(" ");

    // push in a map or vector (move semantic)
    m_graph2D.m_xVar2.push_back({w_listDbl.front().toDouble() /*x*/,
                                 w_listDbl.back().toDouble() /*variable*/});
  } // while-loop

  QVector<double> w_xCoord1;
  w_xCoord1.reserve(m_graph2D.m_xVar1.size());
  std::transform(
      m_graph2D.m_xVar1.cbegin(), m_graph2D.m_xVar1.cend(),
      std::back_inserter(w_xCoord1),
      [](std::pair<double, double> aPairVal) { return aPairVal.first; });

  QVector<double> w_xCoord2;
  w_xCoord2.reserve(m_graph2D.m_xVar2.size());
  std::transform(
      m_graph2D.m_xVar2.cbegin(), m_graph2D.m_xVar2.cend(),
      std::back_inserter(w_xCoord2),
      [](std::pair<double, double> aPairVal) { return aPairVal.first; });

  // sanity check
  assert(w_xCoord1.size() == w_xCoord2.size());

  // check x-coord equality
  if (w_xCoord1 == w_xCoord2) {
    m_bigEditor->append(QString{"Compare graph: both x-coordinate are equal"});
  }

  // sanity check x-coord (test concatenate) just a test
  //       auto w_tplCat = std::tuple_cat( m_graph2D.m_xVar1[0],
  //       m_graph2D.m_xVar2[0]); assert(
  //       std::tuple_size<decltype(w_tplCat)>::value == 4);

  // how we going to format to prepare for viewing? we have 2 curves
  // leave it with 4 tuple-element, use std::tie(x,h_ex,ignore,h_eval)
  // first curve: std::tie(x,h_ex,ignore,ignore)
  // second curve: std::tie(ignore,ignore, x, h_eval,)
  // call multigraph
  // push concatenate tuple in a vector, and extract data for each curve
  // GraphPrm::dummy1<double, double, double, double> w_dataFmt{
  // std::tuple_cat(m_graph2D.m_xVar1[0], m_graph2D.m_xVar2[0]) };
  // w_dataFmt.m_data=std::tuple_cat(m_graph2D.m_xVar1[0],
  // m_graph2D.m_xVar2[0]);

  // m_graph2D.m_data4.m_data = std::tuple_cat(m_graph2D.m_xVar1[0],
  // m_graph2D.m_xVar2[0]); GraphPrm::dummyVec<double, double, double, double>
  // w_twoCurveData2View;
  if (!m_graph2D.m_twoCurveData2View.isEmpty()) {
    m_graph2D.m_twoCurveData2View.clear();
  }
  m_graph2D.m_twoCurveData2View.reserve(w_xCoord2.size());
  for (auto i = 0; i < m_graph2D.m_xVar1.size(); ++i) // prepare dat to be view
  {
    // tuple of 4 element with the following format:
    // X|Var1|X|Var2 two-curves format
    m_graph2D.m_twoCurveData2View.push_back(
        {std::tuple_cat(m_graph2D.m_xVar1[i], m_graph2D.m_xVar2[i])});
  }

  // prepare data to view (data for viewing both curve )
  // retrieve data
  //      const auto w_vecurvesData = m_graph2D.m_twoCurveData2View;

  // Curve #1
  //       m_graph2D.m_xVar1.reserve(m_graph2D.m_twoCurveData2View.size());
  //       std::transform(w_vecurvesData.cbegin(), w_vecurvesData.cend(),
  //       m_graph2D.m_xVar1.begin(),
  //         [](const auto& aTpl) -> std::pair<double, double> { double x;
  //         double h;  std::tie(x, h, std::ignore, std::ignore); return{ x,h };
  //         });
  //
  //       // Curve #2
  //       m_graph2D.m_xVar2.reserve(m_graph2D.m_twoCurveData2View.size());
  //       std::transform(w_vecurvesData.cbegin(), w_vecurvesData.cend(),
  //       m_graph2D.m_xVar2.begin(),
  //         [](const auto& aTpl) -> std::pair<double, double> { double x;
  //         double h;  std::tie(std::ignore, std::ignore, x, h); return{ x,h };
  //         });

  // x-coordinate
  //       m_graph2D.m_qvecX.reserve(m_graph2D.m_xVar1.size());
  //       std::transform(m_graph2D.m_xVar1.cbegin(), m_graph2D.m_xVar1.cend(),
  //       // x-coord are identical for both graph
  //         std::back_inserter(m_graph2D.m_qvecX),
  //         [](std::pair<double, double> aPairVal)
  //       {
  //         return aPairVal.first;
  //       });

  m_graph2D.m_qvecX = w_xCoord1;

  // first curve: h value
  m_graph2D.m_ValuesCurve1.reserve(m_graph2D.m_xVar1.size());
  std::transform(
      m_graph2D.m_xVar1.cbegin(), m_graph2D.m_xVar1.cend(),
      std::back_inserter(m_graph2D.m_ValuesCurve1),
      [](std::pair<double, double> aPairVal) { return aPairVal.second; });

  // second curve: h value
  m_graph2D.m_ValuesCurve2.reserve(m_graph2D.m_xVar2.size());
  std::transform(
      m_graph2D.m_xVar2.cbegin(), m_graph2D.m_xVar2.cend(),
      std::back_inserter(m_graph2D.m_ValuesCurve2),
      [](std::pair<double, double> aPairVal) { return aPairVal.second; });

  // find max y-axis value
  auto MaxValue1 = *std::max_element(m_graph2D.m_ValuesCurve1.cbegin(),
                                     m_graph2D.m_ValuesCurve1.cend());
  auto MaxValue2 = *std::max_element(m_graph2D.m_ValuesCurve2.cbegin(),
                                     m_graph2D.m_ValuesCurve2.cend());
  // maximum value y-axis scale
  m_graph2D.m_yMaxValue = std::max(MaxValue1, MaxValue2);
}

// NOTE: lot of hard coding, all these value will be set
// from simulation GUI (on construction)
void DamBreakVisualDbg::loadData2Show() {
  // DESIGN NOTE
  //  this is a piece of crapt!!! need a serious refctoring!!
  // Not sure about this one!! need to think about it!
  std::vector<double> w_X; // shall we use reserve? yes i do think so
  // E. McNeil data
  if (m_graph2D.m_dataType == eDataType::EMcNeil) {
    w_X.reserve(101); // since we are using back_inserter (push_back)
    const auto w_dx = 10.;
    const auto w_startX = -10.;
    const unsigned w_nbPts = 101;
    std::generate_n(std::back_inserter(w_X),              // start
                    w_nbPts,                              // number of elements
                    NumSequence<double>(w_startX, w_dx)); // generate values

    // need QVector to be use in QtCustomPlot, let's create one with the helper
    m_graph2D.m_qvecX = QVector<double>::fromStdVector(w_X);
    assert(m_graph2D.m_qvecX.size() == w_nbPts);
  } else // only 2 supported for now
  {
    // std::vector<double> w_X;  shall we use reserve? yes i do think so
    w_X.reserve(101); // since we are using back_inserter (push_back)
    const auto w_dx = 0.01;
    const auto w_startX = -0.01;
    const unsigned w_nbPts = 101;
    std::generate_n(std::back_inserter(w_X),              // start
                    w_nbPts,                              // number of elements
                    NumSequence<double>(w_startX, w_dx)); // generate values

    // need QVector to be use in QtCustomPlot, let's create one with the helper
    m_graph2D.m_qvecX = QVector<double>::fromStdVector(w_X);
    assert(m_graph2D.m_qvecX.size() == w_nbPts);
  }

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  // std::chrono::time_point<std::chrono::system_clock> start, end;
  //  auto start = std::chrono::system_clock::now();
  switch (m_graph2D.m_filFmt) {
  case qplot::DamBreakVisualDbg::eFileFormat::dbgFormat:
    // extract data from result file
    qplot::PrepareSimRes2View(m_graph2D.m_mapValUp, m_graph2D.m_mapValU,
                              m_graph2D.m_file1.fileName().toStdString());
    break;
  case qplot::DamBreakVisualDbg::eFileFormat::var_profile:
    readDataFromFile(m_graph2D.m_file1); // format: X/H, X/V
    break;
  case qplot::DamBreakVisualDbg::eFileFormat::full_result:
    readFullResultFile(m_graph2D.m_file1); // vector<pair<Time, values>>, X/H or
                                           // X/Q or X/V user select
    extractSimTimes(); // extract times value and fill combo box
    // extractProfileFromFullRes();
    break;
  case qplot::DamBreakVisualDbg::eFileFormat::exact_computed:
    dambreak2Curve();
    break;
  default: // nothing to do for now (not supported format)
    break;
  }

#if 0
      // not sure if it is relevant anymore!!!
      auto end = std::chrono::system_clock::now();
      long long elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>
        (end - start).count();
#endif

  m_bigEditor->append(QString{"Finished preparing data to view"});
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

#if 0 // see header file, now a template
    auto TestQTApp::extractLineValues(QString& aStrRead/*, QVector<dbltpl>& aVecOfValues*/)
    {
      // remove all white space, but its not what i want
      // but i think i need to do that
      // Returns a string that has whitespace removed from the start and the end,
      // and that has each sequence of internal whitespace replaced with a single space.
      auto w_lineTrimmed = aStrRead.simplified().toStdString();
      std::istringstream w_iis{ w_lineTrimmed }; // step to next white space
      auto w_tplResult = parse<double, double, double, double, double>(w_iis);
      return w_tplResult;

      //       auto beginIIS = std::istream_iterator<std::string>{ w_iis }; // begin range
      //       auto endIIS = std::istream_iterator<std::string>{}; // end range
      //       std::vector<std::string> w_spitVec{ beginIIS, endIIS };// contains all string separated by white space
      //
      //       if( w_spitVec.size()==5) // full result file format
      //       {
      //         // ready to copy to tuple order X | H | A | Q | V
      //         auto w_valTpl = dbltpl{ std::stod(w_spitVec[0]), std::stod(w_spitVec[1]),
      //            std::stod(w_spitVec[2]), std::stod(w_spitVec[3]), std::stod(w_spitVec[4]) };
      //
      //         aVecOfValues.push_back(w_valTpl);
      //       }
      //       else if( w_spitVec.size() == 3) // comparing graph
      //       {
      //         // ready to copy to tuple order X | Var1 | Var2
      //         auto w_valTpl = dbltpl3{ std::stod(w_spitVec[0]), std::stod(w_spitVec[1]),
      //           std::stod(w_spitVec[2]) };
      //       }
    }
#endif

void DamBreakVisualDbg::extractSimTimes() {
  // extract value from full result according to user profile selection
  auto w_vecValueAtime = m_graph2D.m_vecTimeValues[0].first;
  assert(0.000 == w_vecValueAtime);

  // Store all times (for each iteration), want to use it in the visualizing
  // the full result (user may want to check or display different profile at
  // different time)
  m_graph2D.m_simTimesList.reserve(70);
  for (const auto w_time : m_graph2D.m_vecTimeValues) {
    m_graph2D.m_simTimesList.push_back(QString::number(w_time.first));
  }

  // GUI side we have a drop-down box that list all times
  // user select a time to visualize simulation result
  m_simTimesItem->addItems(m_graph2D.m_simTimesList);
}
} // namespace qplot
