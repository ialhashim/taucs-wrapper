# Where to find the TAUCS crap?
INCLUDEPATH += $$PWD/external/include
DEPENDPATH += $$PWD/external/include

#----------------------------------------------
# HEADERS EXPOSED BY THE LIBRARY
#----------------------------------------------
EXPOSED_HEADERS = TaucsMatrix.h TaucsSolver.h

#----------------------------------------------
# RESOURCES
#----------------------------------------------
SOURCES += \
    TaucsMatrix.cpp \
    TaucsSolver.cpp \
    taucs_addon.cpp 
HEADERS += \
    TaucsSolver.h \
    TaucsMatrix.h \
    taucs_addon.h \  
    TaucsException.h

#----------------------------------------------
# OPTIMIZATION
#----------------------------------------------
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

#----------------------------------------------
# DEPENDENT LIBRARIES
#----------------------------------------------
mac{
    LIBS += -framework Accelerate 
    LIBS += -L$$PWD/external/lib/darwin11
    LIBS += -lmetis -ltaucs
}

#----------------------------------------------
# TARGET SPECIFICATION
#----------------------------------------------
mac{
    TEMPLATE = lib
    CONFIG += lib_bundle
    QT -= core gui
    TARGET = Taucs
    VERSION = 1.0

    # Where to deploy the library?
    DESTDIR = ~/Library/Frameworks
    
    # What headers end up in library
    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = $$EXPOSED_HEADERS
    FRAMEWORK_HEADERS.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS

    # Avoid weird ass stuff
    QMAKE_CXXFLAGS += -Wno-write-strings
}

#----------------------------------------------
# INSTALL STEP DEFINITION (make install)
# REQUIRES SUDO PRIVILEGES...
#----------------------------------------------
mac{
    framework.files = ~/Library/Frameworks/Taucs.framework
    framework.path = /Library/Frameworks
    INSTALLS += framework
}
