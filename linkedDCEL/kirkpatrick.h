#pragma once

#include <vector>
using std::vector;

#include "triangle.h"
#include "linked_triangle_dcel.h"
#include <map>
using std::map;

struct Kirkpatrick
{
//    vector<LinkedTriangleDcel*> lauers_d;
    vector<vector<Triangle*> >  lauers_t;
    Triangle* root;

    int countPower(Vertex* v)
    {
        //todo
        return 0;
    }

    vector<Triangle*> getAllTriangles(Veretex* v)
    {
        //todo
        return {};
    }


    Kirkpatrick(LinkedTriangleDcel* dcel)
    {
        dcel->triangulateDcel();
        lauers_t.push_back({}); // создали нулевой слой

        for(int i=1; i<dcel->faces.size(); ++i)      // перебрать все фейсы.
        {
            Triangle* triangle = dcel->faces[i]->triangle;
            lauers_t[0].push_back(triangle);
        }

        // now the main part: removing vertices and construction

        while(true)
        {
            if(dcel->vertices.size()==3)
                break;

            map<Vertex*, bool> used;
            vector<Vertex*> victims;

            for(int i=3; i<vertices.size(); ++i)  // найдем жертв
            {
                bool flag = true;
                if(countPower(dcel->vertices[i]) < 12)
                {
                    Edge* cur = dcel->vertices[i]->incidentEdge;
                    do
                    {
                        if(used[cur->twin->origin])
                        {
                            flag = false;
                            break;
                        }
                        cur = cur->twin->next;
                    }while(cur!=dcel->vertices[i]->incidentEdge);

                    if(flag)
                    {
                        used[dcel->vertices[i]]=true;
                        victims.push_back(dcel->vertices[i]);
                    }
                }
            }

            for(int i=0; i<victims.size(); ++i)   // удалим по одной и пролинкуем
            {
                // старые вокруг жертвы
                vector<Triangle*> oldTriangles = getAllTriangles(victims[i]);

                //удалим жертву и триангулируем фигуру. получим новые треугольники
                Face bigFace = dcel->deleteVertex(victims[i]);
                vector<Face*> newFaces = dcel->triangulateFace(bigFace);
                vector<Triangle*> newTriangles;
                for(int j=0; j<newFaces.size(); ++j)
                    newTriangles.push_back(newFaces[j]->triangle);

                //теперь надо попересекать их
                for(int j=0; j<newTriangles.size(); ++j)
                {
                    for(int k=0; k<oldTriangles.size(); ++k)
                    {
                        if(oldTriangles[k]->intersects(newTriangles[j]))
                            newTriangles[j]->successors.push_back(oldTriangles[k]);
                    }
                }
            }

            //теперь надо запушать все в следующий уровень
            lauers_t.push_back({});
            for(int i=1; i<dcel->faces.size(); ++i)      // перебрать все фейсы.
            {
                Triangle* triangle = dcel->faces[i]->triangle;
                // у triangle уже натянуты линки на нижний уровень

                lauers_t[lauers_t.size()-1].push_back(triangle);
            }
        }

        root = dcel->faces[1]->triangle;
    }

    Face* localize(point_2)
    {
        if(! root->triangle->contains(p))
            return lauers_d[0]->faces[0];
        else
        {
            Node* u = root;
            while(u->successors.size())
            {
                for(int i=0; i<u->successors.size(); ++i)
                {
                    if(u->successors[i]->triangle->contains(p))
                    {
                        u=u->successors[i];
                        break;
                    }
                }
            }
            return u->triangle->e->incidentFace;
        }
    }

};
