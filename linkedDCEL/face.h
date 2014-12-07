#ifndef FACE_H
#define FACE_H

#include "edge.h"

struct Edge;

struct Face
{
    Edge* startEdge;

    Face(): startEdge(NULL)
    {}

    Face(Edge* edge): startEdge(edge)
    {}

//    bool isInside(point_2 point)
//    {
//        int cur = startEdge;
//        do
//        {
//            if(!cur.line.isOnLeft(p))
//                return false;
//            cur = cur.next;
//        }while (cur!= startEdge);
//        return true;
//    }
};

#endif // FACE_H
