#pragma once

#include "dcel.h"

struct triangulator
{
    static int countVertices(dcel &d, int face)
    {
        int vertices = 1;
        int start_edge = d.faces[face].startEdge;
        int cur_edge = d.edges[start_edge].next;

        while(cur_edge != start_edge)
        {
            ++vertices;
            cur_edge = d.edges[cur_edge].next;
        }
        return vertices;
    }

    static int findStaertVertex(dcel &d, int face)
    {
        int start_edge = d.faces[face].startEdge;
        int cur_edge = d.edges[start_edge].next;
        int vertex = edges[cur_edge].origin;

        while(cur_edge != start_edge && d.edges[cur_edge].origin != 0)
        {
            cur_edge = d.edges[cur_edge].next;
        }
        start_edge = edges[cur_edge].next;
        return start_edge;
    }

    static void convexTriangulation(dcel &d, int face)
    {
        int vertices = countVertices(d, face);
        if(vertices <=3)
            return;

        int start_edge = findStartEdge(d, face);
        int start_vertex = edges[start_edge].origin;
        int cur_edge = d.edges[start_edge].next;
        cur_edge = d.edges[cur_edge].next;

        while(d.edges[cur_edge].next != start_edge)
        {
           // d.addEdge(face, start_vertex, d.edges[cur_edge].origin); // TODO
            cur_edge = d.edges[cur_edge].next;
        }
    }

    //----------------------------------------------------------------------------------


    static list<int> cunstructBorder(dcel d, int face)
    {
        int cur_edge = faces[face].startEdge;
        int start_edge = cur_edge;
        list<int> l;
        do
        {
            l.insert(l.end(),edges[cur_edge].origin);
            cur_edge = edges[cur_edge].next;
        }while(cur_edge != start_edge);
        return l;
    }

    typedef list<int>::iterator li;

    static void cutEar(list<int> &dcvl, li it1, li it2, li it3)
    {
        // TODO
    }

    static void EarTriangulation(dcel d, int face)
    {
        list<int> dcvl = constructBorder(face);
        list<int>::iterator it=dcvl.begin();
        list<int>::iterator it2;
        list<int>::iterator it3;
        while(dcvl.size() > 3)
        {
            it2 = it;
            int v1,v2,v3;
            v1 = *it2; ++it2;
            if(it2 == dcvl.end()) it2=dcvl.begin();
            it3=it2;
            v2 = *it3; ++it3;
            if(it3 == dcvl.end()) it3=dcvl.begin();
            v3 = *it3;

//            if(leftTurn(v1,v2,v3))
//            {
//                if(isEar(dcvl, v1,v2,v3))
//                {
//                    cutEar(dcvl, it1, it2, it3);
//                }
//            }
        }
    }
};
