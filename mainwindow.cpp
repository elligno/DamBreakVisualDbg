#include "mainwindow.h"
#include "ui_mainwindow.h"

// Plot library
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent),
  ui(new Ui::MainWindow)
{
    ui->setupUi(this);

 //   QCustomPlot* w_testLink = new QCustomPlot;
 //   if(w_testLink) delete w_testLink;
}

MainWindow::~MainWindow()
{
    delete ui;
}
