#pragma once

#include <vector>
#include <string>
#include "Mesh/mesh.h"

using namespace std;

#define ADD_PROPERTY(T, x) \
private:\
    T m_##x; \
public:\
    T & x() { return m_##x; } \

class CSimplicialFoliation
{
public:
    class CVertex
    {

    };
    class CEdge
    {

    };
    class CFace
    {
        ADD_PROPERTY(MeshLib::CPoint*, direction)
    };
    class CHalfEdge
    {
        ADD_PROPERTY(int, mark)
    };
    using CMesh = MeshLib::CBaseMesh<CVertex, CEdge, CFace, CHalfEdge>;

public:
	CSimplicialFoliation();
	~CSimplicialFoliation();

	int readMesh(string filename);

	int setSourceSink();    
	int calculateFoliationDirectionField();

    MeshLib::CPoint * chooseDirection(CFace * f);

	int output(string filename);

private:
	CMesh * mesh;
};

