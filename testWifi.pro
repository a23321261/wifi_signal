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
    Define_class.cpp \
    monitor_manage.cpp \
    remote_processes.cpp \
    SSH_IO.cpp \
    mainprocess.cpp \
    dbm_to_meter.cpp

HEADERS  += mainwindow.h \
    Define_class.h \
    global_variable.h \
    monitor_manage.h \
    normalLib.h \
    remote_processes.h \
    SSH_IO.h \
    mainprocess.h \
    traceing.h \
    dbm_to_meter.h

FORMS    += mainwindow.ui

CONFIG += c++11

INCLUDEPATH+=$$PWD/../libssh/lib
DEPENDPATH += $$PWD/../libssh/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libssh/lib/libssh.dll
else:win32:CONFIG(debug, debug|release): LIBS += $$PWD/../libssh/lib/libssh.dll.a

INCLUDEPATH += $$PWD/../libssh/include
DEPENDPATH += $$PWD/../libssh/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libssh/lib/libssh.dll.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libssh/lib/libssh.dll.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../libssh/lib/libssh.dll.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../libssh/lib/libssh.dll.lib
