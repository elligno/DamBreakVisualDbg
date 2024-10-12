#pragma once

// STL include
#include <vector>
// App include
#include "GraphSettingsPrm.h"

#include "qcustomplot.h"

namespace qplot {

// forward declaration
class qcustomplot;

class GraphVisualizer {
public:
  GraphVisualizer();
  ~GraphVisualizer();

  void plot2DProfile();
  void AddGraph(const eGraphType aGrType);
  void setDbgData(const QVector<double> &aqVecx,
                  const std::vector<double> &aMapVal);
  void setCmpData(const pairxVar &aVecXH);
  void configure();
  QVector<double> generateXcoord(eDataTypes aDatatype);
  // ...
  void setPlotParams(GraphSettingsPrm aGraphSettings);
  // auto w_anyPlottable = m_customPlot->plottableCount();
  // if(0!=w_anyPlottable)
  //   m_customPlot->clearGraphs();
  // bool hasAnyPlottable() {
  //    return (0 == m_customPlot->plottableCount()); }
  void clearPlottable() {
    auto grCount = m_customPlot->graphCount();
    if (grCount != 0)
      m_customPlot->clearGraphs();

    auto any = m_customPlot->graphCount();

    assert(0 == any);
  }

  // return instance of the plotter
  QCustomPlot *customPlot() const { return m_customPlot; }

private:
  //
  // int m_currGraph;

  // QtCustom plot library
  QCustomPlot *m_customPlot;
  // data to plot
  // CurvesData2Show m_dat2Show;
  // Graph settings params
  // GraphSettingsPrm m_graphSettings;

  // temp fix to make it work with new types
  // pass a vector of data (A,Q) not sure yet
  void setMultipleGraph(/*const std::vector<double> &aData2Display*/);
};
} // namespace qplot
