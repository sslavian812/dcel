#ifndef EDGE_H
#define EDGE_H

#include "vertex.h"
#include "face.h"
#include "line.h"
#include<cg/primitives/point.h>

struct Vertex;
struct Face;
struct Line;

struct Edge
{
    Vertex* origin;
    Face* incidentFace;
    Edge* next;
    Edge* prev;
    Edge* twin;
    Line* line;

    Edge(Vertex* origin, Line* line);
    Edge();
    Edge(const Edge & other): origin(other.origin),
        incidentFace(other.incidentFace), next(other.next),
        prev(other.prev), twin(other.twin), line(other.line)
    {}

    std::pair<point_2, point_2> getCoords(Vertex* inf);
};

#endif // EDGE_H
