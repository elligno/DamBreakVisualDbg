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
#if 0
class DamBreakVisualDbg : public QMainWindow
{
    Q_OBJECT

public:
    DamBreakVisualDbg(QWidget *parent = nullptr);
    ~DamBreakVisualDbg();

private:
    Ui::DamBreakVisualDbg *ui;
};
#endif

namespace {
// Access copies of local variables at the time the lambda was created.
// Every call to make_offseter returns a new lambda function object through
// the std::function< > function wrapper. This is the safest form of local
// variable capture.
std::function<int(int)> make_offseter(int aOffset) {
  return [=](int j) { return aOffset + j; };
}

template <typename T> T read(std::istringstream &is) {
  T t;
  is >> t;
  return t;
}

template <typename... Args> std::tuple<Args...> parse(std::istringstream &is) {
  return std::make_tuple(read<Args>(is)...);
}

// Read file format "full result" for each simulation time
// Use utility function to parse line (return a tuple with
// value in reverse order tuple index 4->0 instead of 0->4)
// Return a vector of values corresponding at simulation time.
template <typename... Args> // let compiler deduce return type (maybe i should
                            // use decltype(auto)?)
                            auto
                            extractLineValuesTmplt(QTextStream &afileStream,
                                                   QString &&aFirstLine) {
  QString w_line2Read{std::move(aFirstLine)};
  QVector<std::tuple<Args...>> w_vecoftpl;
  w_vecoftpl.reserve(101); // hard coded!!!
  do {
    // replace sequence of white by a single one
    auto w_lineTrimmed = w_line2Read.simplified().toStdString();
    std::istringstream w_iis{
        w_lineTrimmed}; // step to next white space one after one
    auto w_tplResult =
        parse<Args...>(w_iis); // parse a line with format X|H|A|...
    w_vecoftpl.push_back(
        std::move(w_tplResult)); // not sure we are moving something!! No, see
                                 // Scott Meyer's book item #25
  } while (afileStream.readLineInto(&w_line2Read) && !w_line2Read.isEmpty());

  return w_vecoftpl;
}

// See Noe above
// reverse tuple index order (problem when parsing each line and return a tuple)
template <class Tuple, std::size_t... Is>
Tuple tuple_inverse_idx_order(const Tuple &t, std::index_sequence<Is...>) {
  return std::make_tuple(std::get<Is>(t)...);
}
} // namespace

namespace qplot {
using dbltpl = std::tuple<double, double, double, double, double>;
// using dbltpl3 = std::tuple<double, double, double>;

class DamBreakVisualDbg
    : public QMainWindow // shall inherit from jb::Observer class
{                        // dll library and implement update() method
  Q_OBJECT

public:
  // Support of 3 format (default one is debug)
  // enum class eFileFmt {debug, Variable Profile, Full result};
  enum class eFileFormat {
    dbgFormat,
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
  DamBreakVisualDbg(/*GraphType aGraph=parabola, */ QWidget *parent = 0);
  ~DamBreakVisualDbg();

  void setPlotParams(const QString &aYaxisLabel, double aYmax);
  //		void createAddButtons();
  // ...
  void setYaxis(QString aYaxis) { m_yAxisLabel = aYaxis; }
  QString getYaxis() { return m_yAxisLabel; }

protected:
  void readDataFromFile(QFile &aFile2read);
  void readFullResultFile(QFile &aFile2read);
  void extractProfileFromFullRes(int aListIndex);
  void readFiles2Cmp();
  void extractSimTimes();

private slots:
  void plot2dButton();  //
  void saveToFile();    // save as .jpeg file
  void loadFromFile();  // file according to user selection
  void plot2DProfile(); // stepping mode
  void getProfileTimes(const QString &);

private:
  Ui::DamBreakVisualDbg *ui;
  QString m_yAxisLabel; // y axis label: H,V,Q,Fr,... default is H
  std::pair<double, double> m_yAxisRng; // x-axis/y-axis
  std::pair<double, double> m_xAxisRng; // x-axis/y-axis
                                        //	void setColorComboBox();
  void loadData2Show();
  void dambreak0Curve();
  void setMultipleGraph();
  void plot2DProfile(
      const eGraphFmt aGraphFmt); // plot 2D graph inside the GUI (main window)

  // GUI ... to be completed
  QCustomPlot *m_customPlot; // call qt custom plot package
  QCustomPlot *m_plot2d{nullptr};
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

  void dambreak2Curve();
  QGridLayout *singleLayout();

  // testing stuff
  int m_stepGraphCounter{5};

  void setAllColorComboBox();
  void setAllLineTypeComboBox();
  std::array<QHBoxLayout *, 5>
  setAllHboxLayout(std::initializer_list<std::string> aListGraphName);

  /** */
  struct GraphPrm {
    //
    // GraphPrm( TestQTApp * a) : m_baseClass(a) {}

    template <typename... Params> struct dummy1 {
      std::tuple<Params...> m_data;
    };

    // variable template
    template <typename... Prms>
    using dummyVec = QVector<GraphPrm::dummy1<Prms...>>;

    // two-curve format: X|Var1|X|Var2
    dummyVec<double, double, double, double> m_twoCurveData2View;

    struct CurvesData {
      // Labels (Axis, legend name, ...)
      // x-coordinate
      // Variable 1 data
      // Variable 2 data
    };

    // Parameter to manage the graph plotting format/reading
    QString m_graphName;
    QString m_yaxisLabel;
    QFile m_file1;                  /*multiple file for multi curve support*/
    QFile m_file2;                  /*multiple file for multi curve support*/
    mapValU12 m_mapValUp;           /*map key is iteration number=1,...,N*/
    mapValU12 m_mapValU;            /*iteration number=1,...,N*/
    QVector<double> m_qvecX;        /*store values as a pair of X/Value*/
    QVector<double> m_ValuesCurve1; /**/
    QVector<double> m_ValuesCurve2; /**/
    double m_yMaxValue;             /**/
    QVector<pairtimeline> m_vecTimeValues; /*< store data to be visualized at a
                                              given time (x-coord/value)*/
    vecpair m_xVar1; /* plot variable profile: X/H, X/Q, X/V user select */
    vecpair m_xVar2; /* plot variable profile: X/H, X/Q, X/V user select */
    eFileFormat m_filFmt{
        eFileFormat::dbgFormat}; /* file contain the final profile that user
                                    want to view*/
    eGraphFmt m_graphFmt{eGraphFmt::XH};      /* default */
    eDataType m_dataType{eDataType::EMcNeil}; /* default */
    QStringList m_simTimesList;
    constexpr static int iterationStep = 5;
    void setYaxisLabel(const eGraphFmt aYaxis2Set) {
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
        //         case TestQTApp::eGraphFmt::XF: to be implemented Froude
        //         Number
        //           m_yaxisLabel = QString{ "V(Water Velocity)" };
        //           //w_yAxisLabel[0]; break;
      default:
        break;
      }
    }

  private:
    // testing some stuff to implement
    // TestQTApp* m_baseClass;  call default ctor (not available)
  } m_graph2D;

  // ...
  // std::unique_ptr<GraphPrm> m_graphPrm;

  // what for?
  // Y Axis label (set as static for our nested class to have access to it??)
  // but onoy use by static function!!!
  QStringList w_yAxisLabel{QString{"H(Water Depth)"}, QString{"Velocity"},
                           QString{"Discharge"}, QString{"Froude Number"},
                           QString{"Energy Slope"}};

#if 0 // future development
        void createMenu()
        {
            QMenuBar* menuBar = new QMenuBar;

            QMenu* fileMenu = new QMenu(tr("&File"), this);
            QAction* exitAction = fileMenu->addAction(tr("E&xit"));
            menuBar->addMenu(fileMenu);

//			QObject::connect( exitAction, &QAction::triggered, this, &QDialog::accept);
        }
#endif
  // template<typename ...Args>
  auto
  extractLineValues(QString &aStrRead /*, QVector<dbltpl>& aVecOfValues*/) {
    // remove all white space, but its not what i want
    // but i think i need to do that
    // Returns a string that has whitespace removed from the start and the end,
    // and that has each sequence of internal whitespace replaced with a single
    // space.
    auto w_lineTrimmed = aStrRead.simplified().toStdString();
    std::istringstream w_iis{w_lineTrimmed};
    auto w_tplResult = parse<double, double, double, double, double>(w_iis);
    return w_tplResult;
  }

#if 0
    {
        // just testing something with variadic template (try to understand!!)
        template<typename ...Args>
        auto extractLineValuesTest( QString& aStrRead/*, QVector<dbltpl>& aVecOfValues*/)
        {
          QVector < std::tuple<Args...> > w_vecoftpl;
          auto w_lineTrimmed = aStrRead.simplified().toStdString();
          std::istringstream w_iis{ w_lineTrimmed };
          auto w_tplResult = parse<Args...>(w_iis);
          w_vecoftpl.push_back(w_tplResult);
        }
    }
#endif // 0

  bool w_bStartReadVal;
};
} // namespace qplot
