#pragma once

#include <vector>
using std::vector;

#include "triangle.h"
#include "linked_triangle_dcel.h"
#include <map>
using std::map;

struct Kirkpatrick
{
    vector<LinkedTriangleDcel*> levels;

    vector<vector<Triangle*> >  lauers;
    Triangle* root;

    int countPower(Vertex* v)
    {
        int counter = 0;
        Edge* cur = v->incidentEdge;
        do
        {
            ++counter;
            cur = cur->twin->next;
        }while(cur!=v->incidentEdge);
        return counter;
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
        levels.push_back(dcel);

        lauers.push_back({}); // the 0-th lauer created

        for(int i=1; i<dcel->faces.size(); ++i)      // add all triangles to lauer 0
        {
            Triangle* triangle = dcel->faces[i]->triangle;
            lauers[0].push_back(triangle);
        }


        //dcel = new LinkedTriangleDcel(*dcel); // make a copy

        // the main part: removing vertices and construction
        while(true)
        {  
            dcel = new LinkedTriangleDcel(*dcel); // make a copy
            levels.push_back(dcel);

            if(dcel->vertices.size()==3)
                break;

            map<Vertex*, bool> used;
            vector<Vertex*> victims;

            for(int i=3; i<dcel->vertices.size(); ++i)  // find victims
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

            for(int i=0; i<victims.size(); ++i)   // delete victims one-by-one and link appropriate triangles
            {
                // old triangles arount the victim
                vector<Triangle*> oldTriangles = getAllTriangles(victims[i]);

                // delete the victim
                Face* bigFace = dcel->deleteVertex(victims[i]);

                // get new triangles after triangulation
                vector<Triangle*> newTriangles = dcel->triangulateFace(bigFace);


                // intersect old and new triangles
                for(int j=0; j<newTriangles.size(); ++j)
                {
                    for(int k=0; k<oldTriangles.size(); ++k)
                    {
                        if(oldTriangles[k]->intersects(newTriangles[j]))
                            newTriangles[j]->successors.push_back(oldTriangles[k]);
                    }
                }
            }

            // push triangles to the next level
            lauers.push_back({});

            // traverse all faces and add triangles, wich already have successors-links
            for(int i=1; i<dcel->faces.size(); ++i)
            {
                Triangle* triangle = dcel->faces[i]->triangle;
                lauers[lauers.size()-1].push_back(triangle);
            }
        }

        //dcel->triangulateDcel();
        root = dcel->faces[1]->triangle;
    }

    Face* localize(point_2 p)
    {
        if(! root->contains(p))
            return levels[0]->outer_face;
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
            return u->e->incidentFace;
        }
    }

    LinkedTriangleDcel* getLevel(int l)
    {
        if(l >= levels.size() || l < 0)    // extra defence
            return NULL;
        else
            return levels[l];
    }

    int getMaxLevel()
    {
        return levels.size()-1;
    }

};
