#ifndef VERTEX_H
#define VERTEX_H

#include "edge.h"
#include <cg/primitives/point.h>

struct Vertex
{
    point_2 point;
    int line1;
    int line2;

    int incidentEdge;

    Vertex(point_2 point): point(point), incidentEdge(-1)
    {}

    Vertex(double x, double y): point(point_2(x,y)), incidentEdge(-1)
    {}

    Vertex(int l1, int l2, point_2 point, int edge = -1): line1(l1), line2(l2), point(point), incidentEdge(edge)
    {}

    Vertex(double x, double y, int edge): point(point_2(x,y)), incidentEdge(edge)
    {}

    Vertex(int l1, int l2, double x, double y, int edge = -1): line1(l1), line2(l2), incidentEdge(edge)
    {point = point_2(x,y);}

    //Vertex(int l1, int l2, ): line1(l1), line2(l2),     {}

    point_2 getPoint() const
    {
        return point;
    }
};

#endif // VERTEX_H
