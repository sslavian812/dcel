#include "linked_triangle_dcel.h"

typedef list<Vertex*>::iterator lvi;

list<Vertex*> LinkedTriangleDcel::constructBorder(Face* face)
{
    Edge* cur_edge = face->startEdge;
    Edge* start_edge = cur_edge;
    list<Vertex*> l;
    do
    {
        l.insert(l.end(),cur_edge->origin);
        cur_edge = cur_edge->next;
    }while(cur_edge != start_edge);
    return l;
}

bool LinkedTriangleDcel::isEar(list<Vertex*> border, Vertex* v1, Vertex* v2, Vertex* v3)
{
    lvi it = border.begin();
    while(it != border.end())
    {
        if(*it == v1 || *it == v2 || *it == v3)
        {
            it++;
            continue;
        }
        if(!Vertex::leftTurn(*it, v2, v1) && !Vertex::leftTurn(*it, v3, v2) && !Vertex::leftTurn(*it, v1, v3))
            return false;
        it++;
    }
    return true;
}

Face* LinkedTriangleDcel::addEdge(Vertex* from, Vertex* to, Face* bigface)
{
    Edge* e1 = new Edge(from, NULL);
    Edge* e2 = new Edge(to, NULL);
    edges.push_back(e1);
    edges.push_back(e2);
    e1->twin = e2;
    e2->twin = e1;

    Edge* cur_edge = bigface->startEdge;

    while(cur_edge->origin != from)
        cur_edge = cur_edge->next;

    e2->next = cur_edge;
    e2->prev = cur_edge->next;
    e1->next = cur_edge->next->next;
    e1->prev = cur_edge->prev;

    cur_edge->prev->next = e1;
    e1->next->prev = e1;
    cur_edge->prev = e2;
    cur_edge->next->next = e2;

    bigface->startEdge = e1;
    e1->incidentFace = bigface;

    Face* smallface = new Face(e2);
    e2->incidentFace =smallface;
    cur_edge->incidentFace = smallface;
    cur_edge->next->incidentFace = smallface;
    faces.push_back(smallface);
    return smallface;
}

vector<Triangle*> LinkedTriangleDcel::triangulateFace(Face* face)
{
    vector<Face*> fs;
    fs.push_back(face);
    list<Vertex*> border = constructBorder(face);
    int border_size = border.size();

    lvi it1=border.begin();
    lvi it2;
    lvi it3;
    while(border_size > 3)
    {
        it2 = it1;

        Vertex* v1 = *it2; ++it2;
        if(it2 == border.end()) it2=border.begin();
        it3=it2;
        Vertex* v2 = *it3; ++it3;
        if(it3 == border.end()) it3=border.begin();
        Vertex* v3 = *it3;

        if(Vertex::leftTurn(v1,v2,v3))
        {
            if(isEar(border, v1,v2,v3))
            {
                border.erase(it2);
                border_size--;
                fs.push_back(addEdge(v1, v3, face)); // gives new face
                continue;
            }
        }

        ++it1;
        if(it1 == border.end()) it1=border.begin();

    }

    vector<Triangle*> res;
    for(int i=0; i<fs.size(); ++i)
    {
        //leek!                // never deleted!
        fs[i]->triangle = new Triangle(fs[i]->getVertices(), fs[i]->getStrongEdge());
        res.push_back(fs[i]->triangle);
    }

    return res;
}

void LinkedTriangleDcel::triangulateDcel()
{
    for(int i=1; i<faces.size(); ++i)
    {
        triangulateFace(faces[i]);
    }
}
