

#include<vector>
using std::vector;

#include"edge.h"
#include"vertex.h"

#include <cg/primitives/point.h>
using cg::point_2;

#include <cg/primitives/segment.h>
using cg::segment_2t;

#include <cg/operations/has_intersection/segment_segment.h>
using cg::has_intersection;


bool Triangle::intersects(Triangle* other)
{
    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            // rational calculations of double point, then checking intersection (double, interval, ratinal)
            // some accuracy lost, but it worth

            point_2 p1(other->v[i]->getMpqPoint().x.get_d(), other->v[i]->getMpqPoint().y.get_d());
            point_2 p2(other->v[(i+1)%3]->getMpqPoint().x.get_d(), other->v[(i+1)%3]->getMpqPoint().y.get_d());

            point_2 p3(other->v[j]->getMpqPoint().x.get_d(), other->v[j]->getMpqPoint().y.get_d());
            point_2 p4(other->v[(j+1)%3]->getMpqPoint().x.get_d(), other->v[(j+1)%3]->getMpqPoint().y.get_d());


            segment_2t<double> s1(p1, p2);
            segment_2t<double> s2(p3, p4);
            if(has_intersection(s1, s2))
                return true;
        }
    }

    // one triangle inside the other or no intersection to all
    bool flag = true;
    for(int i=0; i<3; ++i)
    {
        if(! Vertex::leftTurn(v[i], v[(i+1)%3], other->v[0]))
        {
            flag = false;
            break;
        }
    }

    if(flag) return true;

    flag = true;
    for(int i=0; i<3; ++i)
    {
        if(! Vertex::leftTurn(other->v[i], other->v[(i+1)%3], v[0]))
        {
            flag = false;
            break;
        }
    }

    return flag;
}

bool Triangle::contains(point_2 p)
{
    for(int i=0; i<3; ++i)
    {
        if(!Vertex::leftTurn(v[i], v[(i+1)%3], p))
        {
            return false;
        }
    }
    return true;
}

