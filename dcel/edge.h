#ifndef EDGE_H
#define EDGE_H

#include "vertex.h"
#include "face.h"
#include "line.h"

struct Edge
{
    int origin; // Vertex
    int incidentFace; //Face
    int next; // Edge
    int prev; // Edge
    int twin; // Edge

    int line; //Line

    Edge(int origin, int line): origin(origin), incidentFace(-1), line(line)
    {}
};

#endif // EDGE_H
