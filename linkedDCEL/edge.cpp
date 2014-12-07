#include "edge.h"
#include "vertex.h"
#include "face.h"
#include "line.h"
#include<cg/primitives/point.h>

Edge::Edge(Vertex* origin, Line* line): origin(origin), incidentFace(NULL), line(line)
{}

Edge::Edge(): origin(NULL), incidentFace(NULL), line(NULL)
{}

std::pair<point_2, point_2> Edge::getCoords(Vertex* inf)
{

    std::pair<point_2, point_2> res = line->getSegment();
    if(origin != inf)
        res.first = origin->getPoint();
    if(twin->origin != inf)
        res.second = twin->origin->getPoint();
    return res;
}
