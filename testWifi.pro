#-------------------------------------------------
#
# Project created by QtCreator 2016-01-26T19:44:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = testWifi
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../GitHub/wifi_signal/src/SSH_IO.cpp \
    ../GitHub/wifi_signal/src/remote_processes.cpp \
    ../GitHub/wifi_signal/src/monitor_manage.cpp \
    ../GitHub/wifi_signal/src/Define_class.cpp

HEADERS  += mainwindow.h \
    ../GitHub/wifi_signal/include/SSH_IO.h \
    ../GitHub/wifi_signal/include/remote_processes.h \
    ../GitHub/wifi_signal/include/normalLib.h \
    ../GitHub/wifi_signal/include/monitor_manage.h \
    ../GitHub/wifi_signal/include/global_variable.h \
    ../GitHub/wifi_signal/include/Define_class.h

FORMS    += mainwindow.ui

CONFIG += c++11

INCLUDEPATH+=$$PWD/../../../../libssh/lib
DEPENDPATH += $$PWD/../../../../libssh/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../libssh/lib/libssh.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$PWD/../../../../libssh/lib/libssh.dll.a

INCLUDEPATH += $$PWD/../../../../libssh/include
DEPENDPATH += $$PWD/../../../../libssh/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../libssh/lib/libssh.dll.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../libssh/lib/libssh.dll.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../libssh/lib/libssh.dll.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../libssh/lib/libssh.dll.lib
