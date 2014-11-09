#pragma once

#include<cmath>
#include<cg/primitives/point.h>
#include<cg/primitives/vector.h>
#include<string>

using std::string;

using cg::point_2;
using cg::point_2f;
using cg::vector_2f;

struct Line
{
	double a,b,c;
	point_2 n;

    Line(point_2f p, vector_2f vct)
    {
        double x0 = p.x;
        double y0 = p.y;
        double u = vct.x;
        double v = vct.y;
        double x1 = -v+x0;
        double y1 = u+y0;

        this->a=double(y0-y1);
        this->b=double(x1-x0);
        this->c=double(x0*y1-y0*x1);

        double r = sqrt(a*a+b*b);
        n = point_2(a/r, b/r);  // normalized normal vector
    }
	
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

    std::pair<point_2f, point_2f> getSegment()
    {
        double x0,x1,y0,y1;
        if(b!=0)
        {
            x0 =50000; x1 = -50000;
            y0 = (-c-a*x0)/b;
            y1 = (-c-a*x1)/b;
        }
        else
        {
            x0 = x1 = -c/a;
            y0 = -50000;
            y1 = 50000;
        }
        return std::make_pair(point_2f((float)x0,(float)y0), point_2f((float)x1,(float)y1));
    }

    bool operator<(const Line &other) const
    {
        return atan2(n.y, n.x) < atan2(other.n.y, other.n.x);
    }

    point_2 intersect(Line other)
    {
        double a2 = other.a;
        double b2 = other.b;
        double c2 = other.c;
        double x = -(c*b2-c2*b)/(a*b2-a2*b);
        double y = -(a*c2-a2*c)/(a*b2-a2*b);
        return point_2(x,y);
    }


	string toString()
	{
		string s="<";
		s+= std::to_string(a);
		s+= ",";
		s+= std::to_string(b);
		s+= ",";
		s+= std::to_string(c);
		s+= ">";

		return s;
	}
};
