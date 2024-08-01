#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <QVector>
#include <math.h>


class Manipulator
{

public:
    explicit Manipulator();

    double getX();
    double getY();
    double getR();

    
    void setX(double x);
    void setY(double y);
    void setXY(double x, double y);
    void setR(double radius);


    struct Point{
        double x, y;
    };

    Point getXY();
    double square(double num);
    double getDistance(Point point);
    QVector<Point> reachedPoints;

private:
    double x, y, radius;
};

#endif // MANIPULATOR_H
