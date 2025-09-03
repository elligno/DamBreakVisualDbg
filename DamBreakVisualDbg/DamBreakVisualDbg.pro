QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataLoader.cpp \
    GraphVisualizer.cpp \
    TestReadRes.cpp \
    main.cpp \
    dambreakvisualdbg.cpp

HEADERS += \
    DataLoader.h \
    Enumerators.h \
    FunctionLambda.hpp \
    GenNumber.hpp \
    GraphSettingsPrm.h \
    GraphVisualizer.h \
    VisualDbgAliases.hpp \
    VisualDbgUtilities.hpp \
    dambreakvisualdbg.h

FORMS += \
    dambreakvisualdbg.ui

INCLUDEPATH += $$PWD/../../../QCustomPlot
INCLUDEPATH += $(BOOST_ROOT)
INCLUDEPATH += $$PWD/../DamBreakAPI
#LIBS += -L$$PWD/../bin/ -lqcustomplotd2 original

# original config done by Qt Creator, link error couldn't find library
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/ -lqcustomplot2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/ -lqcustomplotd2

#LIBS += -L$(BOOST_ROOT)/lib64-msvc-14.2/lib -lboost_filesystem-vc142-mt-gd-x64-1_82

#TRANSLATIONS += DamBreakVisualDbg_en_US.ts
#CONFIG += lrelease
#CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
