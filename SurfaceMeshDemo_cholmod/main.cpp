#include <QtCore/QCoreApplication>
#include <iostream>

/// Stuff for surfacemesh
#include "SurfaceMeshModel.h"
#include "LaplacianHelper.h"
#include "EigenPoissonHelper.h"

int main(int argc, char* argv[]){   
    QCoreApplication a(argc, argv);
    
    if(argc<2){
        qDebug() << "Wrong parameters!!! Provided: " << argc;
        qDebug() << "Usage:\n  smoothme in.off [out.off]";
        exit(0);
    } 
    
    /// Read mesh model
    SurfaceMeshModel model;
    bool status = model.read(argv[1]);
    if(status==false){
        qDebug() << "cannot read model";
        exit(0);
    }
    
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
    CotangentLaplacianHelper clh(&model);
    clh.computeCotangentEdgeWeights("e:weight");
    EigenPoissonHelper tph(&model);
    model.add_vertex_property<SurfaceMeshTypes::Scalar>("v:constraint",.01);
    tph.solve("e:weight","v:constraint",VPOINT,VPOINT);
#endif
    
    /// Write to output
    if(argc==3)
        model.write(argv[2]);
    
    // qDebug() << "Done!";
    return 0;
}
