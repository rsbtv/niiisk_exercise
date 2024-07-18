#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "manipulator.h"
#include <QDebug>
#include <QVector>
#include <math.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    struct Point
    {
        double x, y;
    };

    struct Distance_with_position
    {
        double distance;
        int position;
    };

    QVector<Point> points;
    Manipulator manipulator1, manipulator2;


private slots:
    Distance_with_position closestPoint(QVector<double> distances);

    double square(double x);

    QVector<double> calcDistances(Manipulator manipulator, QVector<Point> points);

    void on_pushButton_SaveManupulators_clicked();

    void on_pushButton_LoadPoints_clicked();

    void pathBuilding();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
