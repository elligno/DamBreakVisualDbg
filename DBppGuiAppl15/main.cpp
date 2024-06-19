#include "dbppguiappl15.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DBppGuiAppl15 w;
    w.show();
    return a.exec();
}
