#ifndef FACE_H
#define FACE_H

#include<vector>
using std::vector;

#include "edge.h"
#include "vertex.h"
#include "triangle.h"

using std::pair;

struct Edge;
struct Vertex;

struct Face
{
    Edge* startEdge;
    Triangle * triangle;

    Face(): startEdge(NULL), triangle(NULL)
    {}

    Face(Edge* edge): startEdge(edge), triangle(NULL)
    {}

    Face(const Face & other): startEdge(other.startEdge), triangle(other.triangle)
    {}

    vector<Vertex*> getVertices();
    Edge* getStrongEdge();
    vector<pair<point_2, point_2> > getEdgesToDraw();
};

#endif // FACE_H
