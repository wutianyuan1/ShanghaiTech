#ifndef __OBJECTS_HPP
#define __OBJECTS_HPP

#include <iostream>
#include <utility>
#include <vector>
#include <GL/gl.h>

#define G -0.1 // Gravity

// Position
// ===========================================
struct Position
{
    Position() : x(0), y(0) {};
    Position(int x_, int y_) : x(x_), y(y_) {};
    int x, y;
};


// Base object
// ===========================================
class BaseObject
{
public:
    BaseObject();
    virtual ~BaseObject();
    virtual std::vector<Position> getPixelPositions();
    virtual Position getPosition();
    virtual void setPosition(const Position& pos);
    virtual void setPosition(const int x, const int y);

protected:
    Position m_position;
};


// Circle
// ===========================================
class Circle : public BaseObject
{
public:
    Circle(int radius, int x, int y);
    ~Circle();

    std::vector<Position> getPixelPositions();
private:
    int m_radius;
};


// Rectangle
// ===========================================
class Rectangle : public BaseObject
{
public:
    Rectangle(int h, int w, int x, int y);
    ~Rectangle();

    std::vector<Position> getPixelPositions();
private:
    int m_h, m_w;
};

void fallDown(BaseObject* obj, int currTimeStamp);

#endif // __OBJECTS_HPP