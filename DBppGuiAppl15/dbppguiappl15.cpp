#include "dbppguiappl15.h"
#include "ui_dbppguiappl15.h"

DBppGuiAppl15::DBppGuiAppl15(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DBppGuiAppl15)
{
    ui->setupUi(this);
}

DBppGuiAppl15::~DBppGuiAppl15()
{
    delete ui;
}

