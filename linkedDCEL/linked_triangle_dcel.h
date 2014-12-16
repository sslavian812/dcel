#pragma once

#include<vector>
#include<list>
#include<map>

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

struct LinkedTriangleDcel : Dcel
{
    vector<Line*> lines;
    vector<Edge*> edges;
    vector<Vertex*> vertices;
    vector<Face*> faces;
    Face* outer_face;

    Edge* start_pool[4];

    LinkedTriangleDcel(Line l1, Line l2, Line l3)
    {
        Face* inner_face = new Face();
        outer_face = new Face();

        faces.push_back(outer_face);
        faces.push_back(inner_face);

        Line* line1 = new Line(l1);
        Line* line1r = new Line(-l1.a, -l1.b, -l1.c);
        Line* line2 = new Line(l2);
        Line* line2r = new Line(-l2.a, -l2.b, -l2.c);
        Line* line3 = new Line(l3);
        Line* line3r = new Line(-l3.a, -l3.b, -l3.c);
        lines.push_back(line1);
        lines.push_back(line1r);
        lines.push_back(line2);
        lines.push_back(line2r);
        lines.push_back(line3);
        lines.push_back(line3r);

        Vertex* v1 = new Vertex(line1, line3);
        Vertex* v2 = new Vertex(line1, line2);
        Vertex* v3 = new Vertex(line2, line3);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);


        Edge* e1 = new Edge(v2, line1);
        Edge* e2 = new Edge(v1, line1r);
        e1->twin = e2;
        e2->twin = e1;

        Edge* e3 = new Edge(v3, line2);
        Edge* e4 = new Edge(v2, line2r);
        e3->twin = e4;
        e4->twin = e3;

        Edge* e5 = new Edge(v1, line3);
        Edge* e6 = new Edge(v3, line3r);
        e5->twin = e6;
        e6->twin = e5;

        e1->next = e5;
        e5->next = e3;
        e3->next = e1;

        e1->prev = e3;
        e3->prev = e5;
        e5->prev = e1;

        e2->next = e4;
        e4->next = e6;
        e6->next = e2;

        e2->prev = e6;
        e6->prev = e4;
        e4->prev = e2;

        inner_face->startEdge = e2;
        outer_face->startEdge = e1;

        e1->incidentFace = outer_face;
        e5->incidentFace = outer_face;
        e3->incidentFace = outer_face;

        e2->incidentFace = inner_face;
        e4->incidentFace = inner_face;
        e6->incidentFace = inner_face;

        v1->incidentEdge = e5;
        v2->incidentEdge = e1;
        v3->incidentEdge = e3;

        edges.push_back(e1);
        edges.push_back(e2);
        edges.push_back(e3);
        edges.push_back(e4);
        edges.push_back(e5);
        edges.push_back(e6);

        start_pool[0] = e1; // 0
        start_pool[1] = e5; // 180
        start_pool[2] = e3; // 315
        start_pool[3] = e1; // 0
    }

    ~LinkedTriangleDcel()
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

    void localize(point_2f p, vector<pair<point_2, point_2> > &v) const
    {
        Edge* cur;

        for(int i=1; i<faces.size(); ++i)
        {
            bool flag = true;
            cur = faces[i]->startEdge;
            do
            {
                if(cg::orientation(point_2(0,0),
                                   cur->line->getDirection(),
                                   point_2(p.x-cur->origin->getPoint().x, p.y-cur->origin->getPoint().y)) != cg::CG_LEFT)
                {
                    flag = false;
                    break;
                }
                cur = cur->next;
            }while(cur != faces[i]->startEdge);

            if(flag)
            {
                cur = faces[i]->startEdge;
                do
                {
                    v.push_back(cur->getCoords(NULL));
                    cur = cur->next;
                }while(cur != faces[i]->startEdge);
                return;
            }
        }

        cur = faces[0]->startEdge;
        do
        {
            v.push_back(cur->getCoords(NULL));
            cur = cur->next;
        }while(cur != faces[0]->startEdge);
        return;
    }

    void getAllToDraw(std::vector<point_2> &res_vertices, vector<pair<point_2, point_2> > &res_edges) const
    {
        for(int i=0; i<faces.size(); ++i)
        {
            Edge* start_edge = faces[i]->startEdge;
            Edge* cur_edge = start_edge;
            do
            {
                res_edges.push_back(cur_edge->getCoords(NULL));
                cur_edge = cur_edge->next;

            }while(cur_edge != start_edge);
         }

        for(int i=0; i<vertices.size(); ++i)
        {
            res_vertices.push_back(vertices[i]->getPoint());
        }
    }

    void getBounds(Line &l1, Line &l2, Line &l3)
    {}

    //-----------------------------------------------------------------------------------------
    //usils:

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

        e1->next->prev = e3;
        e2->next->prev = e4;

        e4->next = e2->next;
        e3->next = e1->next;

        e1->next=e3;
        e3->prev=e1;
        e2->next=e4;
        e4->prev=e2;

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

        Vertex* v1 = edge->origin;
        Vertex* v2 = edge->twin->origin;

        Line* v1l1 = v1->line1;
        Line* v1l2 = v1->line2;

        Line* v2l1 = v2->line1;
        Line* v2l2 = v2->line2;

        int turn_to_point_1, turn_to_point_2;
        int det1, det2;

        turn_to_point_1 = (int) orientation(Point3d(*line), Point3d(*v1l1), Point3d(*v1l2));
        det1 = (int)cg::orientation(point_2(0,0), point_2(v1l1->a, v1l1->b), point_2(v1l2->a, v1l2->b));


        turn_to_point_2 = (int) orientation(Point3d(*line), Point3d(*v2l1), Point3d(*v2l2));
        det2 = (int)cg::orientation(point_2(0,0), point_2(v2l1->a, v2l1->b), point_2(v2l2->a, v2l2->b));

        return turn_to_point_1 * det1 != turn_to_point_2 * det2;
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

    Edge* getStartEdge(Line* line)
    {
        /* итерируюсь по ребрам (читай ориентированным прямым)
         * внешней грани. Ищу такое ребро, что
         * векторное произведение нашей прямой на него - вектор вглубь,
         * а векторное произведение нашей прямой на следующий вектор - вектор на нас.
         * С одного из этих векторов и можно смело начинать,
         * но лучше со "следующего вектора"*/

        const point_2 Z(0.0,0.0);

        Edge* cur_edge;
        Edge* next_edge;

        for(int i=0; i<3; ++i)
        {
            Edge* cur_edge = start_pool[i];
            Edge* next_edge = start_pool[i+1];

            if(cg::orientation(Z, line->getDirection(), cur_edge->line->getDirection())  == cg::CG_RIGHT &&
                               cg::orientation(Z, line->getDirection(), next_edge->line->getDirection()) == cg::CG_LEFT)
            {
                return next_edge;
            }
        }
        return NULL; // sometimes reachable, but I don't know why
    }



    // :utils
    //-----------------------------------------------------------------
    // interface


    bool addLine(double a, double b, double c)
    {
        Line* line1 = new Line( a, b, c);
        Line* line2 = new Line(-a,-b,-c);
        lines.push_back(line1);
        lines.push_back(line2);

        Edge* begin;
        Edge* end;
        Edge* in;
        Vertex* start_vertex;

        begin = getStartEdge(line1);
        end = begin->prev;

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

        Edge* e1=in;
        Edge* e2=in->twin;
        Edge* e3;
        Edge* e4;
        Vertex* v;

        //subdivide(e1, e2, line1, e3, e4, v);
        halfSubdivide(e1, e2, line1, e3, e4, v, outer_face, outer_face);
        v->isOnBorder = true;

        begin = e4;
        end = e2;
        start_vertex = v;

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

            if(end->incidentFace == outer_face)
            {
                delete begin->incidentFace;
                faces.pop_back();
                begin->incidentFace = outer_face;
                v->isOnBorder = true;
                break;
            }
        }

        checkConsistensy();
        return true;
    }


    // :interface
    //----------------------------------------------------------------
    // checkers:

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

            if(f!=0 && cg::orientation(point_2(0,0),
                               cur_edge->line->getDirection(),
                               next_edge->line->getDirection()) != cg::CG_LEFT)
            {
                cerr<<"edge "<<cur_edge<<" int face "<<f<<" has incorrect TURN!!"<<endl;
                res = false;
            }


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


    // :checkers

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

