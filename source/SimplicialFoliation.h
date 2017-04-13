#pragma once

#include <vector>
#include <string>
#include "Mesh/mesh.h"
#include "Mesh/boundary.h"

using namespace std;

#define ADD_PROPERTY(T, x) \
private:\
    T m_##x; \
public:\
    T & x() { return m_##x; } \

class CSimplicialFoliation
{
public:
    enum Mark { SOURCE, SINK, PARALLEL, FREE };
    class CSVertex
    {
        ADD_PROPERTY(short, source)
        ADD_PROPERTY(short, sink)
    };
    class CSEdge
    {

    };
    class CSFace
    {
        ADD_PROPERTY(MeshLib::CPoint, normal)
        ADD_PROPERTY(MeshLib::CPoint, direction)
        ADD_PROPERTY(short, parallel)
        ADD_PROPERTY(int, index)
        ADD_PROPERTY(bool, fixed)
    };
    class CSHalfEdge
    {
        ADD_PROPERTY(MeshLib::CPoint, normal)
        ADD_PROPERTY(Mark, mark)
    };
    using CMesh = MeshLib::CBaseMesh<CSVertex, CSEdge, CSFace, CSHalfEdge>;
    using CVertex = typename CMesh::CVertex;
    using CEdge = typename CMesh::CEdge;
    using CFace = typename CMesh::CFace;
    using CHalfEdge = typename CMesh::CHalfEdge;
    using CLoop = MeshLib::CLoop<CSVertex, CSEdge, CSFace, CSHalfEdge>;
    using CBoundary = MeshLib::CBoundary<CSVertex, CSEdge, CSFace, CSHalfEdge>;
    using CPoint = MeshLib::CPoint;

public:
    CSimplicialFoliation();
    ~CSimplicialFoliation();

    int readMesh(string filename);

    int setSourceSink();
    int calculateFoliationDirectionField();

    int smoothDirectionField(int numIterations = 100);

    int output(string filename);

private:
    CMesh * mesh;
};

