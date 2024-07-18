#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <QVector>
#include <math.h>


class Manipulator
{
public:
    Manipulator();

    struct Point
    {
        double x, y;
    };

    struct distance_with_position {
        double distance;
        int position;
    };

    double getX();
    double getY();
    double getRadius();
    
    void setX(double x);
    void setY(double y);
    void setRadius(double radius);

    double square(double x);

    QVector<double> calcDistances(QVector<Point> points);

    QVector<Point> points;

    distance_with_position getClosestPoint(QVector<double> distances);

    bool isInRange(double distance);

    QVector<double> goToPoint(int position);

private:
    double x, y, radius;
};

#endif // MANIPULATOR_H
