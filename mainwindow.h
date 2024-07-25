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

    void readData();

private slots:

    void onStateChanged(int state);

    void onErrorOccurred(QModbusDevice::Error error);

    void on_pushButton_LoadPoints_clicked();

    void pathBuilding();

    void addToTable(Manipulator manipulator, int row, int column);

    void setManipulators();

    void coordsChanged();

    void setSpinBoxesEnability(bool state);

    void sendAndGetData(Manipulator::Point point);

//    void xyChangedSlot();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
