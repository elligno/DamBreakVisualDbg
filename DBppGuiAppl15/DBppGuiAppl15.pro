QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    dbppguiappl15.cpp

HEADERS += \
    dbppguiappl15.h

FORMS += \
    dbppguiappl15.ui

INCLUDEPATH += $(BOOST_ROOT)

TRANSLATIONS += \
    DBppGuiAppl15_en_US.ts

LIBS += -L$(BOOST_ROOT)/lib64-msvc-14.2/lib -lboost_filesystem-vc142-mt-gd-x64-1_82

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# original config done by Qt Creator, link error couldn't find library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DamBreakAPI/release/ -lDamBreakAPI
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DamBreakAPI/debug/ -lDamBreakAPI

# added by Jean Belanger (see comment above). I think the problem comes from tool Kits that we select
# in the settings project tab (yeah, projects tab, then setting build and run, can switch between different
# tool kits or configuration e.g. desktop, LLVM clang, ...) create a folder with prefix "build" where
# intermediate files are saved. Config was done initialy with desktop config (not too sure) need to
# investigate a little bit more

INCLUDEPATH += $$PWD/../DamBreakAPI
#DEPENDPATH += $$PWD/../DamBreakAPI

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DamBreakAPI/release/libDamBreakAPI.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DamBreakAPI/debug/libDamBreakAPI.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DamBreakAPI/release/DamBreakAPI.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../DamBreakAPI/debug/DamBreakAPI.lib
