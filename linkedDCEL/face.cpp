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


