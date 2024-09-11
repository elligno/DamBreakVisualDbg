#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class DBppGuiAppl15;
}
QT_END_NAMESPACE

namespace dbpp {
class Wave1DSimulator;
}

/** Brief By default we inherit from QGuiApplication.
 * DamBreak app is a GUI to manage the simulation
 * parameters and algorithm to simulate the
 * dam-break problem (non linear wave propagation).
 * GUI is a wrapper around wave simulator that allow user
 * to easy configure simulation parameters and run scenario.
 * This is the first implementation as a prototype for further
 * development.
 */
class DBppGuiAppl15 : public QMainWindow {
  Q_OBJECT

  // QThread workerThread;

public:
  DBppGuiAppl15(QWidget *parent = Q_NULLPTR);

  ~DBppGuiAppl15();

private:
  Ui::DBppGuiAppl15 *ui;
  /**< Instance of dambreakapp ui*/

public slots:
  void runSimulation();
  void runStepbyStep();
  void initSim();
  // 	void saveSimulation();
  // 	void about(); //from menu
  // void onCFLValueChanged();
  void setCFLValue(double aCflValue);
  void setNbIterationsMax(int aIterMax);
  void setInitialWaveProfile();
  void setUpstream(double aPhi1);
  void setDownstream(double aPhi0);
  void setShockLoc(double aShockValue);
  void setActiveNumericalMethod(QString aNumMethod);
  void setDamBreakData(QString aDisrData);
  // void writeMsg2Gui(const QString &);  don't need that!! not sure yet
  // void setReconstructionAlgorithm();   maybe in the next re-factoring
  void about();
  void handleResults(const QString &aMsg2Write);
signals: // thread stuff
  void operate(const QString &);

private:
  /** ... */
  // void initialization();

  /** GUI initialization */
  void writeSettings();

  /** ... */
  bool maybeSave();

  QStringList m_listAlgo;
  /**< list of algorithm name*/

  dbpp::Wave1DSimulator *m_waveSim;
  /**< Simulator instance*/

  //	std::unique_ptr<jb::TimePrm> m_tprm;
  /**< Simulation time  parameters*/

  double m_finalTime;
  /**< Simulation final time*/
};
