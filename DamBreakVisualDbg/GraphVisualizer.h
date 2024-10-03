#pragma once

namespace qplot {

  // forward declaration
  class QCustomPlot;

class GraphVisualizer {
public:
  void visualize();
  void configure();
  void setMultipleGraph();

private:

  // QtCustom plot library
  QCustomPlot* m_customPlot;
};
} // namespace qplot
