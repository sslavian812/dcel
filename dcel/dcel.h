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
        vertices.push_back(Vertex(-1, -1, 0.0, 0.0)); // infinite vertex
        faces.push_back(Face());   // infinite face
    }

    ~Dcel()
    {}

    // visualization
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

        int e1_victim = edges[e1].next;
        int e2_victim = edges[e2].next;

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

        edges[e1_victim].prev = e3;
        edges[e2_victim].prev = e4;

        vector<int> res = {e1,e2,e3,e4,v};
        return res;
    }

    void subdivide(int e1, int e2, int l, int &e3, int &e4, int &v)
    {
        vector<int> div = subdivide(e1, e2, l);
        e3=div[2];
        e4=div[3];
        v = div[4];

        faces.push_back(Face(e4));
        edges[e4].incidentFace = faces.size()-1;

        faces.push_back(Face(e3));
        edges[e3].incidentFace = faces.size()-1;
    }


    void halfSubdivide(int e1, int e2, int l, int &e3, int &e4, int &v, int f1, int f3)
    {
        vector<int> div = subdivide(e1, e2, l);
        e3=div[2];
        e4=div[3];
        v = div[4];

        //subdivide(e1, e2,l, e3,e4, v);

        int victim = edges[e1].incidentFace;
        faces[victim].startEdge = edges[e1].next;

        edges[e1].incidentFace = f1;
        edges[e3].incidentFace = f3;

        faces.push_back(Face(e4));
        edges[e4].incidentFace = faces.size()-1;

    }

    bool intersects(int edge, int l)
    {
        // determines, does the line intersect the edge or not.
        if(edges[edge].origin == 0)
            return intersects(edges[edge].twin, l);

        // origin != 0 now.

        Line line = lines[l];
        Line edge_line = lines[edges[edge].line];
        Vertex v = vertices[edges[edge].origin];

        int twin = edges[edge].twin;

        Line vl1 = lines[v.line1];
        Line vl2 = lines[v.line2];


        bool flag1 = false, flag2 = false;


        orientation_t first, second;
        first = orientation(Point3d(line), Point3d(vl1), Point3d(vl2));
        if(cg::orientation(point_2(0,0), point_2(vl1.a, vl1.b), point_2(vl2.a, vl2.b)) == cg::CG_LEFT)
            flag1=true;

        if(edges[twin].origin == 0)
        {
            if(cg::orientation(point_2(0.0,0.0), line.n, edge_line.n) == cg::CG_RIGHT)
                second = orientation_t::CG_RIGHT;
            else if(cg::orientation(point_2(0.0,0.0), line.n, edge_line.n) == cg::CG_LEFT)
                second = orientation_t::CG_LEFT;
            else
                second = orientation_t::CG_COLLINEAR;
        }
        else
        {
            v = vertices[edges[twin].origin];

            vl1 = lines[v.line1];
            vl2 = lines[v.line2];

            second = orientation(Point3d(line), Point3d(vl1), Point3d(vl2));
            if(cg::orientation(point_2(0,0), point_2(vl1.a, vl1.b), point_2(vl2.a, vl2.b)) == cg::CG_LEFT)
                flag2=true;
        }

        return (first!=second && flag1==flag2) || (first==second && flag1!=flag2);
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


    void linkForward(int e1, int e2, int prev, int next, int v)
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


    void linkBackward(int e1, int e2, int prev, int next, int v)
    {
        // links "right" side of e1|e2 between prev-next edges
        linkForward(e2, e1, next, prev, v); // enough to swap e1 and e2, according to images!!
    }


    void updateFace(int edge, int twin)
    {
        int new_face = edges[edge].incidentFace;
        int cur_edge = edges[edge].next;
        int curface;

        while(cur_edge != edge)
        {
            curface = edges[cur_edge].incidentFace;
            if(faces[curface].startEdge == cur_edge && curface != new_face)
            {
               faces[curface].startEdge = twin;
            }
            edges[cur_edge].incidentFace = new_face;
            cur_edge = edges[cur_edge].next;
        }

        //traverseFace(*this, edges[edge].incidentFace);
    }


    //-----------------------------------------------------------------

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

            linkForward(new_e1, new_e2, e1, e3, v);

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

            linkBackward(new_e1, new_e2, e4, e2, v);

            edges[e4].next = new_e2;
            edges[new_e2].prev = e4;
            edges[new_e1].next = e2;
            edges[e2].prev = new_e1;
            //closure

            checkConsistensy(*this);

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


            int new_e1 = edges.size();
            edges.push_back(Edge(start_vertex, l1));
            int new_e2 = edges.size();
            edges.push_back(Edge(-1, l2));
            edges[new_e1].twin = new_e2;
            edges[new_e2].twin = new_e1;

            if(in == begin || in == end)
            {
                int e1=in, e2 = edges[in].twin, e3,e4,v;

                subdivide(e1, e2, l1, e3, e4, v);

                linkForward(new_e1, new_e2, e1, e3, v);

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

                updateFace(new_e1, new_e2);
                updateFace(new_e2, new_e1);

                begin = e4;
                end = e2;
                start_vertex = v;
            }
            else
            {
                // case: begin, ..., in, ...end;

                linkBackward(new_e1, new_e2, begin, end, start_vertex);

                int e1 = in, e2=edges[in].twin, e3, e4, v;
                subdivide(e1,e2, l1, e3,e4,v);
                linkForward(new_e1, new_e2, e1, e3, v);

                updateFace(new_e1, new_e2);
                updateFace(new_e2, new_e1);

                begin = e4;
                end = e2;
                start_vertex = v;
            }


            while(true)
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


                linkBackward(new_e1, new_e2, begin, end, start_vertex);

                int e1 = in, e2=edges[in].twin, e3, e4, v;
                halfSubdivide(e1, e2, l1, e3, e4, v, edges[begin].incidentFace, edges[end].incidentFace);
                linkForward(new_e1, new_e2, e1, e3, v);

                //TODO: check wether e1 e3 needed

                updateFace(new_e1, new_e2);
                updateFace(new_e2, new_e1);

                begin = e4;
                end = e2;
                start_vertex = v;
            }


            new_e1 = edges.size();
            edges.push_back(Edge(start_vertex, l1));
            new_e2 = edges.size();
            edges.push_back(Edge(0, l2));

            edges[new_e1].twin = new_e2;
            edges[new_e2].twin = new_e1;

            linkBackward(new_e1, new_e2, begin, end, start_vertex);


            cur = begin;
            while(edges[cur].origin != 0)
                cur = edges[cur].next;
            int prev_cur = edges[cur].prev;

            edges[new_e1].next = cur;
            edges[cur].prev = new_e1;

            edges[prev_cur].next = new_e2;
            edges[new_e2].prev = prev_cur;

            updateFace(new_e1, new_e2);
            updateFace(new_e2, new_e1);

            checkConsistensy(*this);
            return true;
        }

        return false; // !!!! impossible
    }

    static bool traverseFace(Dcel d, int f)
    {
        int start_edge = d.faces[f].startEdge;
        int cur_edge = d.edges[start_edge].next;

        int k = 0;
        int max = 10000;

        vector<bool> used(d.edges.size(), false);

        bool res = true;

        while(cur_edge != start_edge)
        {
            used[cur_edge] = true;
            int next_edge = d.edges[cur_edge].next;

            if(d.edges[cur_edge].incidentFace != f)
            {
                std::cerr<<"edge "<<cur_edge<<
                           " has incorrect associated face: "<< d.edges[cur_edge].incidentFace<<
                           " insted of " <<f<<std::endl;
                res = false;
            }

            if(used[next_edge])
            {
                std::cerr<<"edge "<<next_edge<<" has been visited twice in face "<<f<<std::endl;
                res = false;
            }

            if(next_edge<0 || next_edge >= d.edges.size())
            {
                std::cerr<<"edge "<<cur_edge<<" is invalid and has "<<next_edge<<" as it's next"<<std::endl;
                return false;
            }

            if(k>=max)
            {
                std::cerr<<"face "<<f<<"has more than "<<max<<"edges "<<std::endl;
                return false;
            }

            if(d.edges[next_edge].prev != cur_edge)
            {
                std::cerr<<"edge "<<next_edge<<" has incorrect prev pointer "<<d.edges[next_edge].prev<<" instead of "<<cur_edge<<std::endl;
                return false;
            }
            cur_edge = next_edge;
            ++k;
        }
        if(!res)
            std::cerr<<"ended checking face "<<f<<": "<<std::endl<<std::endl;
        return res;
    }

    static bool checkConsistensy(Dcel d)
    {
        bool res = true;
        for(int i=0; i<d.faces.size(); ++i)
        {
            if( !traverseFace(d, i) )
            {
                std::cerr<<"face "<<i<<" is incorrect"<<std::endl;
                res = false;
            }
        }
        if(res)
            std::cerr<<"dcel is CORRECT!"<<std::endl;
        else
            std::cerr<<"dcel is INCORRECT!"<<std::endl;
        return res;
    }
};

