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

#include <QModbusTcpClient>
#include <QModbusDataUnit>
#include <QTimer>
#include <qcustomplot.h>

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
    QModbusTcpClient *client;
    QCPCurve *curve1, *curve2;
    QTimer *timer;
    long long animationDuration, animationStartTime;

    void readData();
    void updateGraph();


private slots:

    void onStateChanged(int state);

    void onErrorOccurred(QModbusDevice::Error error);

    void on_pushButton_LoadPoints_clicked();

    void pathBuilding();

    void addToTable(Manipulator manipulator, int row, int column);

    void setManipulators();

    void coordsChanged();

    void setSpinBoxesEnability(bool state);

    void sendData(Manipulator::Point point);

//    void xyChangedSlot();

    void on_pushButton_Reset_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
