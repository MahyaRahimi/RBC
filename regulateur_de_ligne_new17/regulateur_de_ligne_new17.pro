QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    listener.cpp \
    frame_analysis.cpp \
    transmitter.cpp \
    inputbuffer.cpp \
    outputbuffer.cpp \
    listen_canbus.cpp \
    inputbuffer_can.cpp \
    blockoccupation.cpp \
    tcpnet.cpp

HEADERS += \
    listener.h \
    definitions.h \
    frame_analysis.h \
    transmitter.h \
    inputbuffer.h \
    outputbuffer.h \
    listen_canbus.h \
    inputbuffer_can.h \
    blockoccupation.h \
    tcpnet.h
