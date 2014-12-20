#ifndef VERTEX_H
#define VERTEX_H

#include "edge.h"
#include "line.h"
#include <cg/primitives/point.h>

#include <boost/numeric/interval.hpp>
#include <gmpxx.h>



struct Edge;

using cg::point_2;
using cg::point_2t;

struct Vertex
{
    point_2 point;

    Line* line1;
    Line* line2;
    Edge* incidentEdge;

    bool isOnBorder;

    Vertex(point_2 point): point(point), incidentEdge(NULL), isOnBorder(false)
    {}

    Vertex(double x, double y): point(point_2(x,y)), incidentEdge(NULL), isOnBorder(false)
    {}

    Vertex(Line* l1, Line* l2, point_2 point, Edge* edge = NULL): line1(l1), line2(l2), point(point), incidentEdge(edge), isOnBorder(false)
    {}

    Vertex(Line* l1, Line* l2, Edge* edge = NULL): line1(l1), line2(l2), incidentEdge(edge), isOnBorder(false)
    {point = l1->intersect(*l2);}

    Vertex(double x, double y, Edge* edge): point(point_2(x,y)), incidentEdge(edge), isOnBorder(false)
    {}

    Vertex(Line* l1, Line* l2, double x, double y, Edge* edge = NULL): line1(l1), line2(l2), incidentEdge(edge), isOnBorder(false)
    {point = point_2(x,y);}


    point_2 getPoint() const
    {return point;}

    typedef boost::numeric::interval_lib::unprotect<boost::numeric::interval<double> >::type interval;

    point_2t<interval> getIntervalPoint()
    {
        boost::numeric::interval<double>::traits_type::rounding _;

        point_2t<interval> p = line1->intervalIntersect(*line2);
        return p;
    }

    point_2t<mpq_class> getMpqPoint()
    {
        point_2t<mpq_class> p = line1->mpqIntersect(*line2);
        return p;
    }

    static bool leftTurn(Vertex* v1, Vertex* v2, Vertex* v3)
    {
        //interval
        boost::numeric::interval<double>::traits_type::rounding _;

        point_2t<interval> a = v1->getIntervalPoint();
        point_2t<interval> b = v2->getIntervalPoint();
        point_2t<interval> c = v3->getIntervalPoint();

        interval res =   (b.x - a.x) * (c.y - a.y)
                       - (b.y - a.y) * (c.x - a.x);

        if (res.lower() > 0)
           return true;
        if (res.upper() < 0)
           return false;
        if (res.upper() == res.lower())
           return false;

        // rational

        point_2t<mpq_class> d = v1->getMpqPoint();
        point_2t<mpq_class> e = v2->getMpqPoint();
        point_2t<mpq_class> f = v3->getMpqPoint();

        mpq_class res1 =   (e.x - d.x) * (f.y - d.y)
                        - (e.y - d.y) * (f.x - d.x);

        int cres = cmp(res1, 0);

        if(cres > 0)
            return true;
        else
            return false;
    }
};

#endif // VERTEX_H
