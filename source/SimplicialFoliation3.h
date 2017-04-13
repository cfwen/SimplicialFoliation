#pragma once

#include <vector>
#include <string>
#include "TetMesh/tmesh.h"
//#include "TetMesh/boundary.h"

using namespace std;

#define ADD_PROPERTY(T, x) \
private:\
    T m_##x; \
public:\
    T & x() { return m_##x; } \



class CSimplicialFoliation3
{
public:
    using CPoint = MeshLib::CPoint;
    enum Mark { SOURCE, SINK, PARALLEL, FREE };
    class CSTVertex
    {

    };
    class CSVertex
    {
        ADD_PROPERTY(Mark, mark)
        ADD_PROPERTY(short, source)
        ADD_PROPERTY(short, sink)
    };
    class CSTEdge
    {

    };
    class CSHalfEdge
    {
        
    };
    class CSEdge
    {

    };
    class CSHalfFace
    {
        ADD_PROPERTY(Mark, mark)
        ADD_PROPERTY(MeshLib::CPoint, normal)
    };
    class CSFace
    {
        
    };
    class CSTet
    {
        ADD_PROPERTY(CPoint, direction)
        ADD_PROPERTY(short, parallel)
        ADD_PROPERTY(int, index)
        ADD_PROPERTY(bool, fixed)
        ADD_PROPERTY(bool, touched)
    };
    
    using CTMesh = TMeshLib::CBaseTMesh<CSTVertex, CSVertex, CSHalfEdge, CSTEdge, CSEdge, CSHalfFace, CSFace, CSTet>;
    using CTVertex = typename CTMesh::CTVertex;
    using CVertex = typename CTMesh::CVertex;
    using CHalfEdge = typename CTMesh::CHalfEdge;
    using CTEdge = typename CTMesh::CTEdge;
    using CEdge = typename CTMesh::CEdge;
    using CHalfFace = typename CTMesh::CHalfFace;
    using CFace = typename CTMesh::CFace;
    using CTet = typename CTMesh::CTet;    
    
public:
    CSimplicialFoliation3();
    ~CSimplicialFoliation3();

    int readMesh(string filename);

    int setSourceSink();
    int calculateFoliationDirectionField();
    int smoothDirectionField(int numIterations = 100);

    int output(string filename);

private:

    CTMesh * tmesh;

};

