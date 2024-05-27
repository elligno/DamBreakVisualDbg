#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// forward declarations
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QTextEdit;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QSpinBox;
class QLabel;
class QDialogButtonBox;

// forward declaration
class QCustomPlot;

namespace qcplot {

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  // useful alias
  using mapValU12 =
      std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;

  using Qvecpair = QVector<std::pair<double /*x-coord*/, double /*H-value*/>>;

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void plot2dButton(); // ...
  void saveToFile();
  void loadFromFile();
  void plot2DProfile(); // plot 2D graph inside the GUI

private:
  Ui::MainWindow *ui;

  void loadData2Show();
  void setMultipleGraph();

  // ... to be completed
  //  QVector<double> m_x;
  //  QVector<double> m_h;
  // QString m_fileName;

  // GUI ... to be completed
  QCustomPlot *m_customPlot{nullptr}; // call qt custom plot package
  // QPushButton *m_findButton{nullptr};   find file in a repository
  //  QPushButton *m_closeButton{nullptr}; // close the GUI
  // QPushButton *m_stopButton{nullptr};   stop plotting
  QPushButton *m_plotButton{nullptr}; // potting
  QWidget *m_centralWidget{nullptr};  // central area of the win widget
  QTextEdit *m_bigEditor{nullptr};    //
  QCustomPlot *m_plot2d{nullptr};
  // Actual vedrsion support 5 graphs
  static constexpr int nbGraph = 5;
  QComboBox *m_colorCurve[nbGraph];
  QComboBox *m_lineType[nbGraph];
  QCheckBox *m_activate[nbGraph];
  QSpinBox *m_iterNo[nbGraph];
  QLineEdit *m_graphName[nbGraph];
  // testing stuff ??
  int m_stepGraphCounter{5};
  // Helpers
  QGridLayout *singleLayout();
  void setAllColorComboBox();
  void setAllLineTypeComboBox();
  std::array<QHBoxLayout *, 5>
  setAllHboxLayout(std::initializer_list<std::string> aListGraphName);
  void PrepareSimRes2View(mapValU12 &aMapU12p, mapValU12 &aMapU12,
                          const std::string &aFilename);
  void loadData2Cmp(const QStringList &aFileNameList);
  void setTwoGraphCmp();
  /** */
  struct GraphPrm {
    // some testing stuff
    QString m_graphName;
    QString m_simFileName; // use QFile instead!!! not a string!!!
    mapValU12 m_mapValUp;  /*map key is iteration number=1,...,N*/
    mapValU12 m_mapValU;   /*iteration number=1,...,N*/
    QVector<double> m_qvecX;
    QVector<double> m_hvalExact;
    QVector<double> m_hvalComputed;
  } m_graph2D;
};
} // namespace qcplot
#endif // MAINWINDOW_H
