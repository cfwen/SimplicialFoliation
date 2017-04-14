#include <queue>
#include "SimplicialFoliation3.h"



CSimplicialFoliation3::CSimplicialFoliation3()
{
    tmesh = new CTMesh();
}


CSimplicialFoliation3::~CSimplicialFoliation3()
{
}

int CSimplicialFoliation3::readMesh(string filename)
{
    tmesh->_load_t(filename.c_str());

    tmesh->_write_t("aa");
    return 0;
}

int CSimplicialFoliation3::setSourceSink()
{
    std::list<CTet*> & tets = tmesh->tets();
    for (auto tet : tets)
    {
        tet->fixed() = false;
    }
    bool hasSource = false;
    bool hasSink = false;

    // source/sink can be labelled on halfface
    std::list<CHalfFace*> halffaces = tmesh->halffaces();
    for (auto hf : halffaces)
    {
        std::string type = hf->face()->string();
        if(type == string("source"))
        {
            hf->mark() = Mark::SOURCE;
            hf->tet()->fixed() = true;
            hasSource = true;
        }
        else if (type == string("sink"))
        {
            hf->mark() = Mark::SINK;
            hf->tet()->fixed() = true;
            hasSink = true;
        }
        else
        {
            if (!hf->dual())
            {
                hf->mark() = Mark::PARALLEL;
            }
            else
            {
                hf->mark() = Mark::FREE;
            }
        }
    }

    if (hasSource && hasSink) return 0;
    
    // source/sink labelled on vertex
    std::list<CVertex*> & vertices = tmesh->vertices();
    for (auto vertex : vertices)
    {
        std::string type = vertex->string();
        if (type == string("ss=(1)"))
        {
            vertex->mark() = Mark::SOURCE;
        }
        if (type == string("ss=(-1)"))
        {
            vertex->mark() = Mark::SINK;
        }
        if (type == string("singular source"))
        {
            
        }
        if (type == string("singular sink"))
        {

        }
    }
    for (auto hf : halffaces)
    {
        CVertex * v0 = hf->halfedge()->source();
        CVertex * v1 = hf->halfedge()->target();
        CVertex * v2 = hf->halfedge()->next()->target();
        if (v0->mark() == Mark::SOURCE && v1->mark() == Mark::SOURCE && v2->mark() == Mark::SOURCE)
        {
            hf->mark() = Mark::SOURCE;
            hf->tet()->fixed() = true;
            hasSource = true;
        }
        if (v0->mark() == Mark::SINK && v1->mark() == Mark::SINK && v2->mark() == Mark::SINK)
        {
            hf->mark() = Mark::SINK;
            hf->tet()->fixed() = true;
            hasSink = true;
        }
    }
    
    if (!hasSource || !hasSink)
    {
        cout << "no source or no sink" << endl;
    }

    return 0;
}

int CSimplicialFoliation3::calculateFoliationDirectionField()
{
    setSourceSink();

    std::queue<CHalfFace*> front;

    // calculate halfface normal
    std::list<CHalfFace*> & halffaces = tmesh->halffaces();
    for (auto hf : halffaces)
    {
        CHalfEdge * he1 = hf->halfedge();
        CHalfEdge * he2 = he1->next();
        CPoint v1 = he1->target()->point() - he1->source()->point();
        CPoint v2 = he2->target()->point() - he2->source()->point();
        CPoint n = v1^v2;
        hf->normal() = n / n.norm();

        if (hf->mark() == Mark::SOURCE)
        {
            hf->tet()->direction() = -hf->normal();
            front.push(hf);
        }
        if (hf->mark() == Mark::SINK)
        {
            hf->tet()->direction() = hf->normal();
        }
    }

    std::list<CTet*> & tets = tmesh->tets();
    for (auto tet : tets)
    {
        tet->touched() = false;
    }

    int k = 0;
    while (true)
    {
        if (front.empty()) break;
        CHalfFace * hf = front.front();
        front.pop();
        CTet * t = hf->tet();

        CPoint ns = CPoint(0, 0, 0);
        CHalfFace * hp = NULL;

        for (int i = 0; i < 4; ++i)
        {
            CHalfFace * hf = t->halfface(i);

            auto mark = hf->mark();
            if (mark == Mark::SOURCE)
            {
                ns -= hf->normal();
            }
            else if (mark == Mark::PARALLEL)
            {
                hp = hf;
            }
            else // sink  or free
            {
                ns += hf->normal();
                hf->mark() = Mark::SOURCE;
            }

            if (hf->dual() && !hf->dual()->tet()->touched())
            {
                if (hf->dual()->mark() == Mark::FREE)
                {
                    hf->dual()->mark() = Mark::SOURCE;
                    front.push(hf->dual());
                }
            }
        }
        if (t->touched()) continue;

        if (hf->dual() && hf->dual()->tet()->touched()) ns += hf->dual()->tet()->direction();
        ns /= ns.norm();
        if (hf->mark() == Mark::PARALLEL)
        {
            ns -= hf->normal()*(hf->normal()*ns);
        }
        t->direction() = ns;

        t->index() = ++k;
        t->touched() = true;
    }

    smoothDirectionField(100);

    return 0;
}

int CSimplicialFoliation3::smoothDirectionField(int numIterations)
{
    int k = 0;
    while (k++ < numIterations)
    {
        std::list<CVertex*> & vertices = tmesh->vertices();
        for (auto v : vertices)
        {
            CPoint ps = CPoint(0, 0, 0);
            std::list<CTVertex*> & tvertices = v->tvertices();
            for (auto tv : tvertices)
            {
                CTet * t = tv->tet();
                ps += t->direction();
            }
            ps /= ps.norm();
            for (auto tv : tvertices)
            {
                CTet * t = tv->tet();
                if (!t->fixed())
                {
                    t->direction() = ps;
                }
            }
        }
        std::list<CHalfFace*> & halffaces = tmesh->halffaces();
        for (auto hf : halffaces)
        {
            if (hf->mark() == Mark::PARALLEL)
            {
                CTet * t = hf->tet();
                CPoint v = t->direction();
                v -= hf->normal()*(hf->normal()*v);
                t->direction() = v;
            }
        }
    }

    return 0;
}

int CSimplicialFoliation3::output(string filename)
{
    std::list<CTet*> & tets = tmesh->tets();
    for (auto tet : tets)
    {
        ostringstream oss;
        oss << "direction=(" << tet->direction() << ")";
        tet->string() = oss.str();
    }
    std::list<CHalfFace*> & halffaces = tmesh->halffaces();
    for (auto hf : halffaces)
    {
        CTet * tet = hf->tet();
        if (!hf->dual())
        {
            tet->string() += " bd=(1)";
        }
        else
        {
            tet->string() += " bd=(0)";
        }
    }
    tmesh->_write_t(filename.c_str());
    
    return 0;
}

int test()
{
    using CTMesh = TMeshLib::CBaseTMesh<>;
    CTMesh * tmesh = new CTMesh();
    tmesh->_load("eight.tet");
    tmesh->_write_t("eight.t");

    using it = std::list<int>::iterator;
    
    return 0;
}
