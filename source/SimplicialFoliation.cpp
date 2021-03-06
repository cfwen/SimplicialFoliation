// SimplicialFoliation.cpp : Defines the entry point for the console application.
//
#include <unordered_set>
#include <queue>
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

    for (auto f : mesh->faces())    
    {
        f->touched() = false;
        CHalfEdge * he1 = f->halfedge();
        CHalfEdge * he2 = he1->next();
        CPoint v1 = he1->target()->point() - he1->source()->point();
        CPoint v2 = he2->target()->point() - he2->source()->point();
        CPoint n = v1^v2;
        f->normal() = n / n.norm();
    }
    for (auto he : mesh->halfedges())    
    {
        CPoint v = he->target()->point() - he->source()->point();
        v /= v.norm();
        he->normal() = v^he->face()->normal();
    }

    queue<CHalfEdge*> front;
    CHalfEdge * hes = NULL;
    CHalfEdge * het = NULL;

    CBoundary boundary(mesh);
    CLoop * loop = boundary.loops()[0];
    vector<CHalfEdge*> bd;
    for (auto he : loop->halfedges()) bd.push_back(he);
    bd.insert(bd.begin(), bd.begin(), bd.end());
    
    auto it = bd.begin();
    // find a source he
    for (; it != bd.end(); ++it)
    {
        CHalfEdge * he = *it;
        if (he->mark() == Mark::SOURCE) break;
    }
    // find first non-source he, which is begining of parallel he
    for (; it != bd.end(); ++it)
    {
        CHalfEdge * he = *it;
        if (he->mark() != Mark::SOURCE) break;
    }

    int k = 0;
    // find parallel he from source to sink
    for (; it != bd.end(); ++it)
    {
        CHalfEdge * he = *it;
        if (he->mark() == Mark::PARALLEL)
        {
            CPoint v = he->target()->point() - he->source()->point();
            v /= v.norm();
            CFace * f = he->face();
            f->direction() = v;
            f->touched() = true;
            f->index() = ++k;
        }
        else break;
    }
    // find sink he
    for (; it != bd.end(); ++it)
    {
        CHalfEdge * he = *it;
        if (he->mark() == Mark::SINK)
        {
            CFace * f = he->face();
            f->direction() = he->normal();
            f->touched() = true;
            f->index() = ++k;
            het = he;
        }
        else break;
    }
    // find sink to source he
    for (; it != bd.end(); ++it)
    {
        CHalfEdge * he = *it;
        if (he->mark() == Mark::PARALLEL)
        {
            CPoint v = he->target()->point() - he->source()->point();
            v /= v.norm();
            CFace * f = he->face();
            f->direction() = -v;
            f->touched() = true;
            f->index() = ++k;
        }
        else break;
    }    
    // find source he
    for (; it != bd.end(); ++it)
    {
        CHalfEdge * he = *it;
        if (he->mark() == Mark::SOURCE)
        {
            CFace * f = he->face();
            f->direction() = -he->normal();
            f->touched() = true;
            f->index() = ++k;
            hes = he;
            front.push(he);
        }
        else break;
    }
    
    if (!hes || !het)
    {
        cerr << "no source or sink specified" << endl;
        return -1;
    }
    //front.push(hes);

    while (true)
    {
        if (front.empty()) break;
        CHalfEdge * he = front.front();
        front.pop();
        CFace * f = he->face();
        
        CPoint ns = CPoint(0, 0, 0);
        CHalfEdge * hp = NULL;

        for (CMesh::FaceHalfEdgeIterator feit(f); !feit.end(); ++feit)
        {
            CHalfEdge * he = *feit;
            auto mark = he->mark();
            if (mark == Mark::SOURCE)
            {
                ns -= he->normal();
            }
            else if (mark == Mark::PARALLEL)
            {
                hp = he;
            }
            else // sink  or free
            {
                ns += he->normal();
                he->mark() = Mark::SOURCE;
            }

            if (he->dual() && !he->dual()->face()->touched())
            {
                if (he->dual()->mark() == Mark::FREE)
                {
                    he->dual()->mark() = Mark::SOURCE;
                    front.push(he->dual());
                }
            }
        }
        if (f->touched()) continue;

        if (he->dual() && he->dual()->face()->touched()) ns += he->dual()->face()->direction();
        ns /= ns.norm();
        ns -= f->normal()*(f->normal()*ns);
        f->direction() = ns;
        f->index() = ++k;
        f->touched() = true;
    }

    smoothDirectionField(20);

    return 0;
}

int CSimplicialFoliation::smoothDirectionField(int numIterations)
{
    int k = 0;
    while (k++ < numIterations)
    {
        for (auto v : mesh->vertices())
        {        
            CPoint ps = CPoint(0, 0, 0);
            for (CMesh::VertexFaceIterator fit(v); !fit.end(); ++fit)
            {
                CFace * f = *fit;
                ps += f->direction();
            }
            ps /= ps.norm();
            for (CMesh::VertexFaceIterator fit(v); !fit.end(); ++fit)
            {
                CFace * f = *fit;
                if (!f->fixed())
                {
                    ps -= f->normal()*(f->normal()*ps);
                    f->direction() = ps;
                }                
            }
        }
    }
    
    return 0;
}


int CSimplicialFoliation::output(string filename)
{
    for (auto f : mesh->faces())    
    {
        ostringstream oss;
        oss << "direction=(" << f->direction() << ")";
        oss << " index=(" << f->index() << ")";
        f->string() = oss.str();
    }
    mesh->write_m(filename.c_str());
    return 0;
}


int CSimplicialFoliation::setSourceSink()
{
    for (auto he : mesh->halfedges())
    {
        string type = he->edge()->string();
        if (type == string("source"))
        {
            he->mark() = Mark::SOURCE;
            he->face()->fixed() = true;
        }
        else if (type == string("sink"))
        {
            he->mark() = Mark::SINK;
            he->face()->fixed() = true;
        }
        else
        {
            if (he->boundary())
            {
                he->mark() = Mark::PARALLEL;
                he->face()->fixed() = true;
            }
            else
            {
                he->mark() = Mark::FREE;
            }
        }
    }
    return 0;
}
