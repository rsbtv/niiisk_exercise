#ifndef MANIPULATOR_H
#define MANIPULATOR_H


class Manipulator
{
public:
    explicit Manipulator();

    double getX();
    double getY();
    double getRadius();
    
    void setX(double x);
    void setY(double y);
    void setRadius(double radius);

private:
    double x, y, radius;
};

#endif // MANIPULATOR_H
