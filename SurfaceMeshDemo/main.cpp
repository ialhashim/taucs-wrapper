#include <QtCore/QCoreApplication>
#include <iostream>

/// Stuff for wrappers
#include <Taucs/TaucsMatrix.h>
#include <Taucs/TaucsSolver.h>

/// Stuff for surfacemesh
#include "SurfaceMeshModel.h"
#include "LaplacianHelper.h"
#include "TaucsPoissonHelper.h"

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
    
    /// Read mesh model
    SurfaceMeshModel model;
    model.read(input);
    
    
#if 0
    /// Compute cotangent laplacian weights
    CotangentLaplacianHelper clh(&model);
    for(int i=0; i<10; i++){
        clh.computeCotangentEdgeWeights("e:weight");
        clh.computeLaplacianVectors("v:laplace",true);
        clh.applyLaplacianIteration(.5);
    }
#else
    /// Compute cotangent laplacian weights
    try{
        CotangentLaplacianHelper clh(&model);
        clh.computeCotangentEdgeWeights("e:weight");
        TaucsPoissonHelper tph(&model);
        model.add_vertex_property<Scalar>("v:constraint",.01);
        tph.setup("e:weight","v:constraint");
        tph.solve(VPOINT,"v:constraint",VPOINT);
    } catch( std::exception& e ){
        cerr << e.what() << endl;
    }

#endif
    
    /// Write to output
    model.write(output);
    
    qDebug() << "Done!";
    return 0;
}
