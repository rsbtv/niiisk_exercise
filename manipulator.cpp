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

double Manipulator::getRadius()
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

void Manipulator::setRadius(double radius)
{
    this->radius = radius;
}

