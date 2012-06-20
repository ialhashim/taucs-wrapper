#include "SurfaceMeshModel.h"
#include "SurfaceMeshQForEachHelpers.h"

SurfaceMeshForEachVertexHelper SurfaceMeshModel::vertices(){
    return SurfaceMeshForEachVertexHelper(this);
}

SurfaceMeshForEachEdgeHelper SurfaceMeshModel::edges(){
    return SurfaceMeshForEachEdgeHelper(this);
}

SurfaceMeshForEachFaceHelper SurfaceMeshModel::faces(){
    return SurfaceMeshForEachFaceHelper(this);
}

SurfaceMeshForEachOneRingEdgesHelper SurfaceMeshModel::onering_hedges(Surface_mesh::Vertex v){
    return SurfaceMeshForEachOneRingEdgesHelper(this,v);
}

QDebug operator<< (QDebug d, const Surface_mesh::Edge& edge) {
    d.nospace() << "Edge[" << edge.idx() << "]";
    return d.space();
}
