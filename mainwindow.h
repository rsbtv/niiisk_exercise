#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "manipulator.h"
#include <QDebug>
#include <QVector>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_SaveManupulators_clicked();

    void on_pushButton_LoadPoints_clicked(); // Пока предполагается что файл всегда правильный, но лучше добавить проверку

private:
    Ui::MainWindow *ui;
    Manipulator manipulator1, manipulator2;
    struct Point
    {
        double x, y;
    };
    QVector<Point> points;
};

#endif // MAINWINDOW_H
