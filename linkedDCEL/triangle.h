#pragma once

#include"triangle.h"

#include<vector>
using std::vector;

#include"edge.h"
#include"vertex.h"
#include <cg/primitives/point.h>
using cg::point_2;


struct Vertex;
struct Edge;

struct Triangle
{
    vector<Vertex*> v;
    Edge* e;
    vector<Triangle*> successors;

    Triangle(vector<Vertex*> v, Edge* e = NULL): v(v), e(e)
    {}

    bool intersects(Triangle* other);

    bool contains(point_2 p);
};
