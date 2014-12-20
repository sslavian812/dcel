#pragma once

#include <vector>
using std::vector;

#include "triangle.h"
#include "linked_triangle_dcel.h"

struct Kirkpatrick
{
    vector<LinkedTriangleDcel*> lauers_d;
    vector<vector<Triangle*> >  lauers_t;
    Node* root;

    Kirkpatrick(LinkedTriangleDcel* initial)
    {
        LinkedTriangleDcel* dcel = new LinkedTriangleDcel(initial);
        dcel->triangulateDcel();
        lauers_d.push_back(dcel);
        lauers_t.push_back({});
        for(int i=1; i<dcel->faces.size(); ++i)
        {
            vector<Vertex*> vertices = dcel->faces[i]->getVertices();
            Edge* edge = dcel->faces[i]->getStrongEdge();
            Triangle* t = new Triangle(vertices, edge);
            lauers_t.push_back(t);
        }

        // now the main part: removing vertices and construction
    }

};
