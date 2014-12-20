#pragma once

#include <vector>
using std::vector;

#include "triangle.h"

struct Node
{
    Triangle* triangle;
    vector<Node*> successors;
}
