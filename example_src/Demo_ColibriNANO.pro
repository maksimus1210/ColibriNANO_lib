TEMPLATE = app              
TARGET 	 = Demo_ColibriNANO

#############################################################
QT += core gui widgets
#############################################################

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(release, debug|release): DESTDIR = bin/win32     # release
CONFIG(debug  , debug|release): DESTDIR = bin/win32_d   # debug

#CONFIG += console
#CONFIG += warn_on

MOC_DIR     = tmp
OBJECTS_DIR = tmp
RCC_DIR     = tmp
UI_DIR      = tmp

CONFIG += c++14

#############################################################
HEADERS += source/dsp/fft.h
HEADERS += source/dsp/spectrumringbuffer.h
HEADERS += source/dsp/window.h

#############################################################
SOURCES += source/main.cpp

#############################################################
HEADERS += source/gui/MainWindow.h
SOURCES += source/gui/MainWindow.cpp
FORMS   += source/gui/MainWindow.ui

HEADERS += source/LibLoader/common.h
HEADERS += source/LibLoader/LibLoader.h
SOURCES += source/LibLoader/LibLoader.cpp

HEADERS += source/dsp/DspCore.h
SOURCES += source/dsp/DspCore.cpp

#############################################################
include(libs/QCustomPlot/QCustomPlot.pri)
