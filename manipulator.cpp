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

double Manipulator::square(double num)
{
    return num * num;
}

QVector<double> Manipulator::getDistances(QVector<Manipulator::Point> points)
{
    QVector<double> distances;

    for (int i = 0; i < points.size(); i++)
    {
       double distance = sqrt(square(getX() - points[i].x) + square(getY() - points[i].y));
       distances.append(distance);
    }

    return distances;
}

int Manipulator::getClosestPoint(QVector<double> distances)
{
    double lowestDistance;
    int closestPointPosition;

    lowestDistance = distances[0];
    closestPointPosition = 0;

    for (int i = 1; i < distances.size(); ++i)
    {
        if (distances[i] < lowestDistance)
        {
            closestPointPosition = i;
        }
    }

    return closestPointPosition;
}

