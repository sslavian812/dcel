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
    vector<vector<Triangle*> >  lauers;
    Triangle* root;

    int countPower(Vertex* v)
    {
        //todo
        return 0;
    }

    vector<Triangle*> getAllTriangles(Vertex* v)
    {
        vector<Triangle*> res;
        Edge* cur = v->incidentEdge;
        do
        {
            if(cur->incidentFace->triangle != NULL)
                res.push_back(cur->incidentFace->triangle);
            cur = cur->twin->next;
        }while(cur!=v->incidentEdge);

        return res;
    }


    Kirkpatrick(LinkedTriangleDcel* dcel)
    {
        dcel->triangulateDcel();
        lauers.push_back({}); // создали нулевой слой

        for(int i=1; i<dcel->faces.size(); ++i)      // перебрать все фейсы.
        {
            Triangle* triangle = dcel->faces[i]->triangle;
            lauers[0].push_back(triangle);
        }

        // now the main part: removing vertices and construction

        while(true)
        {
            if(dcel->vertices.size()==3)
                break;

            map<Vertex*, bool> used;
            vector<Vertex*> victims;

            for(int i=3; i<dcel->vertices.size(); ++i)  // найдем жертв
            {
                bool flag = true;
                Vertex* vert = dcel->vertices[i];
                if(countPower(vert) < 12)
                {
                    Edge* cur = vert->incidentEdge;
                    do
                    {
                        if(used[cur->twin->origin])
                        {
                            flag = false;
                            break;
                        }
                        cur = cur->twin->next;
                    }while(cur!= vert->incidentEdge);

                    if(flag)
                    {
                        used[vert]=true;
                        victims.push_back(vert);
                    }
                }
            }

            for(int i=0; i<victims.size(); ++i)   // удалим по одной и пролинкуем
            {
                // старые вокруг жертвы
                vector<Triangle*> oldTriangles = getAllTriangles(victims[i]);

                //удалим жертву и триангулируем фигуру. получим новые треугольники
                Face* bigFace = dcel->deleteVertex(victims[i]);
                vector<Triangle*> newTriangles = dcel->triangulateFace(bigFace);


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
            lauers.push_back({});
            for(int i=1; i<dcel->faces.size(); ++i)      // перебрать все фейсы.
            {
                Triangle* triangle = dcel->faces[i]->triangle;
                // у triangle уже натянуты линки на нижний уровень

                lauers[lauers.size()-1].push_back(triangle);
            }
        }

        root = dcel->faces[1]->triangle;
    }

    Triangle* localize(point_2 p)
    {
        if(! root->contains(p))
            return lauers.back()[0];
        else
        {
            Triangle* u = root;
            while(u->successors.size())
            {
                for(int i=0; i<u->successors.size(); ++i)
                {
                    if(u->successors[i]->contains(p))
                    {
                        u=u->successors[i];
                        break;
                    }
                }
            }
            return u;
        }
    }

};
