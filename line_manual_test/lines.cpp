#include <vector>
#include "line.h"
#include <iostream>
#include "orientation.h"

using namespace std;
using cg::leftTurn;

vector<Line> lines;

int getNearest(Line* l)
{
	for(int i=0; i<lines.size()-1; ++i)
	{
		if(leftTurn(lines[i]->n, l ,lines[i+1]->n))
		{
			return i;
		}
	}
	if(leftTurn(l, lines[i]->n, lines[i+1]->n))
		return -1;
	else if(leftTurn(lines[i]->n ,lines[i+1]->n), l)
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
		cin>>a>>b>>c;
		Line* l = new Line(a,b,c);
		
		if(lines.size < 2)
		{
			cout<<"line added"<<endl;
			continue;
		}
		int pos = getNearest(l);
		
		if(pos == -2)
		{
			cout<<"PANIC!!";
			return 0;
		}
		
		if(pos == -1)
			cout<<"the smallest poler angle. needs to be first";
		else if(pos == lines.size()-1)
			cout<<"the biggest poler angle. needs to be the last";
		else
			cout<<"between"<<lines[i]<<" and "<<lines[i+1];
		
		lines.push_back(l);
		sort(lines.begin(), lines.end());
	}
	
	return 0;
}