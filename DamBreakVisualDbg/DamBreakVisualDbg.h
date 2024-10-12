#pragma once

// C++ includes
#include <functional>
#include <sstream>
// Qt includes
#include <QFile>
#include <QMainWindow>
#include <QTextStream>

// forward declarations
class QDialogButtonBox;
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

// forward declaration
class QCustomPlot;

QT_BEGIN_NAMESPACE
namespace Ui {
class DamBreakVisualDbg;
}
QT_END_NAMESPACE

// App include
#include "GraphVisualizer.h"

namespace qplot {
using dbltpl = std::tuple<double, double, double, double, double>;
// using dbltpl3Col = std::tuple<double, double, double>;

/**
 */
class DamBreakVisualDbg : public QMainWindow {
  Q_OBJECT

public:
  /** Support of 3 format (default one is debug)*/
  enum class eFileFormat {
    dbgFormat, /**< */
    var_profile,
    full_result,
    exact_computed
  };
  enum class eGraphFmt { XH, XQ, XV };
  enum class eDataType { EMcNeil, Hudson };
  enum class eStepMode { stepBy, None }; // default is none

public:
  // useful typedef (actually we don't really need it, just the "auto" keyword)
  using mapValU12 =
      std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;
  // another way to store data {U1p,U2P} as a pair, read each line and store
  // data also could be profile format {x,val}
  using vecpair = QVector<std::pair<double, double>>;
  using mapIterUValue = std::map<unsigned, vecpair>;
  // using mapIterUValue = std::map<unsigned, std::vector<std::tuple<double,
  // double>>>;
  using pairxVar = std::pair<QVector<double> /*x-coord*/,
                             QVector<double> /*var value to cmp*/>;
  // using twographcmp = std::tuple
  using pairtimeline =
      std::pair<double /*Time*/, QVector<dbltpl> /*line of values*/>;

public:
  DamBreakVisualDbg(QWidget *parent = 0);
  ~DamBreakVisualDbg();

  // ...
  void setYaxis(QString aYaxis) { m_yAxisLabel = aYaxis; }
  QString getYaxis() { return m_yAxisLabel; }

protected:
  void extractProfileFromFullRes(int aListIndex);

private slots:
  void plot2dButton();  //
  void saveToFile();    // save as .jpeg file
  void loadFromFile();  // file according to user selection
  void plot2DProfile(); // stepping mode
  void getProfileTimes(const QString &);
  void getFileFmt(QString);
  void getGraphFmt(QString);

private:
  // Ui::DamBreakVisualDbg *ui;
  QString m_yAxisLabel; // y axis label: H,V,Q,Fr,... default is H
  std::pair<double, double> m_yAxisRng; // x-axis/y-axis
  std::pair<double, double> m_xAxisRng; // x-axis/y-axis
  // void setColorComboBox();
  // plot 2D graph inside the GUI (main window)
  void plot2DProfile(const eGraphFmt aGraphFmt);

  // GUI ... to be completed
  // QCustomPlot *m_customPlot; call qt custom plot package
  QCustomPlot *m_plot2d;
  QWidget *m_centralWidget; // central area of the win widget
  QTextEdit *m_bigEditor;   //

  // new implementation
  static constexpr int nbGraph = 5;
  QComboBox *m_colorCurve[nbGraph];
  QComboBox *m_lineType[nbGraph];
  QCheckBox *m_activate[nbGraph];
  QSpinBox *m_iterNo[nbGraph];
  QLineEdit *m_graphName[nbGraph];
  // GUI user select graph params
  QComboBox *m_fileFmtItem{nullptr};
  QComboBox *m_graphFmtItem{nullptr};
  QComboBox *m_simTimesItem{nullptr};

  // ... to be completed
  QGridLayout *singleLayout();

  // start at 0
  int m_stepGraphCounter{};

  void setAllColorComboBox();
  void setAllLineTypeComboBox();
  std::array<QHBoxLayout *, 5>
  setAllHboxLayout(std::initializer_list<std::string> aListGraphName);

  /** */
  struct GraphPrm { // this is an aggregate, since C++17, can inherit from
    // Parameter to manage the graph plotting format/reading
    QString m_graphName;
    QString m_yaxisLabel;
    eFileFormat m_filFmt{
        eFileFormat::dbgFormat}; /* file contain the final profile that user
                                    want to view*/
    eGraphFmt m_graphFmt{eGraphFmt::XH};      /* default */
    eDataType m_dataType{eDataType::EMcNeil}; /* default (temporary, we need to
                                                 be able to set this field)*/
    QStringList m_simTimesList;
    // inline and a definition since C++17
    static constexpr int iterationStep = 5;

    // NOTE i do not it's in use, but I keep might need it in the future
    void setYaxisLabel(/*const eGraphFmt aYaxis2Set*/) {
      switch (m_graphFmt) {
      case DamBreakVisualDbg::eGraphFmt::XH:
        m_yaxisLabel = QString{"H(Water Depth)"}; // w_yAxisLabel[0];
        break;
      case DamBreakVisualDbg::eGraphFmt::XQ:
        m_yaxisLabel = QString{"Q(Water Discharge)"}; // w_yAxisLabel[0];
        break;
      case DamBreakVisualDbg::eGraphFmt::XV:
        m_yaxisLabel = QString{"V(Water Velocity)"}; // w_yAxisLabel[0];
        break;
        //  case TestQTApp::eGraphFmt::XF: to be implemented Froude Number
        //   m_yaxisLabel = QString{ "V(Water Velocity)" };
        //   w_yAxisLabel[0]; break;
      default:
        break;
      }
    }
  } m_graph2D;

  // Y Axis label (set as static for our nested class to have access to it??)
  // but only use by static function!!!
  QStringList w_yAxisLabel{QString{"H(Water Depth)"}, QString{"Velocity"},
                           QString{"Discharge"}, QString{"Froude Number"},
                           QString{"Energy Slope"}};

  bool w_bStartReadVal;         /**< */
  QStringList m_simTimesList;   /**< */
  CurvesData2Show m_curvData;   /**< */
  GraphVisualizer w_graph2Plot; /**< */

  // Helpers function
  void loadDebugAndSetFmt();
  void loadFinalProfileAndSetFmt();
  void loadFullResultAndSetFmt();
  void loadExactCompAndSetFmt();
};
} // namespace qplot
