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

    //TODO: не всегда работает правильно.
    // проверка плохая.
    // нужно как-то понять, куда направлено ребро,
    // и правильно проассоциировать прямую. тогда будет работать
    bool intersects(int edge, int line)
    {
        Line l = lines[line];
        Line edge_line = lines[edges[edge].line];

        if(edges[edge].origin == 0)
        {
            Vertex v = vertices[edges[edges[edge].twin].origin];
            point_2 p = l.intersect(edge_line);
            p.x += l.getDirection().x;
            p.y += l.getDirection().y;
            if((l.substitute(v.getPoint())) * (l.substitute(p)) > 0)
                return true;
            else
                return false;
        }

        if(edges[edges[edge].twin].origin == 0)
        {
            Vertex v = vertices[edges[edge].origin];
            point_2 p = l.intersect(edge_line);
            p.x += l.getDirection().x;
            p.y += l.getDirection().y;
            if((l.substitute(v.getPoint())) * (l.substitute(p)) > 0)
                return false;
            else
                return true;
        }


        Vertex v1 = vertices[edges[edge].origin];
        Vertex v2 = vertices[edges[edges[edge].twin].origin];

        Line l11 = lines[v1.line1];
        Line l12 = lines[v1.line2];
        Line l21 = lines[v2.line1];
        Line l22 = lines[v2.line2];

        if( opposite(orientation(Point3d(l), Point3d(l11), Point3d(l12)),
                    orientation(Point3d(l), Point3d(l21), Point3d(l22))) )
            return true;
        else
            return false;
    }

    int getNearest(int l)
    {
        // traverse all the edges incidet v0 and choose the nearest
        // we should take the blue one! with a little bigger polar angle

        // less, less, greater, greater
        vector<int> less;
        vector<int> greater;

        int cur_edge = vertices[0].incidentEdge;
        int start_edge = cur_edge;
        do
        {
            if(intersects(cur_edge, l))
            {
                return cur_edge;
            }

            if(lines[edges[cur_edge].line] < lines[l])
            {
                if(less.size() == 0)
                    less.push_back(cur_edge);
                else if(less.size() >= 1)
                {
                    if(lines[edges[cur_edge].line] == lines[edges[less.front()].line])
                        less.push_back(cur_edge);
                    else if(lines[edges[cur_edge].line] > lines[edges[less.front()].line])
                        less[0] = cur_edge;
                }
                else if(less.size() == 2)
                    if(lines[edges[cur_edge].line] > lines[edges[less.back()].line])
                        less[1] = cur_edge;
            }

            if(lines[edges[cur_edge].line] > lines[l])
            {
                if(greater.size() == 0)
                    greater.push_back(cur_edge);
                else if(greater.size() >= 1)
                {
                    if(lines[edges[cur_edge].line] == lines[edges[greater.front()].line])
                        greater.push_back(cur_edge);
                    else if(lines[edges[cur_edge].line] < lines[edges[greater.front()].line])
                        greater[0] = cur_edge;
                }
                else if(greater.size() == 2)
                    if(lines[edges[cur_edge].line] < lines[edges[greater.back()].line])
                        greater[1] = cur_edge;
            }


            cur_edge = edges[edges[cur_edge].twin].next;
        }while(start_edge != cur_edge);

        for(int i=0; i<greater.size(); ++i)
        {
            int cur = greater[i];
            do
            {
                if(intersects(cur, l))
                {
                    return greater[i];
                    break;
                }
                cur = edges[cur].next;
            } while(cur != greater[i]);
        }

        for(int i=0; i<less.size(); ++i)
        {
            int cur = less[i];
            do
            {
                if(intersects(cur, l))
                {
                    return less[i];
                    break;
                }
                cur = edges[cur].next;
            } while(cur != less[i]);
        }

        return -1; // unreachable. but it isn't prooved yet. hope so.
                    // if reachable - cases segfault;
    }



    void link_right(int e1, int e2, int begin, int end)
    {
        edges[e1].origin = edges[begin].origin;
        edges[begin].prev = e2;
        edges[e2].next = begin;
        edges[e1].prev = end;
        edges[end].next = e1;
    }


    void link_left(int e1, int e2, vector<int> &div)
    {
        edges[e2].origin = div[4];
        edges[div[0]].next = e2;
        edges[e2].prev = div[0];
        edges[div[2]].prev = e1;
        edges[e1].next = div[2];
    }

    void link_left(int e1, int e2, int ne1, int ne3, int v)
    {
        edges[e2].origin = v;

        edges[ne1].next = e2;
        edges[e2].prev = ne1;
        edges[ne3].prev = e1;
        edges[e1].next = ne3;
    }

    bool addLine(double a, double b, double c)
    {
        int l1 = lines.size();
        lines.push_back(Line(a,b,c));
        int l2 = lines.size();
        lines.push_back(Line(a,b,c, -1));

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
            int e1 = 0;
            int e2 = 1;
            vector<int> div = subdivide(e1, e2, l1);
            int e3=div[2];
            int e4=div[3];
            int v = div[4];


            int ne1 = edges.size();
            edges.push_back(Edge(v, l1));
            int ne2 = edges.size();
            edges.push_back(Edge(v, l2));
            int ne3 = edges.size();
            edges.push_back(Edge(0, l2));
            int ne4 = edges.size();
            edges.push_back(Edge(0, l1));

            edges[ne1].twin = ne3;
            edges[ne3].twin = ne1;
            edges[ne2].twin = ne4;
            edges[ne4].twin = ne2;

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
            int begin, end, in;

            begin = getNearest(l1);
            end = edges[begin].prev;
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
                // TODO: every third edge is in here!!

                int e1 = edges.size();
                edges.push_back(Edge(0, l1));
                int e2 = edges.size();
                edges.push_back(Edge(-1, l2));

                vector<int> div = subdivide(in, edges[in].twin, l1); // почему-то косячат фейсы
                link_left(e1, e2, div[0], div[2], div[4]);

                if(in == end)
                {
                    edges[e2].next = begin;
                    edges[begin].prev = e2;
                    edges[div[2]].next = e1;
                    edges[e1].prev = div[2];
                }
                if(in == begin)
                {
                    edges[e2].next = begin;
                    edges[begin].prev = e2;
                    edges[end].next = e1;
                    edges[e1].prev = end;
                }

                begin = div[3];
                end = div[1];
            }
//            else
//            {// best case: begin, ..., in, ...end;}


            while(true)
            {
                in = -1;
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

                if(in == -1)
                    break;

                int e1 = edges.size();
                edges.push_back(Edge(0, l1));
                int e2 = edges.size();
                edges.push_back(Edge(-1, l2));

                link_right(e1, e2, begin, end);
                // remember: e2.origin is still unknown
                // remember: new faces are correctly created

                vector<int> div = subdivide(in, edges[in].twin, l1);
                link_left(e1, e2, div);

                begin = div[3];
                end = div[1];
            }

            //TODO: проверить, корректно ли привязались крайние концы и консистентный ли dcel.

        }

        return false; // !!!! impossible
    }
};
