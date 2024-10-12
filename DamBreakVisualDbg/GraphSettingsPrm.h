#pragma once

// C++ include
#include <tuple>
// Qt includes
#include <QFile>
#include <QString>
#include <QVector>
// App include
#include "Enumerators.h"

namespace qplot {

using dbltpl = std::tuple<double, double, double, double, double>;

// alias template
template <typename... prms> using qvectpl = QVector<std::tuple<prms...>>;

// another way to store data {U1p,U2P} as a pair, read each line and store
// data also could be profile format {x,val}
using vecpair = // could be qvectpl<double,double>;
    QVector<std::pair<double, double>>;

// useful typedef (actually we don't really need it, just the "auto" keyword)
using mapValU12 =
    std::map<unsigned, std::pair<std::vector<double>, std::vector<double>>>;

using mapIterUValue = std::map<unsigned, vecpair>;
// using mapIterUValue = std::map<unsigned, std::vector<std::tuple<double,
// double>>>;
using pairxVar = std::pair<QVector<double> /*x-coord*/,
                           QVector<double> /*var value to cmp*/>;
// using twographcmp = std::tuple
using pairtimeline =
    std::pair<double /*Time*/, QVector<dbltpl> /*line of values*/>;

struct BaseGraphSettingsPrm { // aggregate
  // Parameter to manage the graph plotting format/reading
  // 1-curve data
  QString m_graphName;                  /*graph title*/
  QString m_xaxisLabel{"x-coordinate"}; /*graph x-axis label*/
  QString m_yaxisLabel;                 /*graph y-axis label*/
  QFile m_file1;                        /*data from file*/
  vecpair m_xVar1; /* plot variable profile: X/H, X/Q, X/V user select */
};

struct TwoGraphSettingsPrm
    : BaseGraphSettingsPrm { // C++17 aggregate inheritance
  // plot profile for 2 curce data
  QFile m_file2;   /*data from file*/
  vecpair m_xVar2; /* plot variable profile: X/H, X/Q, X/V user select */
};

struct CurvesData2Show {
  using pairofvec =
      std::pair<std::vector<double> /*U1*/, std::vector<double> /*U2*/>;

  // x-coordinate ???
  // Variable 1 data
  // Variable 2 data
  vecpair m_xVar1;      /* plot variable profile: X/H, X/Q, X/V user select */
  vecpair m_xVar2;      /* plot variable profile: X/Hs, X/Q, X/V user select */
  mapValU12 m_mapValUp; /*map key is iteration number=1,...,N*/
  mapValU12 m_mapValU;  /*iteration number=1,...,N*/
  std::pair<double, double> m_HudsonXRng{0., 1.};
  std::pair<double, double> m_HudsonYRng{0.45, 1.1};
  std::pair<double, double> m_EMcNeilXRng{0., 1010.};
  std::pair<double, double> m_EMcNeilYRng{0., 10.};

  // return values for iteration number
  pairofvec getU12pValues(int aIterationNumber) const {
    auto w_mapVal = m_mapValUp.find(aIterationNumber);
    if (w_mapVal != m_mapValUp.end())
      return w_mapVal->second;

    return pairofvec(); // empty
  }

  QVector<double> m_qvecX; /*store values as a pair of X/Value*/
  // two-curve format: X|Var1|X|Var2
  qvectpl<double, double, double, double> m_twoCurveData2View;

  // return by value, C++17 copy elison is mandatory
  QVector<std::pair<double, double>> getVar1Var2Values() const {
    // coming
    QVector<std::pair<double, double>> w_vecPairValues;
    w_vecPairValues.reserve(m_xVar1.size());
    for (auto i = 0; i < m_xVar1.size(); ++i) {
      auto [x1, var1Val] = m_xVar1[i];
      auto [x2, var2Val] = m_xVar2[i];
      w_vecPairValues.push_back({var1Val, var2Val});
    }
    return w_vecPairValues;
  }

  pairxVar getxVar1() {
    pairxVar w_pairxvar;
    for (auto i = 0; i < m_xVar1.size(); ++i) {
      auto [x1, var1Val] = m_xVar1[i];
      w_pairxvar.first.push_back(x1);
      w_pairxvar.second.push_back(var1Val);
    }
    return w_pairxvar;
  }

  pairxVar getxVar2() {
    pairxVar w_pairxvar;
    for (auto i = 0; i < m_xVar2.size(); ++i) {
      auto [x2, var2Val] = m_xVar2[i];
      w_pairxvar.first.push_back(x2);
      w_pairxvar.second.push_back(var2Val);
    }
    return w_pairxvar;
  }

  // double m_yMaxValue;                    /**/
  QVector<pairtimeline> m_vecTimeValues; /*< store data to be visualized at a
                                            given time (x-coord/value)*/

  QStringList getTimesFromTimeValuesVec() const {
    // extract value from full result according to user profile selection
    auto w_vecValueAtime = m_vecTimeValues[0].first;
    assert(0.000 == w_vecValueAtime);

    // Store all times (for each iteration), want to use it in the visualizing
    // the full result (user may want to check or display different profile at
    // different time)
    QStringList w_simTimesList;
    w_simTimesList.reserve(70);
    for (const auto &w_time : m_vecTimeValues) {
      w_simTimesList.push_back(QString::number(w_time.first));
    }
    return w_simTimesList;
  }

  void setDataType(eDataTypeFmt aDataType) { m_dataType = aDataType; }
  eDataTypeFmt getDataType() const { return m_dataType; }

  // need a data store
  eDataTypeFmt m_dataType;
};

// Design Note
//  Could be split into separate struct (aggregate)
/** Base struct to support graph plotting */
struct GraphSettingsPrm { // aggregate
  template <typename... Params> struct dummy1 { std::tuple<Params...> m_data; };
  // variable template
  template <typename... Prms>
  using dummyVec = QVector<GraphSettingsPrm::dummy1<Prms...>>;

  // two-curve format: X|Var1|X|Var2
  dummyVec<double, double, double, double> m_twoCurveData2View;

  // Parameter to manage the graph plotting format/reading
  QString m_graphName;  // ...
  QString m_graphName1; // second grpah
  QString m_xaxisLabel;
  QString m_yaxisLabel;
  eGraphType m_graphType;
  eDataTypes m_dataTypes;
  eVariableProfile m_varProfile;
  std::pair<double, double> m_xAxisRng;
  std::pair<double, double> m_yAxisRng;

  //  QFile m_file1; /*multiple file for multi curve support*/
  //  QFile m_file2; /*multiple file for multi curve support*/
  //  mapValU12 m_mapValUp;           /*map key is iteration number=1,...,N*/
  //  mapValU12 m_mapValU;            /*iteration number=1,...,N*/
  QVector<double> m_qvecX; /*store values as a pair of X/Value*/
                           //  QVector<double> m_ValuesCurve1; /**/
                           //  QVector<double> m_ValuesCurve2; /**/
  double m_yMaxValue;      /**/
  QVector<pairtimeline> m_vecTimeValues; /*< store data to be visualized at a
                                            given time (x-coord/value)*/
  //  vecpair m_xVar1; /* plot variable profile: X/H, X/Q, X/V user select */
  //  vecpair m_xVar2; /* plot variable profile: X/H, X/Q, X/V user select */
  // eFileFormat m_filFmt{
  //    eFileFormat::dbgFormat}; /* file contain the final profile that user
  // want to view * / eGraphFmt m_graphFmt{eGraphFmt::XH}; /* default */
  // eDataType m_dataType{eDataType::EMcNeil}; /* default (temporary, we need to
  //                                           be able to set this field)*/
  QStringList m_simTimesList;

  // inline and a definition since C++17
  static constexpr int iterationStep = 5;

  void setVarProfile(eVariableProfile aVarProfile) {
    m_varProfile = aVarProfile;
  }
  eVariableProfile getVarProfile() const { return m_varProfile; }

  //  void setYaxisLabel(
  //      /*const eGraphFmt aYaxis2Set*/) { // could be a free function
  //    switch (m_graphFmt) {
  //    case DamBreakVisualDbg::eGraphFmt::XH:
  //      m_yaxisLabel = QString{"H(Water Depth)"}; // w_yAxisLabel[0];
  //      break;
  //    case DamBreakVisualDbg::eGraphFmt::XQ:
  //      m_yaxisLabel = QString{"Q(Water Discharge)"}; // w_yAxisLabel[0];
  //      break;
  //    case DamBreakVisualDbg::eGraphFmt::XV:
  //      m_yaxisLabel = QString{"V(Water Velocity)"}; // w_yAxisLabel[0];
  //      break;
  //      //         case TestQTApp::eGraphFmt::XF: to be implemented Froude
  //      //         Number
  //      //           m_yaxisLabel = QString{ "V(Water Velocity)" };
  //      //           //w_yAxisLabel[0]; break;
  //    default:
  //      break;
  //    }
  //  }
};
} // namespace qplot
