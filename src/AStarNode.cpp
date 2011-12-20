#include "AStarNode.h"

AStarNode::AStarNode()
{
    this->x = 0;
    this->y = 0;
}

AStarNode::AStarNode(const int a, const int b)
{
    this->x = a;
    this->y = b;
}

AStarNode::AStarNode(const Point p)
{
    this->x = p.x;
    this->y = p.y;
}

AStarNode::AStarNode(const AStarNode& copy)
{
    x = copy.x;
    y = copy.y;
    g = copy.g;
    h = copy.h;
    parent = copy.parent;
}

int AStarNode::getX() const
{
    return x;
}

int AStarNode::getY() const
{
    return y;
}

Point AStarNode::getCoordinate() const
{
    Point coord = {x,y};
    return coord;
}

Point AStarNode::getParent() const
{
    return parent;
}

void AStarNode::setParent(const Point& p)
{
    this->parent = p;
}

std::list<Point> AStarNode::getNeighbours(int limitX, int limitY) const
{
    Point toAdd;
    std::list<Point> res;
    if(x>node_stride && y>node_stride)
    {
        toAdd.x = x-node_stride;
        toAdd.y = y-node_stride;
        res.push_back(toAdd);
    }
    if(x>node_stride && (limitY==0 || y<limitY-node_stride) )
    {
        toAdd.x = x-node_stride;
        toAdd.y = y+node_stride;
        res.push_back(toAdd);
    }
    if(y>node_stride && (limitX==0 || x<limitX-node_stride) )
    {
        toAdd.x = x+node_stride;
        toAdd.y = y-node_stride;
        res.push_back(toAdd);
    }
    if( (limitX==0 || x<limitX-node_stride) && (limitY==0 || y<limitY-node_stride) )
    {
        toAdd.x = x+node_stride;
        toAdd.y = y+node_stride;
        res.push_back(toAdd);
    }
    if(x>node_stride)
    {
        toAdd.x = x-node_stride;
        toAdd.y = y;
        res.push_back(toAdd);
    }
    if(y>node_stride)
    {
        toAdd.x = x;
        toAdd.y = y-node_stride;
        res.push_back(toAdd);
    }
    if(limitX==0 || x<limitX-node_stride )
    {
        toAdd.x = x+node_stride;
        toAdd.y = y;
        res.push_back(toAdd);
    }
    if(limitY==0 || y<limitY-node_stride )
    {
        toAdd.x = x;
        toAdd.y = y+node_stride;
        res.push_back(toAdd);
    }

    return res;
}


float AStarNode::getActualCost() const
{
    return g;
}

void AStarNode::setActualCost(const float G)
{
    //Q_ASSERT(G >= 0);
    g = G;
}

float AStarNode::getEstimatedCost() const
{
    return h;
}

void AStarNode::setEstimatedCost(const float H)
{
    //Q_ASSERT(H >= 0);
    h = H;
}

float AStarNode::getFinalCost() const
{
    return g+h;
}

bool AStarNode::operator<(const AStarNode& n) const
{
    return getFinalCost() < n.getFinalCost();
}

bool AStarNode::operator==(const AStarNode& n) const
{
    return x == n.x && y == n.y;
}

bool AStarNode::operator==(const Point& p) const
{
    return x == p.x && y == p.y;
}

bool AStarNode::operator!=(const Point& p) const
{
    return x != p.x || y != p.y;
}
