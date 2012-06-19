TARGET = taucs_demo
TEMPLATE = app
QT -= core gui
CONFIG += console
CONFIG -= app_bundle

# Import framework
QMAKE_CXXFLAGS += -F$$(HOME)/Library/Frameworks
QMAKE_LFLAGS += -F$$(HOME)/Library/Frameworks
LIBS += -framework Taucs

SOURCES += main.cpp
