#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class DBppGuiAppl15;
}
QT_END_NAMESPACE

#if 0  // original code at project creation before merged VS15 stuff
class DBppGuiAppl15 : public QMainWindow
{
    Q_OBJECT

public:
    DBppGuiAppl15(QWidget *parent = nullptr);
    ~DBppGuiAppl15();

private:
    Ui::DBppGuiAppl15 *ui;
};
#endif // 0

// namespace dbpp { class Wave1DSimulator; }

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
  void setNbIterationsMax();
  void setInitialWaveProfile();
  void setUpstream();
  void setDownstream();
  void setShockLoc();
  void setDamBreakData(QString aDisrData);
  // void writeMsg2Gui(const QString &);  don't need that!!
  // void setReconstructionAlgorithm();
  void about();
  void handleResults(const QString &aMsg2Write);
signals:
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

  // dbpp::Wave1DSimulator* m_waveSim;
  /**< Simulator instance*/

  //	std::unique_ptr<jb::TimePrm> m_tprm;
  /**< Simulation time  parameters*/

  double m_finalTime;
  /**< Simulation final time*/
};
