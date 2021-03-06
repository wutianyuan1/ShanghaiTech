#include "objects.hpp"

// Base object
// ===========================================
BaseObject::BaseObject() : m_position(0, 0)
{}

BaseObject::~BaseObject() 
{}

Position BaseObject::getPosition()
{
    return Position(m_position.x, m_position.y);
}

std::vector<Position> BaseObject::getPixelPositions()
{
    std::cout << "Oops, you are trying to getPixelPositions a BaseObject\n";
    return std::vector<Position>();
}

void BaseObject::setPosition(const Position& pos)
{
    m_position.x = pos.x;
    m_position.y = pos.y;
}

void BaseObject::setPosition(const int x, const int y)
{
    m_position.x = x;
    m_position.y = y;
}

// Circle
// ===========================================
Circle::Circle(int radius, int x, int y) : m_radius(radius)
{
    m_position.x = x;
    m_position.y = y;
}

Circle::~Circle()
{}

std::vector<Position> Circle::getPixelPositions()
{
    std::vector<Position> result;
    for (int i = -m_radius; i < m_radius; i++)
        for (int j = -m_radius; j < m_radius; j++)
            if (i * i + j * j < m_radius * m_radius)
                result.push_back(Position(i + m_position.x, j + m_position.y));
    return result;
}

// Rectangle
// ===========================================
Rectangle::Rectangle(int h, int w, int x, int y) : m_h(h), m_w(w)
{
    m_position.x = x;
    m_position.y = y;  
}

Rectangle::~Rectangle()
{}

std::vector<Position> Rectangle::getPixelPositions()
{
    std::vector<Position> result;
    for (int i = 0; i < m_w; i++)
        for (int j = 0; j < m_h; j++)
            result.push_back(Position(i + m_position.x, j + m_position.y));
    return result;    
}

// Let the object fall down
// ===========================================
void fallDown(BaseObject* obj, int currTimeStamp)
{
    Position pos = obj->getPosition();
    obj->setPosition(Position(pos.x, pos.y + G * (currTimeStamp + 0.5)) );
    pos = obj->getPosition();
}