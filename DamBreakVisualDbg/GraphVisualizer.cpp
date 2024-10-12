#include "GraphVisualizer.h"
#include <iostream>
// test includes
#include "GenNumber.hpp"
//#include "VisualDbgUtilities.hpp"
// lib API
#include "Utility/dbpp_AppConstant.hpp"
namespace qplot {

GraphVisualizer::GraphVisualizer() : m_customPlot{nullptr} {
  // does it make sense? not sure
  m_customPlot = new QCustomPlot;

  // 2d plot (plot x-axis and y-axis)
  m_customPlot->setGeometry(
      200, 100, 640, 400); // window size when poping-up with offset in x/y
  m_customPlot->setWindowTitle(QString{"DamBreak Wave Propagation"});

  // first we create and prepare a text layout element:
  QCPTextElement *title = new QCPTextElement(m_customPlot);
  title->setText("HLL Riemann Solver");
  title->setFont(QFont("sans", 10, QFont::Bold));
  // then we add it to the main plot layout:
  m_customPlot->plotLayout()->insertRow(
      0); // insert an empty row above the axis rect
  m_customPlot->plotLayout()->addElement(
      0, 0, title); // place the title in the empty cell we've just created
}

GraphVisualizer::~GraphVisualizer() {
  if (m_customPlot) {
    delete m_customPlot;
    m_customPlot = nullptr;
  }
}

void GraphVisualizer::setMultipleGraph() {
  // auto w_graphId = 0;
#if 0
  if (nullptr != m_customPlot) {
    delete m_customPlot;
    m_customPlot = nullptr;
  }

   does it make sense? not sure
  m_customPlot = new QCustomPlot;

  // 2d plot (plot x-axis and y-axis)
  m_customPlot->setGeometry(
      200, 100, 640, 400); // window size when poping-up with offset in x/y
  m_customPlot->setWindowTitle(QString{"DamBreak Wave Propagation"});

  // first we create and prepare a text layout element:
  QCPTextElement *title = new QCPTextElement(m_customPlot);
  title->setText(QString{"HLL Riemann Solver"});
  title->setFont(QFont("sans", 9, QFont::Bold));
  // then we add it to the main plot layout:
  m_customPlot->plotLayout()->insertRow(
      0); // insert an empty row above the axis rect
  m_customPlot->plotLayout()->addElement(
      0, 0, title); // place the title in the empty cell we've just created

  m_customPlot->xAxis->setLabel(QString{"X Coordinate"});

  if (eVariableProfile::XH == m_graph2D.m_graphFmt) // X/H profile
  {
    m_customPlot->yAxis->setLabel(QString{"H (Water Depth)"});
  } else {
    m_customPlot->yAxis->setLabel(QString{"Q (Discharge)"});
  }

  // loop on all supported graph
  auto jj = 0;
  for (auto i = 0; i < nbGraph; ++i) {
    if (m_activate[i]->isChecked()) {
      m_customPlot->addGraph();
      auto w_graphLineType = m_lineType[i]->currentText();

      if (w_graphLineType == QString{"Dash Line"}) {
        m_customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsLine);
      } else if (w_graphLineType == QString{"Dot Line"}) {
        m_customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsNone);
      } else {
        m_customPlot->graph(i)->setLineStyle(QCPGraph::lsLine);
      }

      // data may belong to different file such as: exact/computed
      // we have to curve but data extract form files.
      //   m_graph2D.m_xVar

      //   m_customPlot->graph(i)->setData(m_graph2D.m_qvecX, // increment
      //   graph no by 1 for legend display and next graph
      //     QVector<double>::fromStdVector(m_graph2D));
#if 0
      // ======================== DEBUG FORMAT
      if (eFileFormat::dbgFormat == m_graph2D.m_filFmt) {
        // m_customPlot->addGraph();
        m_customPlot->graph(i)->setPen(QColor(m_colorCurve[i]->currentText()));
        // m_customPlot->graph(i)->setLineStyle(QCPGraph::lsLine); lsline(each
        // point attached by line), lsnone (no point attached similar to dot)
        m_customPlot->graph(i)->setName(QString("Time_") +
                                        QString::number(jj++));
        if (eGraphFmt::XH == m_graph2D.m_graphFmt) // X/H profile
        {
          m_customPlot->graph(i)->setData(
              m_graph2D.m_qvecX, // increment graph no by 1 for legend display
                                 // and next graph
              QVector<double>::fromStdVector(
                  m_graph2D.m_mapValU[m_iterNo[i]->value()].first));

          // just testing (preprocessing stuff) under construction
          auto w_ymaxValue = std::max_element(
              m_graph2D.m_mapValU[m_iterNo[0]->value()].first.begin(),
              m_graph2D.m_mapValU[m_iterNo[0]->value()].first.end());

          setPlotParams(QString{"H(Water Depth)"},
                        *w_ymaxValue + 2.); // be careful with hard coding 2.?
        } else                              // X/Q profile
        {
          m_customPlot->graph(i)->setData(
              m_graph2D.m_qvecX, // increment graph no by 1 for legend display
                                 // and next graph
              QVector<double>::fromStdVector(
                  m_graph2D.m_mapValU[m_iterNo[i]->value()].second));

          // just testing (preprocessing stuff) under construction
          auto w_ymaxValue = std::max_element(
              m_graph2D.m_mapValU[m_iterNo[0]->value()].second.begin(),
              m_graph2D.m_mapValU[m_iterNo[0]->value()].second.end());

          setPlotParams(QString{"Q(Water Discharge)"},
                        *w_ymaxValue + 2.); // be careful with hard coding 2.?
        }
      } // dbgFormat
#endif
      else if (eFileFormat::exact_computed == m_graph2D.m_filFmt) {
        m_yAxisRng = {0.45, 1.1}; // temporary fix
        m_xAxisRng = {0., 1.};    // temporary fix

        // default graph format is X/H (it's the only one )
        if (i == 0) // temporary fix
        {
          QPen w_pen0;
          w_pen0.setStyle(Qt::DotLine);
          w_pen0.setWidth(1);
          w_pen0.setColor(QColor(180, 180, 180));
          m_customPlot->graph(0)->setPen(w_pen0);
          m_customPlot->graph(0)->setBrush(QBrush(QColor(255, 50, 30, 20)));
          // let the ranges scale themselves so graph 0 fits perfectly in the
          // visible area:
          m_customPlot->graph(0)->rescaleAxes();
          // QColor(m_colorCurve[i]->currentText()));
          //  m_customPlot->graph(i)->setLineStyle(
          //      QCPGraph::lsLine); // lsline(each point attached by line),
          // lsnone (no point attached similar to dot)
          // legend name (first curve)

          m_customPlot->graph(i)->setName(QString{"Exact Solution"});
          setPlotParams(
              QString{"H(Water Depth)"},
              m_graph2D.m_yMaxValue); // be careful with hard coding 2.?

          // ... to be completed
          m_customPlot->graph(i)->setData(m_graph2D.m_qvecX,
                                          m_graph2D.m_ValuesCurve1);
          continue;
        } else // second graph
        {
          // add theory curve graph:
          QPen w_pen1;
          w_pen1.setStyle(Qt::DashLine);
          w_pen1.setWidth(2);
          w_pen1.setColor(Qt::blue);
          m_customPlot->graph(i)->setPen(w_pen1);
          // same thing for graph 1, but only enlarge ranges (in case graph 1 is
          // smaller than graph 0):
          m_customPlot->graph(1)->rescaleAxes(true);

          //          m_customPlot->graph(i)->setPen(
          //              QColor(m_colorCurve[i]->currentText()));
          //          m_customPlot->graph(i)->setLineStyle(
          //              QCPGraph::lsImpulse); // lsline(each point attached by
          //              line),
          // lsnone (no point attached similar to
          // dot)

          // legend name (second curve)
          m_customPlot->graph(i)->setName(QString("Computed Solution"));

          setPlotParams(
              QString{"H(Water Depth)"},
              m_graph2D.m_yMaxValue); // be careful with hard coding 2.?
          m_customPlot->graph(i)->setData(m_graph2D.m_qvecX,
                                          m_graph2D.m_ValuesCurve2);
        }
      }           // exact/computed
      else {      // not sure about this one, really need it?
        continue; // what???
      }
    } // for-loop
#endif

#if 0 // not really need it (done in m_lineType with setGeometry(...))
    // ...
      QPoint posWgtGlobalPos = m_customPlot->mapToGlobal(QPoint(0, 0));

      // Split screen into quadrants
      QDesktopWidget* desktop = QApplication::desktop();
      QRect screenRect = desktop->availableGeometry(posWgtGlobalPos);

      // desktop coordinate
      int screenMidX = screenRect.x() + (screenRect.width() / 2);
      int screenMidY = screenRect.y() + (screenRect.height() / 2);

      // Position the popup widget based on the quadrant posWgt is in
      int popX = posWgtGlobalPos.x();
      int popY = posWgtGlobalPos.y();

      popX += screenMidX / 2; //  m_customPlot->width();
      popY += screenMidY / 2; // m_customPlot->height();

      QPoint popPnt = m_customPlot->mapFromGlobal(QPoint(popX, popY));
      m_customPlot->setGeometry(popX, popY - 50,
        width() / 2,
        height() / 2);
#endif
  // plot selected graph
  m_customPlot->replot();

  // pop-up window (separate widget)
  m_customPlot->show();
  //}
}

void GraphVisualizer::AddGraph(const eGraphType aGrType) {
  m_customPlot->addGraph();

  // NOTE shouldn't be done in this function
  if (aGrType == eGraphType::compare) {
    // exact/computed
    // default graph format is X/H (it's the only one )
    if (1 == m_customPlot->graphCount()) {
      QPen w_pen0;
      w_pen0.setStyle(Qt::DotLine);
      w_pen0.setWidth(1);
      w_pen0.setColor(QColor(180, 180, 180));
      m_customPlot->graph(0)->setPen(w_pen0);
      m_customPlot->graph(0)->setBrush(QBrush(QColor(255, 50, 30, 20)));
    } else { // second graph
      // add theory curve graph:
      QPen w_pen1;
      w_pen1.setStyle(Qt::DashLine);
      w_pen1.setWidth(2);
      w_pen1.setColor(Qt::blue);
      m_customPlot->graph(1)->setPen(w_pen1);
    }
  } else { // multiple graph
           // auto w_graphLineType = m_lineType[i]->currentText();
    if (1 == m_customPlot->graphCount()) {
      m_customPlot->graph(0)->setLineStyle(QCPGraph::LineStyle::lsLine);
      QPen w_pen0;
      w_pen0.setStyle(Qt::SolidLine);
      w_pen0.setWidth(1);
      w_pen0.setColor("blue");
      m_customPlot->graph(0)->setPen(w_pen0);
    } else if (2 == m_customPlot->graphCount()) {
      m_customPlot->graph(1)->setLineStyle(QCPGraph::LineStyle::lsLine);
      QPen w_pen1;
      w_pen1.setStyle(Qt::SolidLine);
      w_pen1.setWidth(1);
      w_pen1.setColor("green");
      m_customPlot->graph(1)->setPen(w_pen1);
    } else if (3 == m_customPlot->graphCount()) {
      m_customPlot->graph(2)->setLineStyle(QCPGraph::LineStyle::lsLine);
      QPen w_pen2;
      w_pen2.setStyle(Qt::SolidLine);
      w_pen2.setWidth(1);
      w_pen2.setColor("red");
      m_customPlot->graph(2)->setPen(w_pen2);
    } else if (4 == m_customPlot->graphCount()) {
      m_customPlot->graph(3)->setLineStyle(QCPGraph::LineStyle::lsLine);
      QPen w_pen3;
      w_pen3.setStyle(Qt::SolidLine);
      w_pen3.setWidth(1);
      w_pen3.setColor("magenta");
      m_customPlot->graph(2)->setPen(w_pen3);
    }
  }
}

// just testing
// return nb graph in the plot
//  auto nbGraph = m_customPlot->graphCount();
//  assert(1 == nbGraph);
//  // last graph inserted
//  auto lastGraph = m_customPlot->graph();
//  auto graph0 = m_customPlot->graph(0);
//  assert(lastGraph == graph0);

// set ranges:
//  m_customPlot->xAxis->setRange(0, 2.5);
//  m_customPlot->yAxis->setRange(0.9, 1.6);

// m_customPlot->graph(0)->setBrush(QBrush(QColor(255, 50, 30, 20)));
// let the ranges scale themselves so graph 0 fits perfectly in the
// visible area:
// m_customPlot->graph(0)->rescaleAxes();
// assign top/right axes same properties as bottom/left:
// m_customPlot->axisRect()->setupFullAxesBox();

// temporary fix
// int i = 0;  graph no (nbGraph=5)
//  QString w_graphLineType;
//  if (w_graphLineType == QString{"Dash Line"}) {
//    m_customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsLine);
//  } else if (w_graphLineType == QString{"Dot Line"}) {
//    m_customPlot->graph(i)->setLineStyle(QCPGraph::LineStyle::lsNone);
//  } else {
//    m_customPlot->graph(i)->setLineStyle(QCPGraph::lsLine);
//  }
//}

void GraphVisualizer::setDbgData(const QVector<double> &aqVecx,
                                 const std::vector<double> &aMapVal) {
  auto w_graphNo = m_customPlot->graphCount();

  // last graph inserted
  m_customPlot->graph()->setName(QString("Time_") +
                                 QString::number(w_graphNo++));
  m_customPlot->graph()->setData(aqVecx,
                                 QVector<double>::fromStdVector(aMapVal));

#if 0
  if (eVariableProfile::XH == m_graphSettings.m_varProfile) // X/H profile
  {
    // increment graph no by 1 for legend
    m_customPlot->graph()->setData(m_graphSettings.m_qvecX,
                                   QVector<double>::fromStdVector(aMapVal));

    // m_graph2D.m_mapValU[m_iterNo[i]->value()].first));

    // just testing (preprocessing stuff) under construction
    //      auto w_ymaxValue = std::max_element(
    //          m_graph2D.m_mapValU[m_iterNo[0]->value()].first.begin(),
    //          m_graph2D.m_mapValU[m_iterNo[0]->value()].first.end());

    // setPlotParams( QString{"H(Water Depth)"},
    //*w_ymaxValue + 2.); be careful with hard coding 2.?
  } else // X/Q profile
  {
    // and next graph
    m_customPlot->graph()->setData(
        m_graphSettings.m_qvecX, // increment graph no by 1 for legend display

        QVector<double>::fromStdVector(aMapVal));
    // m_graph2D.m_mapValU[m_iterNo[i]->value()].second));
#endif
  // just testing (preprocessing stuff) under construction
  //    auto w_ymaxValue = std::max_element(
  //        m_graph2D.m_mapValU[m_iterNo[0]->value()].second.begin(),
  //        m_graph2D.m_mapValU[m_iterNo[0]->value()].second.end());

  //    setPlotParams(QString{"Q(Water Discharge)"},
  //                  *w_ymaxValue + 2.); // be careful with hard coding 2.?
  //}
}

// compare exact/computed (2-profile)
void GraphVisualizer::setCmpData(const pairxVar &aVecXH) {
  // m_yAxisRng = {0.45, 1.1};  temporary fix
  // m_xAxisRng = {0., 1.};     temporary fix

  // default graph format is X/H (it's the only one )
  if (1 == m_customPlot->graphCount()) {
    //    QPen w_pen0;
    //    w_pen0.setStyle(Qt::DotLine);
    //    w_pen0.setWidth(1);
    //    w_pen0.setColor(QColor(180, 180, 180));
    //    m_customPlot->graph(0)->setPen(w_pen0);
    //    m_customPlot->graph(0)->setBrush(QBrush(QColor(255, 50, 30, 20)));
    // let the ranges scale themselves so graph 0 fits perfectly in the
    // visible area:
    m_customPlot->graph(0)->rescaleAxes();

    // QColor(m_colorCurve[i]->currentText()));
    //  m_customPlot->graph(i)->setLineStyle(
    //      QCPGraph::lsLine); // lsline(each point attached by line),
    // lsnone (no point attached similar to dot)
    // legend name (first curve)

    // m_customPlot->graph(0)->setName(QString{"Exact Solution"});
    m_customPlot->graph(0)->setName(QString{"Exact Solution"});

    // setPlotParams(aGraphSet);

    // ... to be completed
    m_customPlot->graph(0)->setData(aVecXH.first, aVecXH.second);
  } else { // second graph
    // add theory curve graph:
    //    QPen w_pen1;
    //    w_pen1.setStyle(Qt::DashLine);
    //    w_pen1.setWidth(2);
    //    w_pen1.setColor(Qt::blue);
    //    m_customPlot->graph(1)->setPen(w_pen1);
    // same thing for graph 1, but only enlarge ranges (in case graph 1 is
    // smaller than graph 0):
    m_customPlot->graph(1)->rescaleAxes(true);

    //          m_customPlot->graph(i)->setPen(
    //              QColor(m_colorCurve[i]->currentText()));
    //          m_customPlot->graph(i)->setLineStyle(
    //              QCPGraph::lsImpulse); // lsline(each point attached by
    //              line),
    // lsnone (no point attached similar to
    // dot)

    // legend name (second curve)
    m_customPlot->graph(1)->setName(QString("Computed Solution"));

    //    setPlotParams(QString{"H(Water Depth)"},
    //                  m_graph2D.m_yMaxValue); // be careful with hard
    //                  coding 2.?

    m_customPlot->graph(1)->setData(aVecXH.first, aVecXH.second);
  }
}

void GraphVisualizer::setPlotParams(GraphSettingsPrm aGraphSettings) {

  try {
    // 2d plot (plot x-axis and y-axis)
    //     m_customPlot = new QCustomPlot;
#if 0
    m_customPlot->setGeometry(
        200, 100, 640, 400); // window size when poping-up with offset in x/y
    m_customPlot->setWindowTitle(QString{"DamBreak Wave Propagation"});

    // first we create and prepare a text layout element:
    QCPTextElement *title = new QCPTextElement(m_customPlot);
    title->setText("HLL Riemann Solver");
    title->setFont(QFont("sans", 12, QFont::Bold));
    // then we add it to the main plot layout:
    m_customPlot->plotLayout()->insertRow(
        0); // insert an empty row above the axis rect
    m_customPlot->plotLayout()->addElement(
        0, 0, title); // place the title in the empty cell we've just created

    m_customPlot->xAxis->setLabel("X Coordinate");
    m_customPlot->yAxis->setLabel(m_yAxisLabel);

    // first we create and prepare a text layout element:
    QCPTextElement *title = new QCPTextElement(m_customPlot);
    title->setText("HLL Riemann Solver");
    title->setFont(QFont("sans", 10, QFont::Bold));
    // then we add it to the main plot layout:
    m_customPlot->plotLayout()->insertRow(
        0); // insert an empty row above the axis rect
    m_customPlot->plotLayout()->addElement(
        0, 0, title); // place the title in the empty cell we've just created
#endif

    m_customPlot->xAxis->setLabel(aGraphSettings.m_xaxisLabel);
    m_customPlot->yAxis->setLabel(aGraphSettings.m_yaxisLabel);

    // set axes ranges, so we see all data:
    m_customPlot->xAxis->setRange(
        aGraphSettings.m_xAxisRng.first /*min*/,
        aGraphSettings.m_xAxisRng.second /*max*/); // x-coordinate

    m_customPlot->yAxis->setRange(aGraphSettings.m_yAxisRng.first /*min*/,
                                  aGraphSettings.m_yAxisRng.second /*max*/); //

    // let the ranges scale themselves so graph 0 fits perfectly in the
    // visible area:
    //    m_customPlot->graph(0)->rescaleAxes();
    //    m_customPlot->graph(0)->rescaleAxes(true);

    // assign top/right axes same properties as bottom/left:
    m_customPlot->axisRect()->setupFullAxesBox();

    // Try to add a legend to the graph
    m_customPlot->setLocale(
        QLocale(QLocale::English,
                QLocale::UnitedStates)); // period as decimal separator and
                                         // comma as thousand separator
    m_customPlot->legend->setVisible(true);

    // Returns the default application font.
    QFont legendFont = QGuiApplication::font();
    legendFont.setPointSize(9);
    m_customPlot->legend->setFont(legendFont);
    //	 m_customPlot->legend->setSelectedFont(legendFont);

    //	 QFont legendFont = font();  // start out with MainWindow's
    // font.. 	 legendFont.setPointSize(9); // and make a bit smaller
    // for legend 	 customPlot->legend->setFont(legendFont);
    m_customPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    // by default, the legend is in the inset layout of the main axis rect. So
    // this is how we access it to change legend placement:
    m_customPlot->axisRect()->insetLayout()->setInsetAlignment(
        0, Qt::AlignTop | Qt::AlignRight);

    //		 m_customPlot->legend->addElement(0, 0, ...);
    // ui->customPlot->legend->setSelectableParts(QCPLegend::spItems);  legend
    // box

    // make top right axes clones of bottom left axes
    m_customPlot->axisRect()->setupFullAxesBox();
  } catch (std::bad_alloc &ba) {
    std::cerr << "Caught exception in setPlotParams " << ba.what() << "\n";
  }
}

QVector<double> GraphVisualizer::generateXcoord(eDataTypes aDatatype) {
  std::vector<double> w_X; // shall we use reserve? yes i do think so
  w_X.reserve(dbpp::EMCNEILNbSections::value);
  // E. McNeil data
  if (aDatatype == eDataTypes::EMcNeilDataType) {
    w_X.reserve(dbpp::EMCNEILNbSections::value); // since we are using
                                                 // back_inserter (push_back)
    const auto w_dx = 10.;
    const auto w_startX = -10.;
    // const unsigned w_nbPts = EMCNEILCTE::value;
    std::generate_n(std::back_inserter(w_X),              // start
                    dbpp::EMCNEILNbSections::value,       // number of elements
                    NumSequence<double>(w_startX, w_dx)); // generate values

    // need QVector to be use in QtCustomPlot, let's create one with the
    // helper (return a temporary, since C++17 copy elison)
    // unmaterialize to xvalue (eXpiring value)
    // m_graph2D.m_qvecX = QVector<double>::fromStdVector(w_X);
    // assert(m_graph2D.m_qvecX.size() == EMCNEILCTE::value);
  } else // only 2 supported for now
  {
    // shall we use reserve? yes since we are using back_inserter (push_back)
    w_X.reserve(dbpp::EMCNEILNbSections::value);
    const auto w_dx = 0.01;
    const auto w_startX = -0.01;
    // const unsigned w_nbPts = EMCNEILCTE::value;
    std::generate_n(std::back_inserter(w_X),              // start
                    dbpp::EMCNEILNbSections::value,       // number of elements
                    NumSequence<double>(w_startX, w_dx)); // generate values

    // need QVector to be use in QtCustomPlot, let's create one with the
    // helper (return a temporary, since C++17 copy elison)
    // unmaterialize to xvalue (eXpiring value)
    // m_graph2D.m_qvecX = QVector<double>::fromStdVector(w_X);
    // assert(m_graph2D.m_qvecX.size() == EMCNEILCTE::value);
  }

  return QVector<double>::fromStdVector(w_X);
}

void GraphVisualizer::plot2DProfile() {
  // plot selected graph
  m_customPlot->replot();

  // pop-up window (separate widget)
  m_customPlot->show();
}
} // namespace qplot
