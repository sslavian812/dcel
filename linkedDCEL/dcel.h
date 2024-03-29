#ifndef DCEL_H
#define DCEL_H

#include <cg/primitives/point.h>
#include "line.h"
#include<vector>

using std::vector;
using std::pair;

struct Dcel
{
    virtual void getAllToDraw(std::vector<point_2> &res_vertices,
                              vector<pair<point_2, point_2> > &res_edges,
                              vector<pair<point_2, point_2> > &tr_edges) const = 0;
    virtual void getBounds(Line &l1, Line &l2, Line &l3) = 0;
    virtual bool addLine(double a, double b, double c) = 0;
    virtual void localize(point_2f p, vector<pair<point_2, point_2> > &v) const = 0;
};


#endif // DCEL_H
