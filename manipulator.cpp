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

double Manipulator::square(double x)
{
    return x * x;
}

QVector<double> Manipulator::calcDistances(QVector<Manipulator::Point> points)
{
    double man_x = getX();
    double man_y = getY();

    QVector<double> distances;

    for (int i = 0; i < points.size(); i++)
    {
       double distance = sqrt(square(man_x - points[i].x) + square(man_y - points[i].y));
       distances.append(distance);
    }

    return distances;
}

Manipulator::distance_with_position Manipulator::getClosestPoint(QVector<double> distances)
{
    distance_with_position closestPoint;
    closestPoint.distance = distances[0];
    closestPoint.position = 0;

    for (int i = 1; i < distances.size(); ++i) {
        if (distances[i] < closestPoint.distance)
        {
            closestPoint.distance = distances[i];
            closestPoint.position = i;
        }
    }
    return closestPoint;
}
