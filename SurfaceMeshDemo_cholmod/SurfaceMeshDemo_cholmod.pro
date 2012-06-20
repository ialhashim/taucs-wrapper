TARGET = smoothme
TEMPLATE = app
QT += core 
QT -= gui
CONFIG += console
CONFIG -= app_bundle

# DEPLOY IN SRC DIR
DESTDIR = $$PWD

# USE Eigen
load($$PWD/eigen-3.1.0-rc1/eigen-3.1.0-rc1.prf)

# USE Cholmod
INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/..

#---------- CHOLMOD ----------
INCLUDEPATH += $$PWD/cholmod/include
win32{
    # static library (Windows)
    LIBS += -L$$PWD/cholmod/win32/lib 
    LIBS += -llibcholmod -llibgoto_CHOLMOD -llibmetis_CHOLMOD -llibamd -llibcamd -llibcolamd -llibccolamd
}
mac{
    LIBS += -L$$PWD/cholmod/lib/darwin11
    # LIBS += -lcholmod -lamd -lcamd -lcolamd -lccolamd
    LIBS += -lamd -lcamd -lccolamd -lcholmod -lcolamd -lcxsparse -lblas
}

# USE SurfaceMeshModel
INCLUDEPATH += $$PWD/../SurfaceMeshModel
DEPENDPATH += $$PWD/../SurfaceMeshModel
SOURCES += main.cpp \
    ../SurfaceMeshModel/Surface_mesh.cpp \
    ../SurfaceMeshModel/IO.cpp \
    ../SurfaceMeshModel/IO_off.cpp \
    ../SurfaceMeshModel/SurfaceMeshModel.cpp

HEADERS += \
    ../SurfaceMeshModel/Surface_mesh.h \
    ../SurfaceMeshModel/properties.h \
    ../SurfaceMeshModel/LaplacianHelper.h \
    ../SurfaceMeshModel/Vector.h \
    EigenPoissonHelper.h


