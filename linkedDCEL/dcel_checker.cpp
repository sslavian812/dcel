#include "linked_triangle_dcel.h"


bool LinkedTriangleDcel::isNonCollinear(int f)
{
    if(f ==0)
        return true;
    Face* face = faces[f];
    Edge* start_edge = face->startEdge;
    Edge* cur_edge = start_edge->next;

    bool flag = false;
    do
    {
        Vertex* v1 = cur_edge->origin;
        Vertex* v2 = cur_edge->next->origin;
        Vertex* v3 = cur_edge->next->next->origin;

        if(Vertex::leftTurn(v1,v2,v3) || Vertex::leftTurn(v3,v2,v1))
        {
            flag = true;
            break;
        }

        cur_edge = cur_edge->next;
    }while(cur_edge != start_edge);

    if(!flag)
        cerr<<"face "<<f<<": "<<" IS COLLINEAR !!!"<<endl<<endl;
    return flag;

}

bool LinkedTriangleDcel::traverseFace(int f)
{
    Face* face = faces[f];
    Edge* start_edge = face->startEdge;
    Edge* cur_edge = start_edge->next;

    int k = 0;
    int max = 10000;

    map<Edge*, bool> used;

    bool res = true;

    while(cur_edge != start_edge)
    {
        used[cur_edge] = true;
        Edge* next_edge = cur_edge->next;

//        if(f!=0 && cur_edge->line != NULL && next_edge->line != NULL
//                && cg::orientation(point_2(0,0),
//                           cur_edge->line->getDirection(),
//                           next_edge->line->getDirection()) != cg::CG_LEFT)
//        {
//            cerr<<"edge "<<cur_edge<<" int face "<<f<<" has incorrect TURN!!"<<endl;
//            res = false;
//        }


        if(cur_edge->incidentFace != face)
        {
            cerr<<"edge "<<cur_edge<<" has incorrect associated face"<<endl;
            res = false;
        }

        if(used[next_edge] == true)
        {
            cerr<<"edge "<<next_edge<<" has been visited twice in face "<<f<<endl;
            res = false;
        }

        if(next_edge == NULL )
        {
            cerr<<"edge "<<cur_edge<<" is invalid and has NULL as it's next"<<endl;
            return false;
        }

        if(k>=max)
        {
            cerr<<"face "<<f<<"has more than "<<max<<"edges "<<endl;
            return false;
        }

        if(next_edge->prev != cur_edge)
        {
            cerr<<"edge "<<next_edge<<" has incorrect prev pointer "<<next_edge->prev<<" instead of "<<cur_edge<<endl;
            return false;
        }
        cur_edge = next_edge;
        ++k;
    }
    if(!res)
        cerr<<"ended checking face "<<f<<": "<<endl<<endl;
    return res;
}

bool LinkedTriangleDcel::checkConsistensy(string s)
{
    bool res = true;
    for(int i=0; i<faces.size(); ++i)
    {
        if( !traverseFace(i) || !isNonCollinear(i))
        {
            std::cerr<<"face "<<i<<" is incorrect"<<std::endl;
            res = false;
        }
    }

    for(int i=0; i<vertices.size(); ++i)
    {
        if(vertices[i]->incidentEdge == NULL)
        {
            std::cerr<<"warning: vertices["<<i<<"]->incidentEdge is NULL"<<std::endl;
            res = false;
        }
        else if(std::find(edges.begin(), edges.end(), vertices[i]->incidentEdge) == edges.end())
        {
            std::cerr<<"warning: vertices["<<i<<"]->incidentEdge doesn't present"<<std::endl;
            res = false;
        }
    }

    if(res)
        std::cerr<<s<<": dcel is CORRECT!"<<std::endl;
    else
        std::cerr<<s<<": dcel is INCORRECT!"<<std::endl;
    return res;
}

bool LinkedTriangleDcel::isTriangle(int f)
{
    Face* face = faces[f];
    Edge* start_edge = face->startEdge;
    if(start_edge->next->next->next == start_edge)
        return true;
    else
        return false;
}

bool LinkedTriangleDcel::triangleCheck()
{
    bool res = true;
    for(int i=1; i<faces.size(); ++i)
    {
        if( !isTriangle(i) )
        {
            std::cerr<<"face "<<i<<" is incorrect"<<std::endl;
            res = false;
        }
    }
    if(res)
        std::cerr<<": dcel is Triangulated!"<<std::endl;
    else
        std::cerr<<": dcel is NOT triangulated!"<<std::endl;
    return res;
}
