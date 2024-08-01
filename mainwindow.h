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
#include <QTimer>
#include <qcustomplot.h>
#include <QTcpSocket>

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
    void on_pushButton_LoadPoints_clicked();

    void pathBuilding();

    void setManipulators();

    void coordsChanged();

    void setSpinBoxesEnability(bool state);

    void on_pushButton_Reset_clicked();

    void updateGraph();

    void onConnected();

    void onReadyRead();

    void sendData(Manipulator::Point pointM1, Manipulator::Point pointM2);

    void addToTable(int column);

private:
    Ui::MainWindow *ui;

    QVector<Manipulator::Point> points;

    Manipulator *M1, *M2;

    QCPCurve *curve1, *curve2;

    QTimer *timer;

    QTcpSocket * socket;

    long long animationDuration, animationStartTime;
};

#endif // MAINWINDOW_H
