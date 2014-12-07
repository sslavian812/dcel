#pragma once

#include<math.h>
#include"line.h"

template <class T> struct Point3D;

typedef Point3D<double> Point3d;
typedef Point3D<float>  Point3f;
typedef Point3D<int>  Point3i;


template <class T>
struct Point3D
{
    T x, y, z;

    Point3D(T x, T y, T z)
     : x(x)
     , y(y)
     , z(z)
    {}


    Point3D(Point3D<T> const & other)
     : x(other.x)
     , y(other.y)
     , z(other.z)
    {}

    Point3D()
     : x(0)
     , y(0)
     , z(0)
    {}

    Point3D(Line l): x(l.a), y(l.b), z(l.c)
    {}
};

template <class T>
inline bool operator < (Point3D<T> const & a, Point3D<T> const & b)
{
 if (a.x == b.x)
     return (a.y == b.y) ? (a.z < b.z) : (a.y< b.y);
 else
    return a.x < b.x;
}

template <class T>
bool operator > (Point3D<T> const & a, Point3D<T> const & b)
{
 return b < a;
}

template <class T>
bool operator == (Point3D<T> const & a, Point3D<T> const & b)
{
 return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

template <class T>
bool operator <= (Point3D<T> const & a, Point3D<T> const & b)
{
 return !(a > b);
}

template <class T>
bool operator >= (Point3D<T> const & a, Point3D<T> const & b)
{
 return !(a < b);
}

template <class T>
bool operator != (Point3D<T> const & a, Point3D<T> const & b)
{
 return !(a == b);
}

template <class T>
Point3D<T>  operator-=(Point3D<T> & a, Point3D<T> const & b)
{
  a.x-=b.x;
  a.y-=b.y;
  a.z-=b.z;
  return a;
}

template <class T>
Point3D<T>  operator+=(Point3D<T> & a, Point3D<T> const & b)
{
  a.x+=b.x;
  a.y+=b.y;
  a.z+=b.z;
  return a;
}

template <class T>
Point3D<T>  operator+(Point3D<T> const & a, Point3D<T> const & b)
{
  Point3D<T>c = a;
  c+=b;
  return c;
}

template <class T>
Point3D<T>  operator-(Point3D<T> const & a, Point3D<T> const & b)
{
    Point3D<T>c = a;
    c-=b;
    return c;
}



// google styleguide:
// Type names should start with a capital letter and have a capital letter for each new word. No underscores.
// Variable names are all lowercase, with underscores between words.
