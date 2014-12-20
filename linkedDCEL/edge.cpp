#include "edge.h"
#include "vertex.h"
#include "face.h"
#include "line.h"
#include<cg/primitives/point.h>

using std::pair;
using std::make_pair;

Edge::Edge(Vertex* origin, Line* line): origin(origin), incidentFace(NULL), line(line)
{}

Edge::Edge(): origin(NULL), incidentFace(NULL), line(NULL)
{}

std::pair<point_2, point_2> Edge::getCoords(Vertex* inf)
{
    if(line == NULL)
        return make_pair(this->origin->getPoint(), this->twin->origin->getPoint());
    std::pair<point_2, point_2> res = line->getSegment();
    if(origin != inf)
        res.first = origin->getPoint();
    if(twin->origin != inf)
        res.second = twin->origin->getPoint();
    return res;
}
