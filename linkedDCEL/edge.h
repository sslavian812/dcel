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

    std::pair<point_2, point_2> getCoords(Vertex* inf);
};

#endif // EDGE_H
