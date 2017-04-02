// SimplicialFoliation.cpp : Defines the entry point for the console application.
//
#include <unordered_set>
#include "SimplicialFoliation.h"


CSimplicialFoliation::CSimplicialFoliation()
{
    mesh = new CMesh();
}


CSimplicialFoliation::~CSimplicialFoliation()
{
}

int CSimplicialFoliation::readMesh(string filename)
{
    mesh->read_m(filename.c_str());

    return 0;
}

int CSimplicialFoliation::calculateFoliationDirectionField()
{
    setSourceSink();

    unordered_set<CMesh::CHalfEdge*> front;

    while (true)
    {
        if (front.empty()) break;
        CMesh::CHalfEdge * cs = *front.begin();
        CMesh::CFace * f = cs->face();
        if (!f) break;

        bool isParallel = false;
        for (CMesh::FaceHalfedgeIterator feit(f); !feit.end(); ++feit)
        {
            CMesh::CHalfEdge * he = *feit;
            int mark = he->mark();
            if (mark == 1) // source
            {

            }
            else if (mark == 2) // parallel
            {

            }
            else // (-1) sink  or (0) unmarked
            {

            }

            he->mark() = 1;

            if (he->he_sym())
            {
                int mark2 = he->he_sym()->mark();
                if (mark2 == 0)	front.insert(he->he_sym());
            }

        }
        MeshLib::CPoint * d = chooseDirection(f);

        f->direction() = d;
        f->touched() = true;
    }

    return 0;
}

MeshLib::CPoint * CSimplicialFoliation::chooseDirection(CFace * f)
{
    return nullptr;
}

int CSimplicialFoliation::output(string filename)
{
    return 0;
}


int CSimplicialFoliation::setSourceSink()
{
    return 0;
}

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        cout << "usage: SimplicialFoliation.exe [mesh]" << endl;
    }

    CSimplicialFoliation sf;
    string filename(argv[1]);
    sf.readMesh(filename);
    sf.calculateFoliationDirectionField();

    if (argc == 3)
    {
        string outfilename(argv[2]);
        sf.output(outfilename);
    }
    else
    {
        sf.output(filename);
    }
    return 0;
}
