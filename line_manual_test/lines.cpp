#include <vector>
#include "line.h"
#include <iostream>
#include "orientation.h"

using namespace std;
using cg::leftTurn;

vector<Line> lines;

int getNearest(Line l)
{
	for(int i=0; i<lines.size()-1; ++i)
	{
        if(leftTurn(lines[i].n, l.n ,lines[i+1].n))
		{
			return i;
		}
	}
    if(l<lines[0])
		return -1;
    else if(lines.back()<l)
		return lines.size()-1;
	
	return -2; // PANIC!!! unreachable;
}

int main()
{
	int n;
	cin>>n;
	
	double a,b,c;
	while(n)
	{
        --n;
		cin>>a>>b>>c;
        Line l = Line(a,b,c);
		
		if(lines.size() < 2)
		{
			cout<<"line added"<<endl;
            lines.push_back(l);
            sort(lines.begin(), lines.end());
			continue;
		}
		int pos = getNearest(l);
		
		if(pos == -2)
		{
			cout<<"PANIC!!";
			return 0;
		}
		
		if(pos == -1)
            cout<<"the smallest poler angle. needs to be first"<<endl;
		else if(pos == lines.size()-1)
            cout<<"the biggest poler angle. needs to be the last"<<endl;
		else
            cout<<"between"<<lines[pos].toString()<<" and "<<lines[pos+1].toString()<<endl;
		
        lines.push_back(l);
		sort(lines.begin(), lines.end());

        cout<<"==========================="<<endl;
        for(int i=0; i<lines.size(); ++i)
        {
            cout<<lines[i].toString()<<endl;
        }
        cout<<"==========================="<<endl;

	}
	
	return 0;
}
