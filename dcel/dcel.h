#pragma once

#include<vector>

#include "line.h"
#include "orientation.h"
#include "orientation3d.h"

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


    vector<int> subdivide(int e1, int e2, int l)
    {
        // returns [e1,e2,e3,e4,v]
        // e1 and e2 needs to be twins-half-edges of edge to be subdivided
        int other_line1 = edges[e1].line;
        int other_line2 = edges[e2].line;

        int v = vertices.size();
        vertices.push_back(Vertex(l, other_line1, lines[l].intersect(lines[other_line1])));
        int e3 = edges.size();
        edges.push_back(Edge(v, other_line1));
        int e4 = edges.size();
        edges.push_back(Edge(v, other_line2));
        vertices[v].incidentEdge = e3;

        edges[e1].twin = e4;
        edges[e4].twin = e1;
        edges[e2].twin = e3;
        edges[e3].twin = e2;

        edges[e4].next = edges[e2].next;
        edges[e3].next = edges[e1].next;

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

    void subdivide(int e1, int e2, int l, int &e3, int &e4, int &v)
    {
        vector<int> div = subdivide(e1, e2, l);
        e3=div[2];
        e4=div[3];
        v = div[4];
    }

    bool intersects(int edge, int l)
    {
        // determines, does the line intersect the edge or not.

        if(edges[edge].origin == 0)
        {
            return intersects(edges[edge].twin, l);
        }

        // origin != 0 now.

        Line line = lines[l];
        Line edge_line = lines[edges[edge].line];

        return cg::orientation(point_2(0.0,0.0), edge_line.n, line.n) == cg::CG_RIGHT;
    }

    int getNearest(int l)
    {
        /* итерируюсь по ребрам (читай ориентированным прямым)
         * выходящим из бесконечной точки. Ищу такое ребро, что
         * векторное произведение нашей прямой на него - вектор вглубь,
         * а векторное произведение нашей прямой на следующий вектор - вектор на нас.
         * С одного из этих векторов и можно смело начинать*/


        int cur_edge = vertices[0].incidentEdge;
        int start_edge = cur_edge;
        int next_edge = edges[edges[cur_edge].twin].next;

        Line line = lines[l];
        const point_2 Z(0.0,0.0);

        while(next_edge != start_edge)
        {
            if(cg::orientation(Z, line.getDirection(), lines[edges[cur_edge].line].getDirection())  == cg::CG_RIGHT &&
                               cg::orientation(Z, line.getDirection(), lines[edges[next_edge].line].getDirection()) == cg::CG_LEFT)
            {
                return next_edge;
            }

            cur_edge = next_edge;
            next_edge = edges[edges[cur_edge].twin].next;
        }

        if(cg::orientation(Z, line.getDirection(), lines[edges[cur_edge].line].getDirection())  == cg::CG_RIGHT &&
                           cg::orientation(Z, line.getDirection(), lines[edges[next_edge].line].getDirection()) == cg::CG_LEFT)
        {
            return next_edge;
        }

        return -1; // unreachable.
    }


/*    // deprecated
//    void link_right(int e1, int e2, int begin, int end)
//    {
//        edges[e1].origin = edges[begin].origin;
//        edges[begin].prev = e2;
//        edges[e2].next = begin;
//        edges[e1].prev = end;
//        edges[end].next = e1;
//    }

//    //deprecated
//    void link_left(int e1, int e2, vector<int> &div)
//    {
//        edges[e2].origin = div[4];
//        edges[div[0]].next = e2;
//        edges[e2].prev = div[0];
//        edges[div[2]].prev = e1;
//        edges[e1].next = div[2];
//    }

//    //deprecated
//    void link_left(int e1, int e2, int ne1, int ne3, int v)
//    {
//        edges[e2].origin = v;

//        edges[ne1].next = e2;
//        edges[e2].prev = ne1;
//        edges[ne3].prev = e1;
//        edges[e1].next = ne3;
//    }
*/


    void link_forward(int e1, int e2, int prev, int next, int v)
    {
        // links "left" side of e1|e2 between prev-next edges
        edges[e2].origin = v;

        edges[prev].next = e2;
        edges[e2].prev = prev;
        edges[next].prev = e1;
        edges[e1].next = next;

        edges[e1].incidentFace = edges[next].incidentFace;
        edges[e2].incidentFace = edges[prev].incidentFace;
    }


    void link_backward(int e1, int e2, int prev, int next, int v)
    {
        // links "right" side of e1|e2 between prev-next edges
        link_forward(e2, e1, next, prev, v); // enough to swap e1 and e2, according to images!!
    }

    bool addLine(double a, double b, double c)
    {
        int l1 = lines.size();
        lines.push_back(Line(a, b, c));
        int l2 = lines.size();
        lines.push_back(Line(-a,-b,-c));

        if(faces.size() == 1)
        {
            int e1 = edges.size();
            edges.push_back(Edge(0, l1));
            int e2 = edges.size();
            edges.push_back(Edge(0, l2));

            vertices[0].incidentEdge = e1;

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
            /*
             * векторное произведение нашей прямой на него - вектор вглубь,
             * а векторное произведение нашей прямой на следующий вектор - вектор на нас.
            */
            int e1 = 0;
            int e2 = 1;
            int e3, e4, v;

            const point_2 Z(0.0, 0.0);

            if(cg::orientation(Z, lines[l1].getDirection(), lines[edges[e1].line].getDirection()) == cg::CG_RIGHT)
            {
                std::swap(e1,e2);
            }

            subdivide(e1, e2, l1, e3, e4, v);

            int new_e1 = edges.size();
            edges.push_back(Edge(0, l1));
            int new_e2 = edges.size();
            edges.push_back(Edge(v, l2));

            edges[new_e1].twin = new_e2;
            edges[new_e2].twin = new_e1;

            link_forward(new_e1, new_e2, e1, e3, v);

            edges[new_e2].next = e1;
            edges[e1].prev = new_e2;
            edges[e3].next = new_e1;
            edges[new_e1].prev = e3;
            // closure


            new_e1 = edges.size();
            edges.push_back(Edge(v, l1));
            new_e2 = edges.size();
            edges.push_back(Edge(0, l2));

            edges[new_e1].twin = new_e2;
            edges[new_e2].twin = new_e1;

            link_backward(new_e1, new_e2, e4, e2, v);

            edges[e4].next = new_e2;
            edges[new_e2].prev = e4;
            edges[new_e1].next = e2;
            edges[e2].prev = new_e1;
            //closure
            return true;
        }
        else
        {
            int begin, end, in;
            int start_vertex = 0;

            begin = getNearest(l1);
            end = edges[begin].prev;

            if(edges[begin].origin != 0)
                std::swap(begin, end);

            int cur = begin;
            do
            {
                if(intersects(cur, l1))
                {
                    in = cur;
                    break;
                }
                cur = edges[cur].next;
            } while(cur != begin);

            if(in == begin || in == end)
            {
                // undesired case; needs sone addition code

                int new_e1 = edges.size();
                edges.push_back(Edge(start_vertex, l1));
                int new_e2 = edges.size();
                edges.push_back(Edge(-1, l2));
                edges[new_e1].twin = new_e2;
                edges[new_e2].twin = new_e1;

                int e1=in, e2 = edges[in].twin, e3,e4,v;
                subdivide(e1, e2, l1, e3, e4, v);

                link_forward(new_e1, new_e2, e1, e3, v);

                if(in == end)
                {
                    edges[new_e2].next = begin;
                    edges[begin].prev = new_e2;
                    edges[e3].next = new_e1;
                    edges[new_e1].prev = e3;
                }
                if(in == begin)
                {
                    edges[new_e2].next = begin;
                    edges[begin].prev = new_e2;
                    edges[end].next = new_e1;
                    edges[new_e1].prev = end;
                }

                begin = e4;
                end = e2;
                start_vertex = v;
            }
//            else
//            {// best case: begin, ..., in, ...end;}


            while(in == in)
            {
                in = -1;
                int cur = edges[begin].next;
                do
                {
                    if(intersects(cur, l1))
                    {
                        in = cur;
                        break;
                    }
                    cur = edges[cur].next;
                } while(cur != end);

                if(in == -1)
                    break;

                int new_e1 = edges.size();
                edges.push_back(Edge(start_vertex, l1));
                int new_e2 = edges.size();
                edges.push_back(Edge(-1, l2));
                edges[new_e1].twin = new_e2;
                edges[new_e2].twin = new_e1;


                link_backward(new_e1, new_e2, begin, end, start_vertex);

                int e1 = in, e2=edges[in].twin, e3, e4, v;
                subdivide(e1,e2, l1, e3,e4,v);
                link_forward(new_e1, new_e2, e1, e3, v);

                begin = e4;
                end = e2;
                start_vertex = v;
            }


            int new_e1 = edges.size();
            edges.push_back(Edge(start_vertex, l1));
            int new_e2 = edges.size();
            edges.push_back(Edge(0, l2));

            edges[new_e1].twin = new_e2;
            edges[new_e2].twin = new_e1;

            link_backward(new_e1, new_e2, begin, end, start_vertex);


            cur = begin;
            while(edges[cur].origin != 0)
                cur = edges[cur].next;
            int prev_cur = edges[cur].prev;

            edges[new_e1].next = cur;
            edges[cur].prev = new_e1;

            edges[prev_cur].next = new_e2;
            edges[new_e2].prev = prev_cur;
            return true;
        }

        return false; // !!!! impossible
    }
};
