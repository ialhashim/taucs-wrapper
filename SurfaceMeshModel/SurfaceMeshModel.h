#pragma once

#include <QDebug>
#include <QString>
#include "Surface_mesh.h"

/// @{ Forward declaration of helpers defined in helper/...
///    This reduces clutter of this class header
class SurfaceMeshForEachVertexHelper;
class SurfaceMeshForEachEdgeHelper;
class SurfaceMeshForEachOneRingEdgesHelper;
class SurfaceMeshForEachFaceHelper;
/// @}

class SurfaceMeshModel : public Surface_mesh{
    // Q_OBJECT
    
    /// @{ Qt foreach helpers
    ///    Example: foreach(Vertex v, m->validVertices()){ ... }
    public:
        using Surface_mesh::vertices; /// F-you C++
        SurfaceMeshForEachVertexHelper vertices(); 
        SurfaceMeshForEachEdgeHelper edges();
        using Surface_mesh::faces; /// F-you C++
        SurfaceMeshForEachFaceHelper faces();
        SurfaceMeshForEachOneRingEdgesHelper onering_hedges(Vertex v);
    /// @}
};
