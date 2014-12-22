#include"face.h"


vector<Vertex*> Face::getVertices()
{
    vector<Vertex*> res;
    Edge* cur = startEdge;
    do
    {
        res.push_back(cur->origin);
        cur = cur->next;
    }while(cur != startEdge);
    return res;
}

Edge* Face::getStrongEdge()
{
    Edge* cur = startEdge;
    do
    {
        if(cur->line !=  NULL)
            return cur;
        cur = cur->next;
    }while(cur != startEdge);
}

vector<pair<point_2, point_2> > Face::getEdgesToDraw()
{
    vector<pair<point_2, point_2> > v;
    Edge* cur_edge = startEdge;
    do
    {
        v.push_back(cur_edge->getCoords(NULL));
        cur_edge = cur_edge->next;
    }while(cur_edge != startEdge);
    return v;
}



