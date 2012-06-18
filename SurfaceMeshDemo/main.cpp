#include <QtCore/QCoreApplication>
#include <iostream>

/// Stuff for wrappers
#include <Taucs/TaucsMatrix.h>
#include <Taucs/TaucsSolver.h>

/// Stuff for surfacemesh
#include "SurfaceMeshModel.h"
#include "LaplacianHelper.h"

int main(int argc, char* argv[]){   
    QCoreApplication a(argc, argv);
    char *input, *output;
    
    if(argc!=3){
        qDebug() << "Wrong parameters!!! Provided: " << argc;
        qDebug() << "Usage:\n  smoothme in.off out.off";
    } else {
        input = argv[1];
        output = argv[2];        
    }
    
    SurfaceMeshModel model;
    model.read(input);
       
    
    
    return 0;
}
