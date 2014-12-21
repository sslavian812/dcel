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
//    {
//        for(int i=0; i<3; ++i)
//        {
//            for(int j=0; j<3; ++j)
//            {
//                segment_2t<Vertex::interval> s1(other->v[i]->getIntervalPoint(), other->v[(i+1)%3]->getIntervalPoint());
//                segment_2t<Vertex::interval> s2(other->v[j]->getIntervalPoint(), other->v[(j+1)%3]->getIntervalPoint());
//                if(has_intersection(s1, s2))
//                    return true;
//            }
//        }

//        // one inside the other or no intersection

//        bool flag = true;

//        for(int i=0; i<3; ++i)
//        {
//            if(! Vertex::leftTurn(v[i], v[(i+1)%3]), other->v[0])
//            {
//                flag = false;
//                break;
//            }
//        }

//        if(flag) return true;

//        flag = true;
//        for(int i=0; i<3; ++i)
//        {
//            if(! Vertex::leftTurn(other->v[i], other->v[(i+1)%3]), v[0])
//            {
//                flag = false;
//                break;
//            }
//        }

//        return flag;
//    }

    bool contains(point_2 p);
//    {
//        for(int i=0; i<3; ++i)
//        {
//            if(!Vertex::leftTurn(v[i], v[(i+1)%3]), p)
//            {
//                return false;
//            }
//        }
//        return true;
//    }
};
