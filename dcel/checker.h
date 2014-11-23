#ifndef CHECKER_H
#define CHECKER_H

#include "dcel.h"
#include<iostream>

using namespace std;

struct Checker{

    static bool traverseFace(Dcel d, int f)
    {
        int start_edge = d.faces[f].startEdge;
        int cur_edge = d.edges[start_edge].next;

        int k = 0;
        int max = 10000;

        vector<bool> used(d.edges.size(), false);

        while(cur_edge != start_edge)
        {
            used[cur_edge] = true;
            int next_edge = d.edges[cur_edge].next;

            if(used[next_edge])
            {
                std::cerr<<"edge "<<next_edge<<" has been visited twice in face "<<f<<endl;
                return false;
            }

            if(next_edge<0 || next_edge >= d.edges.size())
            {
                std::cerr<<"edge "<<cur_edge<<" is invalid and has "<<next_edge<<" as it's next"<<endl;
                return false;
            }

            if(k>=max)
            {
                std::cerr<<"face "<<f<<"has more than "<<max<<"edges "<<endl;
                return false;
            }
            cur_edge = next_edge;
            ++k;
        }
        return true;
    }

    static bool checkConsistensy(Dcel d)
    {
        for(int i=0; i<d.faces.size(); ++i)
        {
            if( !traverseFace(d, i) )
            {
                std::cerr<<"face "<<i<<" is incorrect"<<endl;
                return false;
            }
        }
        return true;
    }
};

#endif // CHECKER_H
