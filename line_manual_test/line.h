#pragma once

#include<cmath>
#include<point2d.h>

using cg::point_2;

struct Line
{
	double a,b,c;
	point_2 n;
	
	Line(double a, double b, double c)
	{
		// b > 0
		// b == 0 -> a>0
		if(b == 0)
		{
			if(a < 0)
			{
				a = -a; c = -c;
			}
		}
		else if(b < 0)
		{
			b = -b; a = -a; c = -c;
		}
		
		this->a=a;
		this->b=b;
		this->c=c;
		
		double r = sqrt(a*a+b*b);
		
		n = point_2(a/r, b/r);  // normalized normal vector
	}
	
	bool operator<(Line* other)
	{
		return atan2(a, b) < atan2(other->a, other->b);
	}
};