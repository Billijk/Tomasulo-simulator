#-------------------------------------------------
#
# Project created by QtCreator 2017-05-30T00:13:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tomasulo_simulator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    addopswidget.cpp \
    setopwidget.cpp \
    removeopwidget.cpp \
    simulator/alu.cpp \
    simulator/cdb.cpp \
    simulator/instructions.cpp \
    simulator/kernel.cpp \
    simulator/memory.cpp \
    simulator/registers.cpp \
    simulator/simulator.cpp \
    ncyclewidget.cpp \
    setmemwidget.cpp \
    setregwidget.cpp

HEADERS  += mainwindow.h \
    addopswidget.h \
    setopwidget.h \
    removeopwidget.h \
    simulator/alu.h \
    simulator/cdb.h \
    simulator/instructions.h \
    simulator/kernel.h \
    simulator/memory.h \
    simulator/registers.h \
    simulator/reservation.h \
    simulator/simulator.h \
    ncyclewidget.h \
    setmemwidget.h \
    setregwidget.h

FORMS    += mainwindow.ui \
    addopswidget.ui \
    setopwidget.ui \
    removeopwidget.ui \
    ncyclewidget.ui \
    setmemwidget.ui \
    setregwidget.ui

CONFIG += c++11
