#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList verticalHeaderLabels;
    ui->tableWidget_Points->setVerticalHeaderLabels(verticalHeaderLabels << "М1" << "М2");


    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::coordMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}





void MainWindow::saveManipulators()
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
}

void MainWindow::on_pushButton_LoadPoints_clicked()
{


    bool isDouble;
    QString fileName = QFileDialog::getOpenFileName(this,"Открыть в формате TXT","C:\\Users\\student\\Documents","Text files (*.txt)");
    if (fileName.isEmpty())
    {
        QMessageBox::information(this,"Ошибка","Файл не выбран",QMessageBox::Ok);
    }
    else
    {
        saveManipulators();
        QFile file;
        file.setFileName(fileName);
        file.open(QIODevice::ReadOnly);
        ui->textEdit_Points->append("Исходные точки: ");

        while (!file.atEnd())
        {
            QString line = file.readLine();
            QStringList parts = line.split(" ");
            if (parts.size() == 2)
            {
                Point point;
                point.x = parts[0].toDouble(&isDouble);
                if (isDouble)
                {
                    point.y = parts[1].toDouble(&isDouble);
                    if (isDouble)
                    {
                        points.push_back(point);
                        ui->textEdit_Points->append(QString::number(point.x) + "; " + QString::number(point.y));
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
    }

    ui->tableWidget_Points->setColumnCount(points.size());
    pathBuilding();
}

double MainWindow::square(double x)
{
    return x * x;
}

QVector<double> MainWindow::calcDistances(Manipulator manipulator, QVector<Point> points)
{
    double man_x = manipulator.getX();
    double man_y = manipulator.getY();

    QVector<double> distances;

    for (int i = 0; i < points.size(); i++)
    {
       double distance = sqrt(square(man_x - points[i].x) + square(man_y - points[i].y));
       distances.append(distance);
    }

    return distances;
}

MainWindow::Distance_with_position MainWindow::closestPoint(QVector<double> distances)
{
    Distance_with_position closestPoint;
    closestPoint.distance = NULL;
    closestPoint.position = NULL;

    closestPoint.distance = distances[0];
    closestPoint.position = 0;

    for (int i = 1; i < distances.size(); ++i)
    {
        if (distances[i] < closestPoint.distance)
        {
            closestPoint.distance = distances[i];
            closestPoint.position = i;
        }
    }

    return closestPoint;
}

void MainWindow::pathBuilding()
{
    int column = 0;
    int original_size = points.size();
    timer->start(1000);
    while (points.size() > 0 && column < original_size)
    {
        // Получаем расстояния от каждого манипулятора до каждой точки
        QVector<double> distances_1 = calcDistances(manipulator1, points);
        QVector<double> distances_2 = calcDistances(manipulator2, points);

        Distance_with_position pointForM1 = closestPoint(distances_1);
        Distance_with_position pointForM2 = closestPoint(distances_2);

        // Не одна ли это точка для обоих манипуляторов?
        if (pointForM1.position == pointForM2.position)
        {
            // Если да, выбираем тот манипулятор, который ближе к этой точке
            if (pointForM1.distance < pointForM2.distance)
            {
                // Если первый ближе, он едет на точку
                // Проверяем на то, что манипулятор может достать эту точку сейчас
                if (pointForM1.distance <= manipulator1.getRadius())
                {
                    manipulator1.setX(points[pointForM1.position].x);
                    manipulator1.setY(points[pointForM1.position].y);

                    // Манипулятор на точке
                    // Удаляем эту точку из points
                    points.remove(pointForM1.position);

                    coordMessage();
                    addToTable(manipulator1, 0, column);
                    addToTable(manipulator2, 1, column);

                }
                else
                {
                    // Получается, что он не дойдет ни до какой другой точки в векторе
                    qDebug() << "M1 стоит на месте";
                }
            }
            else // Едет второй
            {
                // Проверяем на то, что манипулятор может достать эту точку сейчас
                if (pointForM2.distance <= manipulator2.getRadius())
                {
                    manipulator2.setX(points[pointForM2.position].x);
                    manipulator2.setY(points[pointForM2.position].y);
                    // Манипулятор на точке
                    // Удаляем эту точку из points
                    points.remove(pointForM2.position);

                    coordMessage();
                    addToTable(manipulator1, 0, column);
                    addToTable(manipulator2, 1, column);
                }
                else
                {
                    // Получается, что он не дойдет ни до какой другой точки в векторе
                    qDebug() << "M2 стоит на месте";
                }

            }
        }
        else // Точки разные
        {
            bool first_done = false;
            bool second_done = false;
            // Первый едет на свою
            if (pointForM1.distance <= manipulator1.getRadius())
            {
                manipulator1.setX(points[pointForM1.position].x);
                manipulator1.setY(points[pointForM1.position].y);
                // Манипулятор на точке



                first_done = true;

                addToTable(manipulator1, 0, column);
            }
            else
            {
                // Получается, что он не дойдет ни до какой другой точки в векторе
                qDebug() << "M1 стоит на месте";

                addToTable(manipulator1, 0, column);

            }
            // Второй едет на свою
            // Проверяем на то, что манипулятор может достать эту точку сейчас
            if (pointForM2.distance <= manipulator2.getRadius())
            {
                manipulator2.setX(points[pointForM2.position].x);
                manipulator2.setY(points[pointForM2.position].y);
                // Манипулятор на точке

                second_done = true;

                addToTable(manipulator2, 1, column);
            }
            else
            {
                // Получается, что он не дойдет ни до какой другой точки в векторе
                qDebug() << "M2 стоит на месте";

                addToTable(manipulator2, 1, column);
            }
            // Удаляем эту точку из points

            if (first_done && second_done && pointForM1.position > pointForM2.position)
            {
                points.remove(pointForM1.position);
                points.remove(pointForM2.position);
            }
            else if (second_done && first_done && pointForM2.position > pointForM1.position)
            {
                points.remove(pointForM2.position);
                points.remove(pointForM1.position);
            }
            coordMessage();
        }
        column++;
    }
    timer->stop();
    QMessageBox::information(this, "Успешно!", "Оптимальные пути построены!", QMessageBox::Ok);
}

void MainWindow::addToTable(Manipulator manipulator, int row, int column)
{
    QString str_m = QString::number(manipulator.getX()) + "; " + QString::number(manipulator.getY());
    QTableWidgetItem * item = new QTableWidgetItem(str_m);
    ui->tableWidget_Points->setItem(row, column, item);
}

void MainWindow::coordMessage()
{
    qDebug() << "M1 в точке {" << manipulator1.getX() << "; "
                               << manipulator1.getY() << "}";
    qDebug() << "M2 в точке {" << manipulator2.getX() << "; "
                               << manipulator2.getY() << "}";

    ui->textEdit_Points->append("M1 в точке {" + QString::number(manipulator1.getX()) + "; " +
                                QString::number(manipulator1.getY()) + "}\nМ2 в точке {" + QString::number(manipulator2.getX()) + "; "
                                + QString::number(manipulator2.getY()) +"}\n");

}
