#ifndef FACE_H
#define FACE_H


#include<vector>
using std::vector;

#include "edge.h"
#include "vertex.h"
#include "triangle.h"


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

    vector<Vertex*> getVertices();
    Edge* getStrongEdge();

//    bool contains(point_2 p)
//    {
//        Edge* cur = startEdge;
//        do
//        {
//            if(cg::orientation(point_2(0,0),
//                               cur->line->getDirection(),
//                               point_2(p.x-cur->origin->getPoint().x, p.y-cur->origin->getPoint().y)) == cg::CG_RIGHT)
//            {
//                return false;
//            }
//            cur = cur->next;
//        }while(cur != startEdge);
//        return true;
//    }
};

#endif // FACE_H
