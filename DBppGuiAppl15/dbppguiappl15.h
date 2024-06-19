#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class DBppGuiAppl15; }
QT_END_NAMESPACE

class DBppGuiAppl15 : public QMainWindow
{
    Q_OBJECT

public:
    DBppGuiAppl15(QWidget *parent = nullptr);
    ~DBppGuiAppl15();

private:
    Ui::DBppGuiAppl15 *ui;
};
