#pragma once

#include<vector>

#include "line.h"
#include "orientation.h"

#include "vertex.h"
#include "edge.h"
#include "face.h"

#include "cg/primitives/point.h"

using std::vector;

struct Dcel
{
    vector<Line> lines;
    vector<Edge> edges;
    vector<Vertex> vertices;
    vector<Face> faces;


	Dcel()
    {
        vertices.push_back(Vertex(-1, -1, (double)0/0,(double)0/0)); // infinite vertex
        faces.push_back(Face());   // infinite face
    }

    ~Dcel()
    {}

    std::pair<point_2, point_2> getEdge(int edge) const
    {
        int v1, v2;
        v1 = edges[edge].origin;
        v2 = edges[edges[edge].twin].origin;
        Line l = lines[edges[edge].line];
        std::pair<point_2, point_2> res = l.getSegment();
        if(v1 != 0)
            res.first = vertices[v1].getPoint();
        if(v2 != 0)
            res.second = vertices[v2].getPoint();
        return res;
    }

    bool addLine(double a, double b, double c)
    {
        lines.push_back(Line(a,b,c));
        return addLine();
    }

    vector<int> subdivide(int e1, int e2, int l)
    {
        // returns [e1,e2,e3,e4,v]
        // e1 and e2 needs to be twins-half-edges of edge to be subdivided
        int other_line = edges[e1].line;

        int v = vertices.size();
        vertices.push_back(Vertex(l, other_line, lines[l].intersect(lines[other_line])));
        int e3 = edges.size();
        edges.push_back(Edge(v, other_line));
        int e4 = edges.size();
        edges.push_back(Edge(v, other_line));
        vertices[v].incidentEdge = e3;

        edges[e1].twin = e4;
        edges[e4].twin = e1;
        edges[e2].twin = e3;
        edges[e3].twin = e2;

        edges[e1].next=e3;
        edges[e3].prev=e1;
        edges[e2].next=e4;
        edges[e4].prev=e2;

        // remember: some next&prev pointers are invalid

        faces.push_back(Face(e3));
        edges[e3].incidentFace = faces.size()-1;
        faces.push_back(Face(e4));
        edges[e4].incidentFace = faces.size()-1;

        vector<int> res = {e1,e2,e3,e4,v};
        return res;
    }


    bool addLine()
    {
        int l = lines.size()-1;

        if(faces.size() == 1)
        {
            int e1 = edges.size();
            edges.push_back(Edge(0, l));
            int e2 = edges.size();
            edges.push_back(Edge(0, l));

            edges[e1].next = e1;
            edges[e1].prev = e1;
            edges[e2].next = e2;
            edges[e2].prev = e2;
            edges[e1].twin = e2;
            edges[e2].twin = e1;

            edges[e1].incidentFace = 0;
            faces[0].startEdge = e1;
            int f2 = faces.size();
            faces.push_back(Face(e2));
            edges[e2].incidentFace = f2;
            return true;
        }
        else if(faces.size() == 2)
        {
            int e1 = 0;
            int e2 = 1;
            vector<int> div = subdivide(e1, e2, l);
            int e3=div[2];
            int e4=div[3];
            int v = div[4];


            int ne1 = edges.size();
            edges.push_back(Edge(v, l));
            int ne2 = edges.size();
            edges.push_back(Edge(v, l));
            int ne3 = edges.size();
            edges.push_back(Edge(0, l));
            int ne4 = edges.size();
            edges.push_back(Edge(0, l));

            edges[ne1].twin = ne3;
            edges[ne3].twin = ne1;
            edges[ne2].twin = ne4;
            edges[ne4].twin = ne3;

            edges[e1].next = ne1;
            edges[ne1].prev = e1;
            edges[e2].next = ne2;
            edges[ne2].prev = e2;

            edges[ne3].next = e3;
            edges[e3].prev = ne3;
            edges[ne4].next = e4;
            edges[e4].prev = ne4;

            edges[e3].next = ne3;
            edges[ne3].prev = e3;
            edges[e4].next = ne4;
            edges[ne4].prev = e4;

            edges[ne2].next = e2;
            edges[e2].prev = ne2;
            edges[ne1].next = e1;
            edges[e1].prev = ne1;

            edges[ne1].incidentFace = edges[e1].incidentFace;
            edges[ne2].incidentFace = edges[e2].incidentFace;
            edges[ne3].incidentFace = edges[e3].incidentFace;
            edges[ne4].incidentFace = edges[e4].incidentFace;
        }
        else
        {
            return false;
//            int startEdge = getNearest(0, line);
//            int curEdge = edges[startEdge].next;
//            while(curedge != startEdge)
//            {
//                if(line.intersects(edges[curEdge]))
//                {
//                    int v = vertices.size();
//                    vertices.push_back( Vertex(Intersection(l, edges[curEdge].line)) );

//                    int e1 = edges.size();
//                    edges.push_back(Edge(0,l));
//                    int e2 = edges.size();
//                    edges.push_back(Edge(v,l));
//                    edges[e1].twin = e2;
//                    edges[e2].twin = e1;

//                    int e3 = edges.size();
//                    edges.push_back(Edge(v, edges[curedge].line));
//                    int e4 = edges.size();
//                    edges.push_back(Edge(v, edges[curedge].line));
//                    int curtwin = edges[curedge].twin;
//                    edges[curtwin].twin = e4;
//                    edges[e4].twin = curtwin;
//                    edges[curedge].twin = e3;
//                    edges[e3].twin = curEdge;

//                    edges[e1].next = e4;
//                    edges[e4].prev = e1;

//                    if(edges[curedge].next == curedge)
//                    {
//                        edges[e4].next = e1;
//                        edges[e1].prev = e4;
//                    }
//                    else
//                    {
//                        edges[e4].next = edges[curEdge].next;
//                        edges[edges[curEdge].next].prev = e4;
//                    }

//                    edges[curedge].next=e2;
//                    edges[e2].prev=curedge;




//                    edges[]
//                    edges[curEdge].next=e2;
//                    edges[e2].prev=curEdge;


//                    // два ребра в бесконечность
//                    // разбить текущее и его твина
//                    // стартовая вершина - новая половинка твина
//                    // текущая - её next
//                    // если обошлт круг и не встрелити - конец
//                }
//            }

        }

        return false; // !!!! impossible
    }
};
