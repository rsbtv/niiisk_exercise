#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_SaveManupulators_clicked()
{
    // Вносим параметры манипуляторов
    manipulator1.setX(ui->doubleSpinBox_X->value());
    manipulator1.setY(ui->doubleSpinBox_X->value());
    manipulator1.setRadius(ui->doubleSpinBox_Radius->value());
    
    manipulator2.setX(ui->doubleSpinBox_X_2->value());
    manipulator2.setY(ui->doubleSpinBox_Y_2->value());
    manipulator2.setRadius(ui->doubleSpinBox_Radius_2->value());
    
    // блокируем изменение параметров
    ui->doubleSpinBox_X->setEnabled(false);
    ui->doubleSpinBox_X_2->setEnabled(false);
    ui->doubleSpinBox_Y->setEnabled(false);
    ui->doubleSpinBox_Y_2->setEnabled(false);
    ui->doubleSpinBox_Radius->setEnabled(false);
    ui->doubleSpinBox_Radius_2->setEnabled(false);
    ui->pushButton_SaveManupulators->setEnabled(false);
}

void MainWindow::on_pushButton_LoadPoints_clicked()
{
    bool isDouble;
    QString fileName = QFileDialog::getOpenFileName(this,"Открыть файл","","Text files (*.txt)");
    if (fileName.isEmpty())
    {
        QMessageBox::information(this,"Ошибка","Файл не выбран",QMessageBox::Ok);
    }
    else
    {
        QFile file;
        file.setFileName(fileName);
        file.open(QIODevice::ReadOnly);

        while (!file.atEnd())
        {
            QString line = file.readLine();
            QStringList parts = line.split(" ");
            if (parts.size() == 2)
            {
                Manipulator::Point point;
                point.x = parts[0].toDouble(&isDouble);
                if (isDouble)
                {
                    point.y = parts[1].toDouble(&isDouble);
                    if (isDouble)
                    {
                        points.push_back(point);
                    }
                    else {
                        QMessageBox::warning(this, "Ошибка!", "Неверный формат", QMessageBox::Ok);
                        points.clear();
                        break;
                    }
                }
                else
                {
                    QMessageBox::warning(this, "Ошибка!", "Неверный формат", QMessageBox::Ok);
                    points.clear();
                    break;
                }
            }
        }

        file.close();

        for (int i = 0; i < points.size(); i++)
            qDebug() << points[i].x << points[i].y;
    }

    qDebug() << manipulator1.calcDistances(points);
    qDebug() << manipulator1.getClosestPoint(manipulator1.calcDistances(points)).distance;

//    qDebug() << getClosestPoint(manipulator1.calcDistances(points)).distance << getClosestPoint(manipulator1.calcDistances(points)).position;
//    qDebug() << calcDistances(manipulator1, points);
//    qDebug() << calcDistances(manipulator2, points);
}

//double MainWindow::square(double x)
//{
//    return x * x;
//}

//QVector<double> MainWindow::calcDistances(Manipulator manipulator, QVector<Point> points)
//{
//    double man_x = manipulator.getX();
//    double man_y = manipulator.getY();

//    QVector<double> distances;

//    for (int i = 0; i < points.size(); i++)
//    {
//       double distance = sqrt(square(man_x - points[i].x) + square(man_y - points[i].y));
//       distances.append(distance);
//    }

//    return distances;
//}

//MainWindow::distance_with_position MainWindow::getClosestPoint(QVector<double> distances)
//{
//    distance_with_position closestPoint;
//    closestPoint.distance = distances[0];
//    closestPoint.position = 0;

//    for (int i = 1; i < distances.size(); ++i) {
//        if (distances[i] < closestPoint.distance)
//        {
//            closestPoint.distance = distances[i];
//            closestPoint.position = i;
//        }
//    }
//    return closestPoint;
//}
