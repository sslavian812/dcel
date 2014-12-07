#pragma once

#include<vector>
#include<list>
#include<map>
#include<cmath>

#include "line.h"
#include "orientation.h"
#include "orientation3d.h"

#include "vertex.h"
#include "edge.h"
#include "face.h"
#include "dcel.h"

#include "cg/primitives/point.h"

using std::vector;
using std::list;
using std::pair;
using std::cerr;
using std::endl;
using std::map;

using std::max;
using std::min;

struct LinkedInfiniteDcel : Dcel
{
    vector<Line*> lines;
    vector<Edge*> edges;
    vector<Vertex*> vertices;
    vector<Face*> faces;
    Vertex* infinite_vertex;

    LinkedInfiniteDcel()
    {
        infinite_vertex = new Vertex(NULL, NULL, 0.0, 0.0);
        vertices.push_back(infinite_vertex);
        faces.push_back(new Face());   // infinite face
    }

    ~LinkedInfiniteDcel()
    {
        for(int i=0; i<lines.size(); ++i)
        {delete lines[i];}

        for(int i=0; i<edges.size(); ++i)
        {delete edges[i];}

        for(int i=0; i<vertices.size(); ++i)
        {delete vertices[i];}

        for(int i=0; i<faces.size(); ++i)
        {delete faces[i];}
    }


    void getAllToDraw(std::vector<point_2> &res_vertices, vector<pair<point_2, point_2> > &res_edges) const
    {
        if(faces.size()==1)
            return;
        if(faces.size()==2)
        {
             res_edges.push_back(edges[0]->getCoords(infinite_vertex));
             res_edges.push_back(edges[1]->getCoords(infinite_vertex));

             return;
        }

        for(int i=0; i<faces.size(); ++i)
        {
            Edge* start_edge = faces[i]->startEdge;
            Edge* cur_edge = start_edge;

            //pair<point_2, point_2> edge;
            do
            {
                //edge = cur_edge->getCoords(infinite_vertex);
                res_edges.push_back(cur_edge->getCoords(infinite_vertex));
                cur_edge = cur_edge->next;

            }while(cur_edge != start_edge);
         }

        for(int i=1; i<vertices.size(); ++i)
        {
            res_vertices.push_back(vertices[i]->getPoint());
        }
    }

    void getBounds(Line &l1, Line &l2, Line &l3)
    {
        double min_x = vertices[1]->getPoint().x;
        double max_x = vertices[1]->getPoint().x;
        double min_y = vertices[1]->getPoint().y;
        double max_y = vertices[1]->getPoint().y;
        for(int i=2; i<vertices.size(); ++i)
        {
            min_x = min(min_x, vertices[i]->getPoint().x);
            max_x = max(max_x, vertices[i]->getPoint().x);
            min_y = min(min_y, vertices[i]->getPoint().y);
            max_y = max(max_y, vertices[i]->getPoint().y);
        }

        max_x = 2*max_x + 10;
        max_y = 2*max_y + 10;
        min_y -= 10;
        min_x -= 10;

        l1 = Line(point_2(min_x, min_y), point_2(min_x, max_y));
        l2 = Line(point_2(max_x, min_y), point_2(min_x, min_y));
        l3 = Line(point_2(min_x, max_y), point_2(max_x, min_y));
    }


    vector<void*> subdivide(Edge* e1, Edge* e2, Line* line)
    {
        // returns [e1,e2,e3,e4,v]
        // e1 and e2 needs to be twins-half-edges of edge to be subdivided

        Edge* e1_victim = e1->next;
        Edge* e2_victim = e2->next;

        Line* other_line1 = e1->line;
        Line* other_line2 = e2->line;

        Vertex* v = new Vertex(line, other_line1, line->intersect(*other_line1));
        vertices.push_back(v);

        Edge* e3 = new Edge(v, other_line1);
        edges.push_back(e3);
        Edge* e4 = new Edge(v, other_line2);
        edges.push_back(e4);

        v->incidentEdge = e3;

        e1->twin = e4;
        e4->twin = e1;
        e2->twin = e3;
        e3->twin = e2;

        e4->next = e2->next;
        e3->next = e1->next;

        e1->next=e3;
        e3->prev=e1;
        e2->next=e4;
        e4->prev=e2;

        // remember: some next&prev pointers are invalid

        e1_victim->prev = e3;
        e2_victim->prev = e4;

        vector<void*> res = {e1,e2,e3,e4,v};
        return res;
    }

    void subdivide(Edge* e1, Edge* e2, Line* l, Edge* &e3, Edge* &e4, Vertex* &v)
    {
        vector<void*> div = subdivide(e1, e2, l);
        e3 = reinterpret_cast<Edge*>(div[2]);
        e4 = reinterpret_cast<Edge*>(div[3]);
        v = reinterpret_cast<Vertex*>(div[4]);

        Face* face= new Face(e4);
        faces.push_back(face);
        e4->incidentFace = face;

        face = new Face(e3);
        faces.push_back(face);
        e3->incidentFace = face;
    }

    void halfSubdivide(Edge* e1, Edge* e2, Line* l, Edge* &e3, Edge* &e4, Vertex* &v, Face* f1, Face* f3)
    {
        vector<void*> div = subdivide(e1, e2, l);
        e3 = reinterpret_cast<Edge*>(div[2]);
        e4 = reinterpret_cast<Edge*>(div[3]);
        v = reinterpret_cast<Vertex*>(div[4]);

        //subdivide(e1, e2,l, e3,e4, v);

        Face* victim = e1->incidentFace;
        victim->startEdge = e1->next;

        e1->incidentFace = f1;
        e3->incidentFace = f3;

        Face* face = new Face(e4);
        faces.push_back(face);
        e4->incidentFace = face;

    }

    bool intersects(Edge* edge, Line* line)
    {
        // determines, does the line intersect the edge or not.


        if(edge->origin == infinite_vertex)
            return intersects(edge->twin, line);

        // origin != INFINITE_VERTEX now.

        Line* edge_line = edge->line;
        Vertex* v = edge->origin;

        Edge* twin = edge->twin;

        Line* vl1 = v->line1;
        Line* vl2 = v->line2;


        bool flag1 = false, flag2 = false;


        int turn_to_point_1, turn_to_point_2;
        int turn_to_edge;
        int det1, det2;

        turn_to_point_1 = (int) orientation(Point3d(*line), Point3d(*vl1), Point3d(*vl2));
        det1 = (int)cg::orientation(point_2(0,0), point_2(vl1->a, vl1->b), point_2(vl2->a, vl2->b));

        if(twin->origin == infinite_vertex) // half-infinite edge
        {
           turn_to_edge = (int)cg::orientation(point_2(0.0,0.0), line->n, edge_line->n);

           return turn_to_point_1 * det1 != turn_to_edge;
        }
        else   // ordinary edge
        {
            v = twin->origin;
            vl1 = v->line1;
            vl2 = v->line2;

            turn_to_point_2 = (int) orientation(Point3d(*line), Point3d(*vl1), Point3d(*vl2));
            det2 = (int)cg::orientation(point_2(0,0), point_2(vl1->a, vl1->b), point_2(vl2->a, vl2->b));

            return turn_to_point_1 * det1 != turn_to_point_2 * det2;
        }
    }

    Edge* getNearest(Line* line)
    {
        /* итерируюсь по ребрам (читай ориентированным прямым)
         * выходящим из бесконечной точки. Ищу такое ребро, что
         * векторное произведение нашей прямой на него - вектор вглубь,
         * а векторное произведение нашей прямой на следующий вектор - вектор на нас.
         * С одного из этих векторов и можно смело начинать*/

        Edge* cur_edge = infinite_vertex->incidentEdge;
        Edge* start_edge = cur_edge;
        Edge* next_edge = cur_edge->twin->next;

        const point_2 Z(0.0,0.0);

        while(next_edge != start_edge)
        {
            if(cg::orientation(Z, line->getDirection(), cur_edge->line->getDirection())  == cg::CG_RIGHT &&
                               cg::orientation(Z, line->getDirection(), next_edge->line->getDirection()) == cg::CG_LEFT)
            {
                return next_edge;
            }

            cur_edge = next_edge;
            next_edge = cur_edge->twin->next;
        }

        if(cg::orientation(Z, line->getDirection(), cur_edge->line->getDirection())  == cg::CG_RIGHT &&
                           cg::orientation(Z, line->getDirection(), next_edge->line->getDirection()) == cg::CG_LEFT)
        {
            return next_edge;
        }

        return NULL; // sometimes reachable, but I don't know why
    }


    void linkForward(Edge* e1, Edge* e2, Edge* prev, Edge* next, Vertex* v)
    {
        // links "left" side of e1|e2 between prev-next edges
        e2->origin = v;

        prev->next = e2;
        e2->prev = prev;
        next->prev = e1;
        e1->next = next;

        e1->incidentFace = next->incidentFace;
        e2->incidentFace = prev->incidentFace;
    }


    void linkBackward(Edge* e1, Edge* e2, Edge* prev, Edge* next, Vertex* v)
    {
        // links "right" side of e1|e2 between prev-next edges
        linkForward(e2, e1, next, prev, v); // enough to swap e1 and e2, according to images!!
    }


    void updateFace(Edge* edge, Edge* twin)
    {
        Face* new_face = edge->incidentFace;
        Edge* cur_edge = edge->next;
        Face* curface;

        while(cur_edge != edge)
        {
            curface = cur_edge->incidentFace;
            if(curface->startEdge == cur_edge && curface != new_face)
            {
               curface->startEdge = twin;
            }
            cur_edge->incidentFace = new_face;
            cur_edge = cur_edge->next;
        }
    }


    //-----------------------------------------------------------------


    void addFirst(Line* line1, Line* line2)
    {
        Edge* e1 = new Edge(infinite_vertex, line1);
        edges.push_back(e1);
        Edge* e2 = new Edge(infinite_vertex, line2);
        edges.push_back(e2);

        e1->twin = e2;
        e2->twin = e1;

        infinite_vertex->incidentEdge = e1;

        e1->next = e1;
        e1->prev = e1;
        e2->next = e2;
        e2->prev = e2;

        e1->incidentFace = faces[0];
        faces[0]->startEdge = e1;

        Face* f2 = new Face(e2);
        faces.push_back(f2);

        e2->incidentFace = f2;
    }

    void addSecond(Line* line1, Line* line2)
    {
        /*
         * векторное произведение нашей прямой на него - вектор вглубь,
         * а векторное произведение нашей прямой на следующий вектор - вектор на нас.
        */
        Edge* e1 = edges[0];
        Edge* e2 = edges[1];
        Edge* e3;
        Edge* e4;
        Vertex* v;

        const point_2 Z(0.0, 0.0);

        if(cg::orientation(Z, line1->getDirection(), e1->line->getDirection()) == cg::CG_RIGHT)
        {
            std::swap(e1, e2);
        }

        subdivide(e1, e2, line1, e3, e4, v);

        Edge* new_e1 = new Edge(infinite_vertex, line1);
        edges.push_back(new_e1);
        Edge* new_e2 = new Edge(v, line2);
        edges.push_back(new_e2);

        new_e1->twin = new_e2;
        new_e2->twin = new_e1;


        linkForward(new_e1, new_e2, e1, e3, v);

        new_e2->next = e1;
        e1->prev = new_e2;
        e3->next = new_e1;
        new_e1->prev = e3;
        // closure

        new_e1 = new Edge(v, line1);
        edges.push_back(new_e1);
        new_e2 = new Edge(infinite_vertex, line2);
        edges.push_back(new_e2);

        new_e1->twin = new_e2;
        new_e2->twin = new_e1;

        linkBackward(new_e1, new_e2, e4, e2, v);

        e4->next = new_e2;
        new_e2->prev = e4;
        new_e1->next = e2;
        e2->prev = new_e1;
        //closure
    }

    bool addLine(double a, double b, double c)
    {
        Line* line1 = new Line( a, b, c);
        Line* line2 = new Line(-a,-b,-c);
        lines.push_back(line1);
        lines.push_back(line2);

        if(faces.size() == 1)
            addFirst(line1, line2);
        else if(faces.size() == 2)
            addSecond(line1, line2);
        else
        {
            Edge* begin;
            Edge* end;
            Edge* in;
            Vertex* start_vertex = infinite_vertex;

            begin = getNearest(line1);
            end = begin->prev;

            if(begin->origin != infinite_vertex)
                std::swap(begin, end);

            Edge* cur = begin;
            do
            {
                if(intersects(cur, line1))
                {
                    in = cur;
                    break;
                }
                cur = cur->next;
            } while(cur != begin);


            Edge* new_e1 = new Edge(start_vertex, line1);
            edges.push_back(new_e1);
            Edge* new_e2 = new Edge(NULL, line2);
            edges.push_back(new_e2);

            new_e1->twin = new_e2;
            new_e2->twin = new_e1;

            if(in == begin || in == end)
            {
                Edge* e1=in;
                Edge* e2=in->twin;
                Edge* e3;
                Edge* e4;
                Vertex* v;

                subdivide(e1, e2, line1, e3, e4, v);

                linkForward(new_e1, new_e2, e1, e3, v);

                if(in == end)
                {
                    new_e2->next = begin;
                    begin->prev = new_e2;
                    e3->next = new_e1;
                    new_e1->prev = e3;
                }
                if(in == begin)
                {
                    new_e2->next = begin;
                    begin->prev = new_e2;
                    end->next = new_e1;
                    new_e1->prev = end;
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

                Edge* e1 = in;
                Edge* e2=in->twin;
                Edge* e3;
                Edge* e4;
                Vertex* v;
                subdivide(e1,e2, line1, e3,e4,v);
                linkForward(new_e1, new_e2, e1, e3, v);

                updateFace(new_e1, new_e2);
                updateFace(new_e2, new_e1);

                begin = e4;
                end = e2;
                start_vertex = v;
            }


            while(true)
            {
                in = NULL;
                Edge* cur = begin->next;
                do
                {
                    if(intersects(cur, line1))
                    {
                        in = cur;
                        break;
                    }
                    cur = cur->next;
                } while(cur != end);

                if(in == NULL)
                    break;

                Edge* new_e1 = new Edge(start_vertex, line1);
                edges.push_back(new_e1);
                Edge* new_e2 = new Edge(NULL, line2);
                edges.push_back(new_e2);

                new_e1->twin = new_e2;
                new_e2->twin = new_e1;


                linkBackward(new_e1, new_e2, begin, end, start_vertex);

                Edge* e1 = in;
                Edge* e2=in->twin;
                Edge* e3;
                Edge* e4;
                Vertex* v;
                halfSubdivide(e1, e2, line1, e3, e4, v, begin->incidentFace, end->incidentFace);
                linkForward(new_e1, new_e2, e1, e3, v);

                //TODO: check wether e1 e3 needed

                updateFace(new_e1, new_e2);
                updateFace(new_e2, new_e1);

                begin = e4;
                end = e2;
                start_vertex = v;
            }


            new_e1 = new Edge(start_vertex, line1);
            edges.push_back(new_e1);
            new_e2 = new Edge(infinite_vertex, line2);
            edges.push_back(new_e2);

            new_e1->twin = new_e2;
            new_e2->twin = new_e1;

            linkBackward(new_e1, new_e2, begin, end, start_vertex);


            cur = begin;
            while(cur->origin != infinite_vertex)
                cur = cur->next;
            Edge* prev_cur = cur->prev;

            new_e1->next = cur;
            cur->prev = new_e1;

            prev_cur->next = new_e2;
            new_e2->prev = prev_cur;

            updateFace(new_e1, new_e2);
            updateFace(new_e2, new_e1);

            checkConsistensy();
            return true;
        }

    }

    bool traverseFace(int f)
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


    bool checkConsistensy()
    {
        bool res = true;
        for(int i=0; i<faces.size(); ++i)
        {
            if( !traverseFace(i) )
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

//    list<int> cunstructBorder(int face)
//    {
//        int cur_edge = faces[face].startEdge;
//        int start_edge = cur_edge;
//        list<int> l;
//        do
//        {
//            l.insert(l.end(),edges[cur_edge].origin);
//            cur_edge = edges[cur_edge].next;
//        }while(cur_edge != start_edge);
//        return l;
//    }

//    void triangulate(int face)
//    {
//        list<int> dcvl = constructBorder(face);
//        list<int>::iterator it=dcvl.begin();
//        list<int>::iterator it2;
//        list<int>::iterator it3;
//        while(dcvl.size() > 3)
//        {
//            it2 = it;
//            int v1,v2,v3;
//            v1 = *it2; ++it2;
//            if(it2 == dcvl.end()) it2=dcvl.begin();
//            it3=it2;
//            v2 = *it3; ++it3;
//            if(it3 == dcvl.end()) it3=dcvl.begin();
//            v3 = *it3;

//            if(leftTurn(v1,v2,v3))
//            {
//                if(isEar(dcvl, v1,v2,v3))
//                {
//                    curEar(dcvl, it1, it2, it3);
//                }
//            }
//        }


//    }
};

