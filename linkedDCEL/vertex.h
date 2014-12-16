#ifndef VERTEX_H
#define VERTEX_H

#include "edge.h"
#include "line.h"
#include <cg/primitives/point.h>


struct Edge;

using cg::point_2;

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
};

#endif // VERTEX_H
