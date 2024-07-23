#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include "manipulator.h"
#include <QDebug>
#include <QVector>
#include <math.h>
#include <QTimer>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector<Manipulator::Point> points;
    Manipulator *M1, *M2;

private slots:

    void on_pushButton_LoadPoints_clicked();

    void pathBuilding();

    void addToTable(Manipulator manipulator, int row, int column);

    void setManipulators();

    void coordsChanged();

    void setSpinBoxesEnability(bool state);

    void xyChangedSlot();

signals:
    void xyChanged();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
