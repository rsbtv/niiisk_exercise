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
    QVector<double> reached_x, reached_y;

    struct Point{
        double x, y;
    };


    double square(double num);
    QVector<double> getDistances(QVector<Point> points);
    int getClosestPoint(QVector<double> distances);

//signals:
//    void xyChanged();

private:
    double x, y, radius;
};

#endif // MANIPULATOR_H
