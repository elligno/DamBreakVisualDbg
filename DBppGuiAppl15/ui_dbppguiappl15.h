/********************************************************************************
** Form generated from reading UI file 'dbppguiappl15.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBPPGUIAPPL15_H
#define UI_DBPPGUIAPPL15_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DBppGuiAppl15
{
public:
    QAction *action_Exit;
    QAction *action_run;
    QAction *action_doOneStep;
    QAction *actioninitialize;
    QWidget *centralWidget;
    QGroupBox *AlgoSelect;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *reconstr_label;
    QComboBox *reconstr_comboBox;
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *numalgo_label;
    QComboBox *numalgo_combo;
    QWidget *verticalLayoutWidget_9;
    QVBoxLayout *verticalLayout_9;
    QLabel *rhsalgo_label;
    QComboBox *rhsalgo_combo;
    QGroupBox *Simulation;
    QWidget *verticalLayoutWidget_4;
    QVBoxLayout *verticalLayout_4;
    QLabel *nbIter;
    QSpinBox *iterations;
    QWidget *verticalLayoutWidget_5;
    QVBoxLayout *verticalLayout_5;
    QLabel *phi1_label;
    QDoubleSpinBox *_phi1;
    QWidget *verticalLayoutWidget_6;
    QVBoxLayout *verticalLayout_6;
    QLabel *phi0_label;
    QDoubleSpinBox *_phi0;
    QWidget *verticalLayoutWidget_7;
    QVBoxLayout *verticalLayout_7;
    QLabel *shocloc_label;
    QDoubleSpinBox *_shockloc;
    QWidget *verticalLayoutWidget_8;
    QVBoxLayout *verticalLayout_8;
    QLabel *nbgridPts_label;
    QSpinBox *_nbgridpts;
    QWidget *verticalLayoutWidget_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *cflNumlabel;
    QDoubleSpinBox *cflNumSpinBox;
    QTextEdit *_msgText;
    QGroupBox *data_groupBox;
    QWidget *verticalLayoutWidget_11;
    QVBoxLayout *verticalLayout_11;
    QLabel *label;
    QComboBox *discrdata_combo;
    QWidget *verticalLayoutWidget_10;
    QVBoxLayout *verticalLayout_10;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox_2;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menu_Simulation;

    void setupUi(QMainWindow *DBppGuiAppl15)
    {
        if (DBppGuiAppl15->objectName().isEmpty())
            DBppGuiAppl15->setObjectName(QString::fromUtf8("DBppGuiAppl15"));
        DBppGuiAppl15->resize(1382, 755);
        action_Exit = new QAction(DBppGuiAppl15);
        action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
        action_run = new QAction(DBppGuiAppl15);
        action_run->setObjectName(QString::fromUtf8("action_run"));
        action_doOneStep = new QAction(DBppGuiAppl15);
        action_doOneStep->setObjectName(QString::fromUtf8("action_doOneStep"));
        actioninitialize = new QAction(DBppGuiAppl15);
        actioninitialize->setObjectName(QString::fromUtf8("actioninitialize"));
        centralWidget = new QWidget(DBppGuiAppl15);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        AlgoSelect = new QGroupBox(centralWidget);
        AlgoSelect->setObjectName(QString::fromUtf8("AlgoSelect"));
        AlgoSelect->setGeometry(QRect(30, 50, 211, 251));
        QFont font;
        font.setPointSize(8);
        AlgoSelect->setFont(font);
        verticalLayoutWidget = new QWidget(AlgoSelect);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(20, 30, 191, 48));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        reconstr_label = new QLabel(verticalLayoutWidget);
        reconstr_label->setObjectName(QString::fromUtf8("reconstr_label"));

        verticalLayout->addWidget(reconstr_label);

        reconstr_comboBox = new QComboBox(verticalLayoutWidget);
        reconstr_comboBox->setObjectName(QString::fromUtf8("reconstr_comboBox"));
        reconstr_comboBox->setEditable(true);

        verticalLayout->addWidget(reconstr_comboBox);

        verticalLayoutWidget_2 = new QWidget(AlgoSelect);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(20, 100, 191, 48));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        numalgo_label = new QLabel(verticalLayoutWidget_2);
        numalgo_label->setObjectName(QString::fromUtf8("numalgo_label"));

        verticalLayout_2->addWidget(numalgo_label);

        numalgo_combo = new QComboBox(verticalLayoutWidget_2);
        numalgo_combo->setObjectName(QString::fromUtf8("numalgo_combo"));
        numalgo_combo->setEditable(true);

        verticalLayout_2->addWidget(numalgo_combo);

        verticalLayoutWidget_9 = new QWidget(AlgoSelect);
        verticalLayoutWidget_9->setObjectName(QString::fromUtf8("verticalLayoutWidget_9"));
        verticalLayoutWidget_9->setGeometry(QRect(20, 170, 191, 48));
        verticalLayout_9 = new QVBoxLayout(verticalLayoutWidget_9);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        rhsalgo_label = new QLabel(verticalLayoutWidget_9);
        rhsalgo_label->setObjectName(QString::fromUtf8("rhsalgo_label"));

        verticalLayout_9->addWidget(rhsalgo_label);

        rhsalgo_combo = new QComboBox(verticalLayoutWidget_9);
        rhsalgo_combo->setObjectName(QString::fromUtf8("rhsalgo_combo"));

        verticalLayout_9->addWidget(rhsalgo_combo);

        Simulation = new QGroupBox(centralWidget);
        Simulation->setObjectName(QString::fromUtf8("Simulation"));
        Simulation->setGeometry(QRect(270, 50, 291, 251));
        verticalLayoutWidget_4 = new QWidget(Simulation);
        verticalLayoutWidget_4->setObjectName(QString::fromUtf8("verticalLayoutWidget_4"));
        verticalLayoutWidget_4->setGeometry(QRect(20, 100, 121, 45));
        verticalLayout_4 = new QVBoxLayout(verticalLayoutWidget_4);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        nbIter = new QLabel(verticalLayoutWidget_4);
        nbIter->setObjectName(QString::fromUtf8("nbIter"));

        verticalLayout_4->addWidget(nbIter);

        iterations = new QSpinBox(verticalLayoutWidget_4);
        iterations->setObjectName(QString::fromUtf8("iterations"));
        iterations->setMinimum(1);

        verticalLayout_4->addWidget(iterations);

        verticalLayoutWidget_5 = new QWidget(Simulation);
        verticalLayoutWidget_5->setObjectName(QString::fromUtf8("verticalLayoutWidget_5"));
        verticalLayoutWidget_5->setGeometry(QRect(170, 30, 111, 48));
        verticalLayout_5 = new QVBoxLayout(verticalLayoutWidget_5);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        phi1_label = new QLabel(verticalLayoutWidget_5);
        phi1_label->setObjectName(QString::fromUtf8("phi1_label"));

        verticalLayout_5->addWidget(phi1_label);

        _phi1 = new QDoubleSpinBox(verticalLayoutWidget_5);
        _phi1->setObjectName(QString::fromUtf8("_phi1"));
        _phi1->setMinimum(1.000000000000000);
        _phi1->setMaximum(100.000000000000000);
        _phi1->setSingleStep(10.000000000000000);
        _phi1->setValue(10.000000000000000);

        verticalLayout_5->addWidget(_phi1);

        verticalLayoutWidget_6 = new QWidget(Simulation);
        verticalLayoutWidget_6->setObjectName(QString::fromUtf8("verticalLayoutWidget_6"));
        verticalLayoutWidget_6->setGeometry(QRect(170, 100, 111, 48));
        verticalLayout_6 = new QVBoxLayout(verticalLayoutWidget_6);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        phi0_label = new QLabel(verticalLayoutWidget_6);
        phi0_label->setObjectName(QString::fromUtf8("phi0_label"));
        phi0_label->setCursor(QCursor(Qt::SizeHorCursor));

        verticalLayout_6->addWidget(phi0_label);

        _phi0 = new QDoubleSpinBox(verticalLayoutWidget_6);
        _phi0->setObjectName(QString::fromUtf8("_phi0"));
        _phi0->setMinimum(0.500000000000000);
        _phi0->setMaximum(100.000000000000000);

        verticalLayout_6->addWidget(_phi0);

        verticalLayoutWidget_7 = new QWidget(Simulation);
        verticalLayoutWidget_7->setObjectName(QString::fromUtf8("verticalLayoutWidget_7"));
        verticalLayoutWidget_7->setGeometry(QRect(170, 170, 111, 51));
        verticalLayout_7 = new QVBoxLayout(verticalLayoutWidget_7);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        shocloc_label = new QLabel(verticalLayoutWidget_7);
        shocloc_label->setObjectName(QString::fromUtf8("shocloc_label"));

        verticalLayout_7->addWidget(shocloc_label);

        _shockloc = new QDoubleSpinBox(verticalLayoutWidget_7);
        _shockloc->setObjectName(QString::fromUtf8("_shockloc"));
        _shockloc->setMinimum(0.000000000000000);
        _shockloc->setMaximum(500.000000000000000);
        _shockloc->setValue(0.000000000000000);

        verticalLayout_7->addWidget(_shockloc);

        verticalLayoutWidget_8 = new QWidget(Simulation);
        verticalLayoutWidget_8->setObjectName(QString::fromUtf8("verticalLayoutWidget_8"));
        verticalLayoutWidget_8->setGeometry(QRect(20, 170, 121, 45));
        verticalLayout_8 = new QVBoxLayout(verticalLayoutWidget_8);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        nbgridPts_label = new QLabel(verticalLayoutWidget_8);
        nbgridPts_label->setObjectName(QString::fromUtf8("nbgridPts_label"));

        verticalLayout_8->addWidget(nbgridPts_label);

        _nbgridpts = new QSpinBox(verticalLayoutWidget_8);
        _nbgridpts->setObjectName(QString::fromUtf8("_nbgridpts"));
        _nbgridpts->setMinimum(1);
        _nbgridpts->setMaximum(1000);
        _nbgridpts->setValue(100);

        verticalLayout_8->addWidget(_nbgridpts);

        verticalLayoutWidget_3 = new QWidget(Simulation);
        verticalLayoutWidget_3->setObjectName(QString::fromUtf8("verticalLayoutWidget_3"));
        verticalLayoutWidget_3->setGeometry(QRect(20, 30, 121, 51));
        verticalLayout_3 = new QVBoxLayout(verticalLayoutWidget_3);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        cflNumlabel = new QLabel(verticalLayoutWidget_3);
        cflNumlabel->setObjectName(QString::fromUtf8("cflNumlabel"));

        verticalLayout_3->addWidget(cflNumlabel);

        cflNumSpinBox = new QDoubleSpinBox(verticalLayoutWidget_3);
        cflNumSpinBox->setObjectName(QString::fromUtf8("cflNumSpinBox"));
        cflNumSpinBox->setMaximum(1.000000000000000);
        cflNumSpinBox->setSingleStep(0.100000000000000);
        cflNumSpinBox->setValue(0.600000000000000);

        verticalLayout_3->addWidget(cflNumSpinBox);

        _msgText = new QTextEdit(centralWidget);
        _msgText->setObjectName(QString::fromUtf8("_msgText"));
        _msgText->setEnabled(true);
        _msgText->setGeometry(QRect(30, 310, 831, 191));
        data_groupBox = new QGroupBox(centralWidget);
        data_groupBox->setObjectName(QString::fromUtf8("data_groupBox"));
        data_groupBox->setGeometry(QRect(590, 50, 271, 251));
        verticalLayoutWidget_11 = new QWidget(data_groupBox);
        verticalLayoutWidget_11->setObjectName(QString::fromUtf8("verticalLayoutWidget_11"));
        verticalLayoutWidget_11->setGeometry(QRect(10, 30, 191, 48));
        verticalLayout_11 = new QVBoxLayout(verticalLayoutWidget_11);
        verticalLayout_11->setSpacing(6);
        verticalLayout_11->setContentsMargins(11, 11, 11, 11);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget_11);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_11->addWidget(label);

        discrdata_combo = new QComboBox(verticalLayoutWidget_11);
        discrdata_combo->setObjectName(QString::fromUtf8("discrdata_combo"));

        verticalLayout_11->addWidget(discrdata_combo);

        verticalLayoutWidget_10 = new QWidget(data_groupBox);
        verticalLayoutWidget_10->setObjectName(QString::fromUtf8("verticalLayoutWidget_10"));
        verticalLayoutWidget_10->setGeometry(QRect(10, 100, 160, 51));
        verticalLayout_10 = new QVBoxLayout(verticalLayoutWidget_10);
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setContentsMargins(11, 11, 11, 11);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(verticalLayoutWidget_10);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_10->addWidget(label_2);

        doubleSpinBox_2 = new QDoubleSpinBox(verticalLayoutWidget_10);
        doubleSpinBox_2->setObjectName(QString::fromUtf8("doubleSpinBox_2"));

        verticalLayout_10->addWidget(doubleSpinBox_2);

        DBppGuiAppl15->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(DBppGuiAppl15);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        DBppGuiAppl15->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(DBppGuiAppl15);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        DBppGuiAppl15->setStatusBar(statusBar);
        menuBar = new QMenuBar(DBppGuiAppl15);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1382, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menu_Simulation = new QMenu(menuBar);
        menu_Simulation->setObjectName(QString::fromUtf8("menu_Simulation"));
        DBppGuiAppl15->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menu_Simulation->menuAction());
        menuFile->addAction(action_Exit);
        menu_Simulation->addAction(actioninitialize);
        menu_Simulation->addAction(action_run);
        menu_Simulation->addAction(action_doOneStep);

        retranslateUi(DBppGuiAppl15);
        QObject::connect(cflNumSpinBox, SIGNAL(valueChanged(double)), DBppGuiAppl15, SLOT(setCFLValue(double)));
        QObject::connect(_shockloc, SIGNAL(valueChanged(double)), DBppGuiAppl15, SLOT(setShockLoc(double)));
        QObject::connect(_phi1, SIGNAL(valueChanged(double)), DBppGuiAppl15, SLOT(setUpstream(double)));
        QObject::connect(_phi0, SIGNAL(valueChanged(double)), DBppGuiAppl15, SLOT(setDownstream(double)));
        QObject::connect(iterations, SIGNAL(valueChanged(int)), DBppGuiAppl15, SLOT(setNbIterationsMax(int)));
        QObject::connect(numalgo_combo, SIGNAL(currentTextChanged(QString)), DBppGuiAppl15, SLOT(setActiveNumericalMethod(QString)));

        numalgo_combo->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(DBppGuiAppl15);
    } // setupUi

    void retranslateUi(QMainWindow *DBppGuiAppl15)
    {
        DBppGuiAppl15->setWindowTitle(QApplication::translate("DBppGuiAppl15", "DAMBREAK++ Simulator", nullptr));
        action_Exit->setText(QApplication::translate("DBppGuiAppl15", "&Exit", nullptr));
        action_run->setText(QApplication::translate("DBppGuiAppl15", "&run", nullptr));
        action_doOneStep->setText(QApplication::translate("DBppGuiAppl15", "&doOneStep", nullptr));
#ifndef QT_NO_SHORTCUT
        action_doOneStep->setShortcut(QApplication::translate("DBppGuiAppl15", "Shift+S, 1", nullptr));
#endif // QT_NO_SHORTCUT
        actioninitialize->setText(QApplication::translate("DBppGuiAppl15", "&initialize", nullptr));
        AlgoSelect->setTitle(QApplication::translate("DBppGuiAppl15", "Algporithm Selection", nullptr));
        reconstr_label->setText(QApplication::translate("DBppGuiAppl15", "Reconstruction Algorithm", nullptr));
        reconstr_comboBox->setCurrentText(QApplication::translate("DBppGuiAppl15", "MUSCL Slope Limiter", nullptr));
        numalgo_label->setText(QApplication::translate("DBppGuiAppl15", "Numerical Method", nullptr));
        numalgo_combo->setCurrentText(QString());
        rhsalgo_label->setText(QApplication::translate("DBppGuiAppl15", "Physical Algorithm", nullptr));
        rhsalgo_combo->setCurrentText(QString());
        Simulation->setTitle(QApplication::translate("DBppGuiAppl15", "Parameters of run", nullptr));
        nbIter->setText(QApplication::translate("DBppGuiAppl15", "Nb Iterations", nullptr));
        phi1_label->setText(QApplication::translate("DBppGuiAppl15", "Phi1 Value", nullptr));
        phi0_label->setText(QApplication::translate("DBppGuiAppl15", "Phi0 Value", nullptr));
        shocloc_label->setText(QApplication::translate("DBppGuiAppl15", "Shock Location", nullptr));
        nbgridPts_label->setText(QApplication::translate("DBppGuiAppl15", "Nb Grid Points", nullptr));
        cflNumlabel->setText(QApplication::translate("DBppGuiAppl15", "CFL Number", nullptr));
        data_groupBox->setTitle(QApplication::translate("DBppGuiAppl15", "Simulation Data", nullptr));
        label->setText(QApplication::translate("DBppGuiAppl15", "DamBreakData", nullptr));
        label_2->setText(QApplication::translate("DBppGuiAppl15", "Manning Coeff", nullptr));
        menuFile->setTitle(QApplication::translate("DBppGuiAppl15", "File", nullptr));
        menuView->setTitle(QApplication::translate("DBppGuiAppl15", "View", nullptr));
        menu_Simulation->setTitle(QApplication::translate("DBppGuiAppl15", "&Simulation", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DBppGuiAppl15: public Ui_DBppGuiAppl15 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBPPGUIAPPL15_H
