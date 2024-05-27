// C++ includes
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility> // std::move
// Boost string algorithm
#include <boost/algorithm/string.hpp>
// App includes
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Plot library
#include "qcustomplot.h"

// temporary fix for scaling graph
// set scale axis EMcNeil is from 1. to 1000.
// Hudson report is from 0. to 1.
// Compile variable
#define EMCNEILDATA 0

namespace qcplot {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

  ui->setupUi(this);

  // without this central widget we cannot see anything
  // since Qt5, form is divided in different area Docket,
  // menu and central widget.
  m_centralWidget = new QWidget;

  // m_centralWidget->setLayout( m_mainLayout);
  m_centralWidget->setLayout(singleLayout()); // prototyping

  // layout geometry size
  setGeometry(30, 50, 1200, 800);

  // set window title of main window
  setWindowTitle(QObject::tr("DamBreak++ Visual Debugger"));
  setCentralWidget(m_centralWidget);

  try {
    // 2d plot (plot x-axis and y-axis)
    m_customPlot = new QCustomPlot;            // // Graph in a separate window
    m_customPlot->setGeometry(0, 0, 640, 400); // window size when poping-up
    m_customPlot->setWindowTitle(QString("DamBreak Wave Propagation"));
    //   m_customPlot->plotLayout()->addElement( // deprecated, see below new
    //   version
    //       1, 0, new QCPPlotTitle( m_customPlot, "Lax-Friedrichs Solver"));
    m_customPlot->plotLayout()->addElement(
        1, 0, new QCPTextElement(m_customPlot, "LAX-Friedrichs Solver"));
    m_customPlot->xAxis->setLabel("X Coordinate");
    m_customPlot->yAxis->setLabel("H (Water Depth)");
    // IMPORTANT
#if EMCNEILDATA
    // set axes ranges, E. McNeil data X:[0.,1000.], H[0.,10.]
    m_customPlot->xAxis->setRange(0., 1200.); // x-coordinate
    m_customPlot->yAxis->setRange(0., 12.);   //
#else
    // Hudson report values X:[0.,1.], H[0.5,1.]
    m_customPlot->xAxis->setRange(0., 1.);    // x-coordinate
    m_customPlot->yAxis->setRange(0.35, 1.2); //
#endif
    // Try to add a legend to the graph
    m_customPlot->setLocale(
        QLocale(QLocale::English,
                QLocale::UnitedStates)); // period as decimal separator and
                                         // comma as thousand separator
    m_customPlot->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    m_customPlot->legend->setFont(legendFont);
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

MainWindow::~MainWindow() {

  if (m_customPlot) {
    delete m_customPlot;
    m_customPlot = nullptr;
  }

  delete ui;
}

QGridLayout *MainWindow::singleLayout() {
  std::initializer_list<std::string> w_listGraphNo{
      std::string{"Graph #1"}, std::string{"Graph #2"}, std::string{"Graph #3"},
      std::string{"Graph #4"}, std::string{"Graph #5"}};

  // Main layout prototype
  QGridLayout *w_mainGridLayout = new QGridLayout;

  QPushButton *w_plotButton = new QPushButton(tr("Plot"));
  QSizePolicy w_firstColumnSizPolicy;
  w_firstColumnSizPolicy.setHorizontalStretch(20);
  w_plotButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_plotButton, SIGNAL(clicked()), this, SLOT(plot2dButton()));

  QPushButton *w_closeButton = new QPushButton(tr("Close"));
  w_closeButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QPushButton *w_saveButton = new QPushButton(tr("Save"));
  w_saveButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_saveButton, SIGNAL(clicked()), this, SLOT(saveToFile()));

  QPushButton *w_loadButton = new QPushButton(tr("Load"));
  w_loadButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_loadButton, SIGNAL(clicked()), this, SLOT(loadFromFile()));

  QPushButton *w_stepButton = new QPushButton(tr("Step"));
  w_stepButton->setSizePolicy(w_firstColumnSizPolicy);
  QObject::connect(w_stepButton, SIGNAL(clicked()), this,
                   SLOT(plot2DProfile()));

  QVBoxLayout *w_firstColumn = new QVBoxLayout;
  w_firstColumn->setDirection(QBoxLayout::Direction::TopToBottom);
  w_firstColumn->addWidget(w_plotButton); // take the ownership
  w_firstColumn->addWidget(w_saveButton);
  w_firstColumn->addWidget(w_loadButton);
  w_firstColumn->addWidget(w_closeButton);
  w_firstColumn->addWidget(w_stepButton);
  w_firstColumn->insertSpacerItem(1, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(3, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(5, new QSpacerItem(30, 20));
  w_firstColumn->insertSpacerItem(7, new QSpacerItem(30, 20));
  w_firstColumn->addStretch();

  QVBoxLayout *w_secondLayout = new QVBoxLayout(/*w_plotFrame*/);

  // plotter integrated in the GUI (right side)
  m_plot2d = new QCustomPlot;
  m_plot2d->xAxis->setLabel("X Coordinate");
  m_plot2d->yAxis->setLabel("H (Water Depth)");
  // set axes ranges, so we see all data:
  m_plot2d->xAxis->setRange(0., 1200.); // x-coordinate
  m_plot2d->yAxis->setRange(0., 12.);   //
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

  // ...
  auto w_hboxLayout = setAllHboxLayout(w_listGraphNo);

  // ...
  setAllColorComboBox();

  // ...
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

std::array<QHBoxLayout *, 5> MainWindow::setAllHboxLayout(
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

    // Add spacer item with 50px width and 1px height
    // 			w_allBoxLayout->addSpacerItem(new QSpacerItem(30, 50));
    // 			w_allBoxLayout->addStretch(5);
    // 			//	m_HBox1->setContentsMargins(0, 0, 0, 0);
    // 			w_allBoxLayout->setSpacing(25);

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

void MainWindow::setAllColorComboBox() {
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

void MainWindow::setAllLineTypeComboBox() {
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

void MainWindow::plot2dButton() {
  // IMPORTANT key is the iteration number which start at 1
  //  m_x.reserve(static_cast<int>(
  //      m_graph2D.m_mapValU[1].first.size())); // be careful hard coded
  //  m_h.reserve(static_cast<int>(
  //      m_graph2D.m_mapValU[1].first.size())); // shall retrieve value from
  //      ???
  //		}

  // to be completed
  setMultipleGraph();
}

void MainWindow::saveToFile() {
  QString fileName = QFileDialog::getSaveFileName(
      this, tr("Save Graph Image"), "",
      tr("Simulation Image (*.jpg);;All Files (*)"));

  if (!fileName.isEmpty()) // set default name as ...
  {
    m_plot2d->saveJpg(fileName + QString(".jpg"), 640, 400);
  } else {
    // name set from GUI ... this is a BUG i ma not sure what we doing here!!!!
    // need to be review, i think I'm setting name with iteration value??
    m_customPlot->saveJpg(
        m_graph2D.m_graphName +
            QString(std::to_string(m_iterNo[0]->value()).c_str()) +
            QString(".jpg"),
        640, 400);
  }
  // 		else
  // 		{
  // 			QFile file(fileName);
  // 			if (!file.open(QIODevice::WriteOnly))
  // 			{
  // 				QMessageBox::information( this, tr("Unable to
  // open file"), 					file.errorString());
  // return;
  // 			}
  // 	  }
}

// DESIGN NOTE need to pass x-coord minimum, spatial step and nuber of grid node
void MainWindow::loadData2Show() {
  // hard coded for now, simulation will provide this number
  const auto w_dx = 10.;
  auto w_startX = -10.; // start before (offset) because want first node x=0.
  // hard coded for now, simulation will provide this number
  const auto w_nbPts = 101;
  m_graph2D.m_qvecX.reserve(101); // temporary fix
  // generate x-coordiante of the grid
  std::generate_n(std::back_inserter(m_graph2D.m_qvecX), // start
                  w_nbPts,                               // number of elements
                  [&w_startX, w_dx]() -> double {
                    w_startX += w_dx;
                    return w_startX;
                  }); //

  // sanity check
  assert(m_graph2D.m_qvecX.size() == w_nbPts);

  // measuring simulation time (not usre if we will keep it)
  // std::chrono::time_point<std::chrono::system_clock> start, end;
  auto start = std::chrono::system_clock::now();

  // extract data from result file
  PrepareSimRes2View(m_graph2D.m_mapValUp, m_graph2D.m_mapValU,
                     m_graph2D.m_simFileName.toStdString());

  // not sure if it is relevant anymore!!!
  auto end = std::chrono::system_clock::now();
  long long elapsed_seconds =
      std::chrono::duration_cast<std::chrono::seconds>(end - start).count();

  std::cout << "Finished preparing data to view in " << elapsed_seconds << "\n";
}

// reading data from files and plot them for comparison purpose
void MainWindow::loadData2Cmp(const QStringList &aFileNameList) {
  m_graph2D.m_qvecX.resize(101);
  m_graph2D.m_hvalExact.resize(101);
  m_graph2D.m_hvalComputed.resize(101);
  auto w_firstFile = true;
  QStringListIterator w_strListIter{aFileNameList};
  while (w_strListIter.hasNext()) { // file iterate
    QFile w_file{w_strListIter.next()};
    if (!w_file.open(QIODevice::ReadOnly | QIODevice::Text))
      return;

    // ...
    QTextStream w_text2Read{&w_file};
    auto i = 0;
    while (!w_text2Read.atEnd()) { // line iterate
      auto w_line = w_text2Read.readLine();
      std::istringstream w_iss{w_line.toStdString()};
      std::vector<std::string> w_vecSplit{
          std::istream_iterator<std::string>{w_iss},
          std::istream_iterator<std::string>{}};
      if (w_firstFile) {
        m_graph2D.m_qvecX[i] = QString(w_vecSplit[0].c_str()).toDouble();
        m_graph2D.m_hvalExact[i++] = QString(w_vecSplit[1].c_str()).toDouble();
      } else {
        m_graph2D.m_hvalComputed[i++] =
            QString(w_vecSplit[1].c_str()).toDouble();
      }
    } // while-loop (read file)
    w_file.close();
  } // while-loop (file)

  //  // need to save ...
  //  m_customPlot->saveJpg(QString{"JeanCmpTest"} + QString(".jpg"), 640, 400);
}

void MainWindow::loadFromFile() {
  m_bigEditor->append("Starting to load data...");

#if !EMCNEILDATA // temporary fix for solution comparison, feature is not
                 // implemented (Hudson data scaling)
  // ...
  QStringList w_listFiles = QFileDialog::getOpenFileNames(
      this, tr("Open Graph Data"), "",
      tr("Simulation Result (*.txt);;All Files (*)"));

  loadData2Cmp(w_listFiles);
  setTwoGraphCmp();
  return;
#else
  QDir w_currDir = QDir::current();
  m_bigEditor->append(w_currDir.dirName());
  w_currDir.cdUp();
  auto w_absPath = w_currDir.absolutePath();
  w_currDir.setCurrent(w_absPath + QString{R"(\DamBreakVisualDbg\Data2Test)"});
  //  w_currDir.cd(QString{R"(\DamBreakVisualDbg\Data2Test)"});
  m_bigEditor->append(w_currDir.dirName());

  if (w_currDir.cd(QString{R"(\DamBreakVisualDbg\Data2Test)"})) {
    m_graph2D.m_simFileName = QFileDialog::getOpenFileName(
        this, tr("Open Graph Data"), "",
        tr("Simulation Result (*.txt);;All Files (*)"));
  } else {
    m_graph2D.m_simFileName = QFileDialog::getOpenFileName(
        this, tr("Open Graph Data"), "",
        tr("Simulation Result (*.txt);;All Files (*)"));
  }

  if (m_graph2D.m_simFileName.isEmpty())
    return;
  else {
    QFile file(m_graph2D.m_simFileName);

    if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(this, tr("Unable to open file"),
                               file.errorString());
      return;
    }

    m_bigEditor->append(
        "You have selected the following file: " /*+ m_graph2D.m_simFileName*/);
    m_bigEditor->append(m_graph2D.m_simFileName);

    // DESIGN NOTE
    // Thread this function (start it in a independent thread, it doesn't block
    // the GUI) user can continue (std::future data returned by the thread)
    // std::thread w_testThread{}; // use variadic template ctor

    // IMPORTANT:
    //   we are using E. McNeil data discretization for this graph
    //   sequence of 101 numbers starting from 0, grid nodes coordinate
    loadData2Show();

    m_bigEditor->append(QString("Finished to load data"));
    m_bigEditor->append(
        QString("Select the number of graph to plot in the menu bar below"));
    m_bigEditor->append(QString("Once you finished with graph config, click Ok "
                                "to validate your selection"));
  }
#endif
}

void MainWindow::setMultipleGraph() {
  auto jj = 0;
  for (auto i = 0; i < nbGraph; ++i) {
    if (m_activate[i]->isChecked()) {
      m_customPlot->addGraph();
      m_customPlot->graph(i)->setPen(QColor(m_colorCurve[i]->currentText()));
      m_customPlot->graph(i)->setLineStyle(
          QCPGraph::lsLine); // lsline(each point attached by line), lsnone (no
                             // point attached similar to dot)
      m_customPlot->graph(i)->setName(QString("Time_") + QString::number(jj++));
      m_customPlot->graph(i)->setData(
          m_graph2D.m_qvecX, // increment graph no by 1 for legend display and
                             // next graph
          QVector<double>::fromStdVector(
              m_graph2D.m_mapValU[m_iterNo[i]->value()].first));
      //	++jj;
    } else {
      continue;
    }
  } // for-loop

  // plot selected graph
  m_customPlot->replot();

  // pop-up window
  m_customPlot->show();
}

void MainWindow::setTwoGraphCmp() {
  bool w_firstGraph = true;
  QVector<QCPScatterStyle::ScatterShape> shapes;
  shapes << QCPScatterStyle::ssCross;
  shapes << QCPScatterStyle::ssCircle;
  shapes << QCPScatterStyle::ssPlus;
  shapes << QCPScatterStyle::ssDisc;
  shapes << QCPScatterStyle::ssSquare;
  shapes << QCPScatterStyle::ssDiamond;

  for (auto i = 0; i < 2; ++i) {
    if (w_firstGraph) {
      m_customPlot->addGraph();
      m_customPlot->graph(i)->setPen(QColor(Qt::darkBlue));
      m_customPlot->graph(i)->setLineStyle(
          QCPGraph::lsLine); // lsline(each point attached by line),
      m_customPlot->graph(i)->setName(QString("Exact"));
      // set scatter style:
      if (shapes.at(i) != QCPScatterStyle::ssCustom) {
        m_customPlot->graph()->setScatterStyle(
            QCPScatterStyle(shapes.at(i), 10));
      }
      m_customPlot->graph(i)->setData(m_graph2D.m_qvecX, m_graph2D.m_hvalExact);
      //     m_customPlot->graph()->rescaleAxes(true);
      w_firstGraph = false;
    } else {
      m_customPlot->addGraph();
      m_customPlot->graph(i)->setPen(QColor(Qt::darkRed));
      m_customPlot->graph(i)->setLineStyle(
          QCPGraph::lsNone); // lsline(each point attached by line),
      m_customPlot->graph(i)->setName(QString("Computed"));
      // set scatter style:
      if (shapes.at(i) != QCPScatterStyle::ssCustom) {
        m_customPlot->graph()->setScatterStyle(
            QCPScatterStyle(shapes.at(i), 10));
      }
      m_customPlot->graph(i)->setData(m_graph2D.m_qvecX,
                                      m_graph2D.m_hvalComputed);
      //      m_customPlot->graph()->rescaleAxes(true);
    }
  }
  //  // plot selected graph
  m_customPlot->replot();
  //  m_customPlot->show();

  // need to save ...
  m_customPlot->saveJpg(QString{"JeanCmpTest"} + QString(".jpg"), 640, 400);
}

// Simplified version for reading simulation result from file and extract to
// prepare for visualizing
void MainWindow::PrepareSimRes2View(mapValU12 &aMapU12p, mapValU12 &aMapU12,
                                    const std::string &aFilename) {
  using namespace std;
  using namespace boost;

  // tested stuff to read from data source file
  ifstream w_file2read(aFilename);
  if (!w_file2read.is_open()) {
    std::cerr << "Cannot open file for reading simulation data\n";
    return exit(EXIT_FAILURE);
  }

  string w_lineRed;
  w_lineRed.reserve(50); // SSO Short String Optimization
  int w_iterNb = 1;      // initialization

  // some container to store data for future use
  vector<double> w_vU1;  // A
  vector<double> w_vU2;  // Q
  vector<double> w_vU1p; // Ap
  vector<double> w_vU2p; // Qp

  // line-by-line
  while (!w_file2read.eof()) {
    getline(w_file2read, w_lineRed);
    w_lineRed.shrink_to_fit();
    // Can try std::search();
    // const std::string w_textSearch = "Iteration"; instead of contains
    if (contains(w_lineRed, "Iteration number")) {
      std::istringstream w_iss{w_lineRed};
      std::vector<std::string> w_splitVec{
          std::istream_iterator<std::string>{w_iss}, // range begin
          std::istream_iterator<std::string>{}};     // range end

      auto currNb = stoi(w_splitVec[2]);
      if (w_iterNb == 1 && currNb == 1)
        continue;

      // clearing data to map values for future use
      aMapU12p.insert(make_pair(
          w_iterNb, // current iteration
          make_pair(std::move(w_vU1p), std::move(w_vU2p)))); // values extracted

      aMapU12.insert(make_pair(
          w_iterNb,                                        // current iteration
          make_pair(std::move(w_vU1), std::move(w_vU2)))); // values extracted

      w_iterNb = currNb;
      continue;
    }
    if (w_lineRed.empty())
      continue;

    if (w_lineRed.find('p') != std::string::npos) {
      // some test on splitting string without using boost split
      // NOTE std::istringstream operator>> has an interesting property
      // ...
      std::istringstream w_iss{w_lineRed};
      std::vector<std::string> w_splitVec{
          std::istream_iterator<std::string>{w_iss}, // range begin
          std::istream_iterator<std::string>{}};     // range end
      w_lineRed.clear();
      w_lineRed.reserve(50);
      w_vU1p.push_back(std::stod(
          *(std::next(w_splitVec.begin())))); // assume second element is value
      w_vU2p.push_back(
          std::stod(w_splitVec.back())); // assume last element is value
                                         //			continue;
    } else                               // U1,2
    {
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

  // clearing data to map values for future use
  aMapU12p.insert(make_pair(
      w_iterNb, // current iteration number
      make_pair(std::move(w_vU1p), std::move(w_vU2p)))); // values extracted
  aMapU12.insert(make_pair(
      w_iterNb, // current iteration number
      make_pair(std::move(w_vU1), std::move(w_vU2)))); // values extracted
}

void MainWindow::plot2DProfile() {
  //		m_bigEditor->append("\n");
  m_bigEditor->append("******* Wave Profile at Iteration: " +
                      QString(std::to_string(m_stepGraphCounter).c_str()));

  // set data (index set 0 by default) shall we call addGraph()??
  m_plot2d->graph()->setPen(QColor(Qt::darkRed)); //
  m_plot2d->graph()->setLineStyle(
      QCPGraph::lsLine); // lsline(each point attached by line), lsnone (no
                         // point attached similar to dot)
  // this prototype version we load data from file, but in the simulator
  // environment data will be fetch from the simulator (dbsim->getU1Data())
  m_plot2d->graph()->setData(
      m_graph2D.m_qvecX,
      QVector<double>::fromStdVector(
          m_graph2D.m_mapValU[m_stepGraphCounter += 5].first));

  //	m_bigEditor->append("To save this Wave Profile click on save and select
  // folder and name");

  // sanity check (debugging purpose)
  //  auto w_graphNumber2Plot = m_plot2d->graphCount(); // shall be equal 1
  //  QCPGraph *w_checkgraph = m_plot2d->graph(); // shall be not equal nullptr

  // could call savejpg() to save surrent profile to file

  // call replot() it sure is!!
  m_plot2d->replot();
}
} // namespace qcplot
