/********************************************************************************
** Form generated from reading UI file 'dambreakvisualdbg.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DAMBREAKVISUALDBG_H
#define UI_DAMBREAKVISUALDBG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DamBreakVisualDbg
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *DamBreakVisualDbg)
    {
        if (DamBreakVisualDbg->objectName().isEmpty())
            DamBreakVisualDbg->setObjectName(QString::fromUtf8("DamBreakVisualDbg"));
        DamBreakVisualDbg->resize(800, 600);
        centralwidget = new QWidget(DamBreakVisualDbg);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        DamBreakVisualDbg->setCentralWidget(centralwidget);
        menubar = new QMenuBar(DamBreakVisualDbg);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        DamBreakVisualDbg->setMenuBar(menubar);
        statusbar = new QStatusBar(DamBreakVisualDbg);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        DamBreakVisualDbg->setStatusBar(statusbar);

        retranslateUi(DamBreakVisualDbg);

        QMetaObject::connectSlotsByName(DamBreakVisualDbg);
    } // setupUi

    void retranslateUi(QMainWindow *DamBreakVisualDbg)
    {
        DamBreakVisualDbg->setWindowTitle(QApplication::translate("DamBreakVisualDbg", "DamBreakVisualDbg", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DamBreakVisualDbg: public Ui_DamBreakVisualDbg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAMBREAKVISUALDBG_H
