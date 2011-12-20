#ifndef ASTARNODE_H
#define ASTARNODE_H

#include <list>
#include "Utils.h"

const int node_stride = 50;

class AStarNode
{
protected:
    int x;
    int y;

    float g;
    float h;

    Point parent;

public:
    AStarNode();
    AStarNode(const int a, const int b);
    AStarNode(const Point p);
    AStarNode(const AStarNode& copy);

    int getX() const;
    int getY() const;

    Point getCoordinate() const;

    Point getParent() const;
    void setParent(const Point& p);

    std::list<Point> getNeighbours(int limitX=0, int limitY=0) const;

    float getActualCost() const;
    void setActualCost(const float G);

    float getEstimatedCost() const;
    void setEstimatedCost(const float H);

    float getFinalCost() const;

    bool operator<(const AStarNode& n) const;
    bool operator==(const AStarNode& n) const;
    bool operator==(const Point& p) const;
    bool operator!=(const Point& p) const;

};

#endif // ASTARNODE_H
