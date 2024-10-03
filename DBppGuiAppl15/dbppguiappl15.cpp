#include "dbppguiappl15.h"
#include "ui_dbppguiappl15.h"

// C++ include
//#include <thread>
// Qt includes
#include <QBoxLayout>
#include <QDebug> // debug helper
#include <QGroupBox>
#include <QMessageBox>
#include <QSettings>
#include <QTextEdit>

// Library includes
#include "Discretization/dbpp_GlobalDiscretization.h"
#include "SfxTypes/dbpp_DamBreakData.h"
#include "SfxTypes/dbpp_Simulation.h"
#include "Utility/dbpp_Hydro1DLogger.h"
#include "Utility/dbpp_TestLogger.h"
#include "dbpp_Wave1DSimulator.h"

// test stuff
//#include "Utility/dbpp_Worker.h"

// dbpp::Wave1DSimulator* m_waveSim {nullptr};  = new dbpp::Wave1DSimulator; ???

// ++++++++++++++++++++++++++++++++++++++++++
// Thread function
// void testThreadCall() {
//   m_waveSim = new dbpp::Wave1DSimulator;
//   m_waveSim->timeLoop();
// should i delete it the pointer
//}

WorkerThread::WorkerThread(std::function<void()> functionToExecute,
                           QObject *parent)
    : QThread(parent), m_FunctionToExecute(functionToExecute) {}

void WorkerThread::run() {
  if (m_FunctionToExecute) {
    m_FunctionToExecute();
  }
}
// ++++++++++++++++++++++++++++++++++++++++++

DBppGuiAppl15::DBppGuiAppl15(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::DBppGuiAppl15), // ..
      m_waveSim{new dbpp::Wave1DSimulator}            // create the simulator
{

  // Organization name
  qApp->setOrganizationName(QString("Elligno Inc"));

  // Application name
  qApp->setApplicationName(QString("DamBreak++ Simulator"));

  // Some sanity check (we shall have 1 window, main)
  // Widget I am not sure, a bunch of them
  QWindowList w_checkListWin = QApplication::allWindows();
  Q_ASSERT(w_checkListWin.size() == 0);

  QWidgetList w_checkListWgt = QApplication::allWidgets();
  Q_ASSERT(w_checkListWgt.size() == 1);

  qApp->setApplicationVersion(QString{"0.1"});
  qApp->aboutQt(); // just a test for access global variable application

  // auto *quit = new QAction("&Quit", this);

  //  QMenu *file = menuBar()->addMenu("&File");
  //   file->addAction(quit);
  //

  //
  // DamBreak Application Configuration
  //

  // Create a logger to write error message
  dbpp::Logger::instance()->Init();

  // Read from file bathymetry value and Manning coefficient (not sure)

  // set the numerical algorithm to be used (user selection or default)
  // set the CFL number for this simulation (default or user selection)
  // set the discretization to be used for this simulation, default or user
  // selection

  // setup GUI functionality
  ui->setupUi(this);

  // Still need this?
  // NOTE not too sure about this one, not calling the slot "setUpstream()"
  // use lambda expression for the SLOT (could do that from QtDesigner)
  // mechanism for calling overload method (default value is E. McNeil data)
  connect(ui->_phi1, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
          [=](double aNewValue) { ui->_phi1->setValue(aNewValue); });

  // NOTE (apply VS15 not sure in QtCreator env)
  //  GUI development we use QtDesigner for prototyping.
  //  For some reasons, i cannot connect with slot in the action editor
  //  i have to write it here, since this project is just a prototype for

  //  the first version of DamBreak++, it doesn't really matter.
  connect(ui->action_Exit, &QAction::triggered, qApp, QApplication::quit);
  connect(ui->action_run, SIGNAL(triggered()), this, SLOT(runSimulation()));
  connect(ui->action_doOneStep, SIGNAL(triggered()), this,
          SLOT(runStepbyStep()));
  connect(ui->actioninitialize, SIGNAL(triggered()), this, SLOT(initSim()));
  connect(ui->cflNumSpinBox, SIGNAL(valueChanged(double)), this,
          SLOT(setCFLValue(double)));

  // at initialization
  ui->_msgText->setText("Ready to launch the simulation data");

  // validating GUI (numerical algorithm)
  ui->numalgo_combo->addItem(QString{"EMcNeil1D_mod"});
  ui->numalgo_combo->addItem(QString{"EMcNeil1D_f"});
  ui->numalgo_combo->addItem(QString{"TestEMcNeilVec"});
  ui->numalgo_combo->addItem(QString{"TestBcSectF"});
  ui->numalgo_combo->addItem(QString{"TestNewAlgo"});

  ui->reconstr_comboBox->addItem(QString{"MUSCL Slope Limiter"});
  ui->rhsalgo_combo->addItem(QString{"TestRhsImpl"});
  //
  ui->discrdata_combo->addItem(QString{"emcneil"});
  ui->discrdata_combo->addItem(QString{"hudson"});

#if 0 // to do in future version
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
  delete m_waveSim;

  // release all resources
  dbpp::GlobalDiscretization::instance()->release();

  // close logger (remember to initialize before, because
  // it cause an thrown exception when close fclose(nullptr))
  dbpp::Logger::instance()->Shutdown();
  //  dbpp::Logger::instance()->release();

  // simulation manager released
  dbpp::Simulation::instance()->release();

  // Simulation debug file release
  dbpp::DbgLogger::instance()->release();
}

// some thread test!!
void DBppGuiAppl15::runSimulation() {
  // we start it in a separate thread to keep the UI responding
  auto workerThread = new WorkerThread([=]() { m_waveSim->timeLoop(); }, this);
  connect(workerThread, &WorkerThread::finished, workerThread,
          &QObject::deleteLater);
  connect(workerThread, &WorkerThread::finished, this,
          [&] { dbpp::Simulation::instance()->setRunning(false); });
  dbpp::Simulation::instance()->setRunning(true);
  workerThread->start(); // emit "started()" signal before run()

  // ui->_msgText->append(QString{"Simulation completed successfully!"});
  ui->_msgText->append(QString{"Simulation is running in a separate thread!"});
  QMessageBox msgBox;
  msgBox.setText("Simulation is running in a separate thread!");
  msgBox.exec();
}

#if 0
// slot
void DBppGuiAppl15::runSimulation() {
  // assert(0.5 == m_waveSim->getCFL()); debugging purpose (default value)
  // GUI is 'ON' but want launch a new computation
  //  if (dbpp::Simulation::instance()->isRunning()) {
  //    if (nullptr != dbpp::GlobalDiscretization::instance()) {
  //      // release all resources
  //      dbpp::GlobalDiscretization::instance()->release();
  //    }
  //    m_waveSim->timeLoop();
  //  } else // simulator not running (open GUI first time)
  //  {
  dbpp::Simulation::instance()->setRunning(true);

  m_waveSim->timeLoop();

  dbpp::Simulation::instance()->setRunning(false);
  // delete m_waveSim; done in the dtor
  //  }

  // create a thread to run simulation
  // std::thread w_simThread(testThreadCall);
  //   w_simThread.join(); // wait for thread to finish

#if 0
    // wave profile
    auto listSectionsFlow = createListSectionsFlow();

    // made of physical objects (sections flow)
    auto phySystem = createPhysicalSystem();

    // configure initial solution
    phyConfigure->configure(phySystem, simulation);

    // initialize simulator list section flow
    m_listSectionFlow = phySystem->getListSectionFlow();

    // time stepping base on initial solution??
    // auto delta = calculateDt(phySystem->getStateVectorField());

    // initialized in initSim from gui user selection
    dbpp::TimePrm w_timePrm {start,delta,stop};
    w_timePrm.setFinalTime(ui final time); // overwrite default

    // May be create the Numerical Method here instead of retrieving it from Gbl Discr
    // selected from GUI (current method is based on calculFF(), how the numerical flux)
    // at cell face is evaluated. Abstract class and user override 'calculFF()' and
    // implement the face flux algorithm. In the current version, we have already
    // a drop-downbox with the list of available method. Since we plan to use many
    // implementation of calculFF(), also of the source term, we may want list all of them
    // we could run multiple runs with different method and compare result. There is
    // already a slot call 'setActiveNumericalMethod()',

    // retrieve numerical method (it uses a solver to solution the discrete equation)
    auto w_numodel = dbpp::GlobalDiscretization::instance()->num_rep();

    // Set the solver method (solving discrete equation)
    w_numodel->setSolver(new SWESolver);

    // user gui selection (should we pass the solver to PhysicalAlgorithm???)
    w_numodel->odesolver()->addPhysicalAlgorithm(EMcNeilPhysicalAlgorithm);
    // add more if there are more
    // set active algorithm for this simulation

    // register physical for this simulation
    w_numodel->odesolver()->registerPhysicalSystem(phySystem);

    // time stepping
    while (w_timePrm.finished() ||
           dbpp::Simulation::instance()->getNbIterationMax()) {
      // pass time step to advance and global discretization
      w_numodel->advance(dbpp::GlobalDiscretization::instance(), w_timeStep);

      // since our numerical model use shared pointer (setinitSln(m_u,...))NO!!!
      // can retrieve data from our numerical model ("m_u" is StateVector)NO!!!!
      // auto w_stdVecH = m_waveSim->getWaveProfile(odesolver->physystem()->ListSectionsFlow());

      // physical measure on the system
      // phyMeasur->take(odesolver->physystem(), Simulation);

      // save measure to data store
      // fileDataStore->save(phyMeasur)

      // time stepping next iteration
      // auto delta = calculateDt(physystem->getStateVectorField());

      // send message to gui and plot wave profile

      // let this thread sleep allowing time to complete task
      // this_thread::sleep(5s);
    }
#endif

  ui->_msgText->append(QString{"Simulation completed successfully!"});

  QMessageBox msgBox;
  msgBox.setText("Simulation completed successfully!");
  msgBox.exec();
}
#endif

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

  // set simulation active numerical method (What for?)
  dbpp::Simulation::instance()->setAlgorithmName(
      m_waveSim->getActiveAlgorithm());

  // debug purpose
  auto algoName = dbpp::Simulation::instance()->getAlgorithmName();

  // DESIGN NOTE we need to initialize the simulation bean first
  // Read data from file or from the GUI
  dbpp::Simulation::instance()->setCFL(m_waveSim->getCFL()); // hard coded

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

  // debug purpose IMPORTANT!!!! Call boundary cond. applyBC()? b.c. is used in
  // the numerical scheme need to set amont/aval nodal values!!! this need to be
  // validated!
  dbpp::GlobalDiscretization::instance()->gamma().applyBC();

  // logging message to file
  dbpp::Logger::instance()->OutputSuccess(
      std::string{"Wave Simulator initialization completed"}.data());

  ui->_msgText->append(QString{"Simulation initialization done!"});

  //  QMessageBox msgBox;
  //  msgBox.setText("Simulation initialization done!");
  //  msgBox.exec();
}

void DBppGuiAppl15::setNbIterationsMax(int aIterMax) {
  // set time loop iterations
  dbpp::Simulation::instance()->setNbIterationMax(
      aIterMax /*ui->iterations->value()*/);
  // m_waveSim->setIterationNumberMax(aIterMax);
}

void DBppGuiAppl15::setInitialWaveProfile() {
  // to do add code here
}

void DBppGuiAppl15::setUpstream(double aPhi1) { m_waveSim->setPhi1(aPhi1); }
void DBppGuiAppl15::setDownstream(double aPhi0) { m_waveSim->setPhi0(aPhi0); }

void DBppGuiAppl15::setShockLoc(double aShockValue) {
  m_waveSim->setShockLocation(aShockValue /*ui->_shockloc->value()*/);
}

// not sure about this one
void DBppGuiAppl15::setDamBreakData(QString aDisrData) {
  if (aDisrData.compare(QString{"emcneil"})) {
    dbpp::Simulation::instance()->setActiveDiscretization(
        dbpp::DamBreakData::DiscrTypes::emcneil);
  } else {
    QMessageBox w_msg;
    w_msg.setText(QString{"Data Not Supported"});
    w_msg.exec();
  }
}

void DBppGuiAppl15::setActiveNumericalMethod(QString aNumMethod) {
  m_waveSim->setActiveAlgorithm(aNumMethod.toStdString());
}

void DBppGuiAppl15::setCFLValue(double aCflValue) {
  // m_waveSim->setCFL(ui->cflNumber->value());
  // auto aa = ui->cflNumSpinBox->value();
  m_waveSim->setCFL(aCflValue);
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
  ui->_msgText->append(aMsg2Write);
}

void DBppGuiAppl15::writeSettings() {
  QSettings settings(QCoreApplication::organizationName(),
                     QCoreApplication::applicationName());
  settings.setValue("geometry", saveGeometry());
}
