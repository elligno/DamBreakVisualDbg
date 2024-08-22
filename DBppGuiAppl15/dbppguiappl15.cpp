#include "dbppguiappl15.h"
#include "ui_dbppguiappl15.h"

#if 0  // original code at project creation before merged VS15 stuff
DBppGuiAppl15::DBppGuiAppl15(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DBppGuiAppl15)
{
    ui->setupUi(this);
}

DBppGuiAppl15::~DBppGuiAppl15()
{
    delete ui;
}
#endif // 0

// C++ include
//#include <thread>
// Qt includes
#include <QBoxLayout>
#include <QDebug> // debug helper
#include <QGroupBox>
#include <QMessageBox>
#include <QSettings>
#include <QTextEdit>

#if 0
// Library includes
#include "Discretization/dbpp_GlobalDiscretization.h"
#include "SfxTypes/dbpp_Simulation.h"
#include "Utility/dbpp_Hydro1DLogger.h"
#include "Utility/dbpp_TestLogger.h"
#include "dbpp_Wave1DSimulator.h"
#endif
// test stuff
//#include "Utility/dbpp_Worker.h"

// dbpp::Wave1DSimulator* m_waveSim {nullptr};  = new dbpp::Wave1DSimulator; ???

// Thread function
void testThreadCall() {
  // m_waveSim = new dbpp::Wave1DSimulator;
  // m_waveSim->timeLoop();
  // should i delete it the pointer
}

DBppGuiAppl15::DBppGuiAppl15(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::DBppGuiAppl15) // ..
                                // m_waveSim{new dbpp::Wave1DSimulator} create
                                // the simulator
{

  // Organization name
  qApp->setOrganizationName(QString("Elligno Inc"));

  // Application name
  qApp->setApplicationName(QString("DamBreak++ Simulator"));

  // Some sanity check (we shall have 1 window, main)
  // Widget I am not sure, a bunch of them
  //  QWindowList w_checkListWin = QApplication::allWindows();
  //  Q_ASSERT(w_checkListWin.size() == 0);

  QWidgetList w_checkListWgt = QApplication::allWidgets();
  Q_ASSERT(w_checkListWgt.size() == 1);

  qApp->setApplicationVersion(QString{"0.1"});
  qApp->aboutQt(); // just a test for access global variable application

  // auto *quit = new QAction("&Quit", this);

  //  QMenu *file = menuBar()->addMenu("&File");
  //   file->addAction(quit);
  //

#if 0
  // i am not too sure about this, but at some point i need
  // to set the main part of the GUI
  QDesktopWidget* w_desktop = qApp->desktop();
  QWidget* w_centralWidget = Q_NULLPTR;  // create Simulation group box
  QLayout* w_centralLayout = new QHBoxLayout;
  QGroupBox* m_testGRb = Q_NULLPTR;
  w_centralLayout->addWidget(m_testGRb);
  w_centralWidget->setLayout(w_centralLayout);
  setCentralWidget(w_centralWidget);
  ui->centralWidget->setLayout(ui->m_simulation);

  ui->m_simulation;
#endif
  // temporary for now, testing std::thread mechanism
  // should be a member of the class as it used to be
  // m_waveSim = new dbpp::Wave1DSimulator;

  //
  // DamBreak Application Configuration
  //

  // Create a logger to write error message
  // dbpp::Logger::instance()->Init();

  // Read from file bathymetry value and Manning coefficient (not sure)

  // set the numerical algorithm to be used (user selection or default)
  // set the CFL number for this simulation (default or user selection)
  // set the discretization to be used for this simulation, default or user
  // selection

  // setup GUI functionality
  ui->setupUi(this);
#if 0
  // rad default value (E. McNeil data)
  m_waveSim->setPhi1(ui->_phi1->value());                    // 10.
  m_waveSim->setPhi0(ui->_phi0->value());                    // 1.
  m_waveSim->setShockLocation(ui->_shockloc->value());       // 500.
  m_waveSim->setIterationNumberMax(ui->iterations->value()); // default is 1
#endif
  // NOTE not too sure about this one, not calling the slot "setUpstream()"
  // use lambda expression for the SLOT (could do that from QtDesigner)
  // mechanism for calling overload method (default value is E. McNeil data)
  connect(ui->_phi1, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          [=](double aNewValue) { ui->_phi1->setValue(aNewValue); });

  // NOTE
  //  GUI development we use QtDesigner for prototyping.
  //  For some reasons, i cannot connect with slot in the action editor
  //  i have to write it here, since this project is just a prototype for
  //  the first version of DamBreak++, it doesn't really matter.
  connect(ui->action_Exit, &QAction::triggered, qApp, QApplication::quit);
  connect(ui->action_run, SIGNAL(triggered()), this, SLOT(runSimulation()));
  connect(ui->action_doOneStep, SIGNAL(triggered()), this,
          SLOT(runStepbyStep()));
  connect(ui->actioninitialize, SIGNAL(triggered()), this, SLOT(initSim()));

  // connect(ui->_msgText, SIGNAL(textChanged()), this,
  //         SLOT(writeMsg2Gui(QString)));
  // at initialization
  ui->_msgText->setText("Ready to launch the simulation data");

  // validating GUI (add more algorithm in the next version)
  ui->convecflux_combo->addItem(QString{"HLL Solver (E. McNeil)"});
  ui->reconstr_comboBox->addItem(QString{"MUSCL Slope Limiter"});
  // ui->rhsalgo_combo->addItem();
  ui->discrdata_combo->addItem(QString{"emcneil"});
  ui->discrdata_combo->addItem(QString{"hudson"});

#if 0
  // Some tests (QThread is not a thread, its a wrapper around the thread)
  // provide services with signal and slot.
  //
  dbpp::Worker* worker = new dbpp::Worker();
  worker->moveToThread(&workerThread);
  // The connect() series here is the most crucial part. The first connect() line hooks up
  // the error message signal from the worker to an error processing function in the main thread.
  // The second connects the thread’s started() signal to the processing() slot in the worker, causing it to start.
  connect(worker, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
//  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(this, &DBppGuiAppl15::operate, worker, &dbpp::Worker::doWork);
  connect(worker, &dbpp::Worker::resultReady, this, &DBppGuiAppl15::handleResults);
  connect(worker, SIGNAL(finished()), &workerThread, SLOT(quit()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
 // connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
  workerThread.start();
#endif
}

DBppGuiAppl15::~DBppGuiAppl15() {
  delete ui;
  //  delete m_waveSim;

  // release all resources
  // dbpp::GlobalDiscretization::instance()->release();

  // close logger (remember to initialize before, because
  // it cause an thrown exception when close fclose(nullptr))
  // dbpp::Logger::instance()->Shutdown();
  // dbpp::Logger::instance()->release();

  // simulation manager released
  // dbpp::Simulation::instance()->release();

  // Simulation debug file release
  // dbpp::DbgLogger::instance()->release();

  // workerThread.quit();
  // workerThread.wait();
}

// slot
void DBppGuiAppl15::runSimulation() {
#if 0
  // GUI is 'ON' but want launch a new computation
  if( dbpp::Simulation::instance()->isRunning())
  {
    if( nullptr!= dbpp::GlobalDiscretization::instance())
    {
      // release all resources
      dbpp::GlobalDiscretization::instance()->release();
    }
    m_waveSim->timeLoop();
  }
  else // simulator not running (open GUI first time)
  {
    dbpp::Simulation::instance()->setRunning(true);
    //m_waveSim->timeLoop();
    // create a thread to run simulation
    std::thread w_simThread(testThreadCall);
    w_simThread.join(); // wait for thread to finish
    dbpp::Simulation::instance()->setRunning(false);
    //delete m_waveSim; done in the dtor
  }
#endif

  QMessageBox msgBox;
  msgBox.setText("Simulation completed successfully!");
  msgBox.exec();
}

void DBppGuiAppl15::runStepbyStep() {
  // to do add code here
}

// call initialize(from simulator) time initialization, water level,
// IC of simulator and create folder if don't exist and file for saving
// result. It takes the active algorithm name to set file name. NOTE by
// default the simulator has status of GUI mode which means that the user need
// to create numerical scheme and section list. It doesn't make sense but its
// the way it is. Important: simulator default algorithm is "EMcNeil1d_mod"
// call activeAlgo. Since default algo name is set at initialization, if user
// don't change it, file name is set to default.
void DBppGuiAppl15::initSim() {
#if 0
  auto w_currDiscrData = ui->discrdata_combo->currentText();
  // auto w_data = dbpp::DamBreakData::DiscrTypes::emcneil;  debug purpose
  if (w_currDiscrData == QString{"emcneil"}) {
    dbpp::Simulation::instance()->setActiveDiscretization(
        dbpp::DamBreakData::DiscrTypes::emcneil);
  } else // current version support those 2 type of dambreak data
  {
    dbpp::Simulation::instance()->setActiveDiscretization(
        dbpp::DamBreakData::DiscrTypes::hudson);
  }

  // set simulation active numerical method
  dbpp::Simulation::instance()->setAlgorithmName(
      m_waveSim->getActiveAlgorithm());

  // DESIGN NOTE we need to initialize the simulation bean first
  // Read data from file or from the GUI
  // dbpp::Simulation::instance()->setCFL(m_waveSim->getCFL()); // hard coded
  // for debugging

  // initialize the simulator
  m_waveSim->setSimulatorMode(dbpp::Wave1DSimulator::eSimulationMode::guiMode);
  m_waveSim->scan(); // we set some parameters of the simulation

  // basically set water level and state variables init values
  // Also create list of sections and numerical scheme/method
  // NOTE no sense to create those things inside
  // shall let user create it
  m_waveSim->initialize();

  // Create the global discretization for this simulation
  // IMPORTANT in  the current version we create discretization
  // based on E. McNeil code (C-version), but it make no sense
  // the global discretization responsibility to provides an
  // interface (access point) to component of the global discr.
  //  const dbpp::ListSectFlow* w_test = m_waveSim->getListSectionFlow();
  dbpp::GlobalDiscretization::instance()->init(m_waveSim->getListSectionFlow());
  auto w_msg1 = "Global Discretization initialization completed";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg1));

  // auto check123 = dbpp::Simulation::instance()->getActiveDiscretization();
  // debug purpose IMPORTANT!!!! Call boundary cond. applyBC()? b.c. is used in
  // the numerical scheme need to set amont/aval nodal values!!! this need to be
  // validated!
  dbpp::GlobalDiscretization::instance()->gamma().applyBC();

  // logging message to file
  auto w_msg2 = "Wave Simulator initialization completed";
  dbpp::Logger::instance()->OutputSuccess(const_cast<char *>(w_msg2));
#endif

  QMessageBox msgBox;
  msgBox.setText("Simulation initialization done!");
  msgBox.exec();
}

void DBppGuiAppl15::setNbIterationsMax() {
  // set time loop iterations
  // dbpp::Simulation::instance()->setNbIterationMax(ui->iterations->value());
  // m_waveSim->setIterationNumberMax(ui->iterations->value());
}

void DBppGuiAppl15::setInitialWaveProfile() {
  // to do add code here
}

void DBppGuiAppl15::setUpstream() {
  //  m_waveSim->setPhi1(ui->_phi1->value());
}

void DBppGuiAppl15::setDownstream() {
  // m_waveSim->setPhi0(ui->_phi0->value());
}

void DBppGuiAppl15::setShockLoc() {
  // m_waveSim->setShockLocation(ui->_shockloc->value());
}

void DBppGuiAppl15::setDamBreakData(
    QString aDisrData) { // pass string const QString&?
  if (aDisrData.isEmpty())
    return; // debugging purpose
  // dbpp::Simulation::instance()
}

// void DBppGuiAppl15::writeMsg2Gui(const QString &aMsg2Write) { // not in use
// anymore
// overwrite mode as default
// ui->_msgText->append(aMsg2Write);
//}

void DBppGuiAppl15::about() {
  QMessageBox::about(this, tr("About DamBreak++"),
                     tr("DamBreak++ is an application to rapid prototype "
                        "simulation of the damBreak problem"));
}

void DBppGuiAppl15::handleResults(const QString &aMsg2Write) {
  // overwrite mode as default
  // ui->_msgText->append(aMsg2Write);
}

void DBppGuiAppl15::writeSettings() {
  QSettings settings(QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());
  settings.setValue("geometry", saveGeometry());
}
