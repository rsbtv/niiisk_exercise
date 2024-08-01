#include "manipulator.h"

Manipulator::Manipulator()
{

}

double Manipulator::getX()
{
    return x;
}

double Manipulator::getY()
{
    return y;
}

double Manipulator::getR()
{
    return radius;
}

void Manipulator::setX(double x)
{
   this->x = x;
}

void Manipulator::setY(double y)
{
    this->y = y;
}

void Manipulator::setXY(double x, double y)
{
    this->x = x;
    this->y = y;
}

void Manipulator::setR(double radius)
{
    this->radius = radius;
}

Manipulator::Point Manipulator::getXY()
{
    Point point;
    point.x = getX();
    point.y = getY();
    return point;
}

double Manipulator::square(double num)
{
    return num * num;
}

double Manipulator::getDistance(Point point)
{
    return (sqrt(square(getX() - point.x) + square(getY() - point.y)));
}

