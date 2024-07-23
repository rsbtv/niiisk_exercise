#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList verticalHeaderLabels;
    ui->tableWidget_Points->setVerticalHeaderLabels(verticalHeaderLabels << "М1" << "М2");
    
    ui->doubleSpinBox_Radius->setValue(8);
    ui->doubleSpinBox_X_2->setValue(2);
    ui->doubleSpinBox_Y_2->setValue(1);
    ui->doubleSpinBox_Radius_2->setValue(10);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setManipulators()
{
    M1 = new Manipulator;
    M2 = new Manipulator;

    M1->setXY(ui->doubleSpinBox_X->value(), ui->doubleSpinBox_Y->value());
    M1->setR(ui->doubleSpinBox_Radius->value());

    M2->setXY(ui->doubleSpinBox_X_2->value(), ui->doubleSpinBox_Y_2->value());
    M2->setR(ui->doubleSpinBox_Radius_2->value());

    // подкл слоты
//    connect(M1, SIGNAL(xyChanged()), this, SLOT(xyChangedSlot()));
//    connect(M2, SIGNAL(xyChanged()), this, SLOT(xyChangedSlot()));
}

void MainWindow::setSpinBoxesEnability(bool state)
{
    ui->doubleSpinBox_X->setEnabled(state);
    ui->doubleSpinBox_X_2->setEnabled(state);
    ui->doubleSpinBox_Y->setEnabled(state);
    ui->doubleSpinBox_Y_2->setEnabled(state);
    ui->doubleSpinBox_Radius->setEnabled(state);
    ui->doubleSpinBox_Radius_2->setEnabled(state);
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
        setManipulators();
        setSpinBoxesEnability(false);
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
    }

    ui->tableWidget_Points->setColumnCount(points.size());
    QThread::sleep(3);
    pathBuilding();
}


void MainWindow::pathBuilding()
{
    int column = 0;
    int original_size = points.size();

    while (points.size() > 0 && column < original_size)
    {
        QVector<double> distancesM1 = M1->getDistances(points);
        QVector<double> distancesM2 = M2->getDistances(points);

        int M1PointPosition = M1->getClosestPoint(distancesM1);
        int M2PointPosition = M2->getClosestPoint(distancesM2);

        Manipulator::Point pointM1 = points[M1PointPosition];
        Manipulator::Point pointM2 = points[M2PointPosition];

        // Не одна ли это точка для обоих манипуляторов?
        if (M1PointPosition == M2PointPosition)
        {
            // Если да, выбираем тот манипулятор, который ближе к этой точке
            if (distancesM1[M1PointPosition] < distancesM2[M2PointPosition])
            {
                // Если первый ближе, он едет на точку
                // Проверяем на то, что манипулятор может достать эту точку сейчас
                if (distancesM1[M1PointPosition] <= M1->getR())
                {
                    M1->setXY(pointM1.x, pointM1.y);
                    // Манипулятор на точке
                    // Удаляем эту точку из points
                    points.remove(M1PointPosition);


                    addToTable(*M1, 0, column);
                    addToTable(*M2, 1, column);
                    coordsChanged();
                    QThread::sleep(3);
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
                if (distancesM2[M2PointPosition] <= M2->getR())
                {

                    M2->setXY(pointM2.x, pointM2.y);
                    // Манипулятор на точке
                    // Удаляем эту точку из points
                    points.remove(M2PointPosition);


                    addToTable(*M1, 0, column);
                    addToTable(*M2, 1, column);
                    coordsChanged();
                    QThread::sleep(3);
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
            if (distancesM1[M1PointPosition] <= M1->getR())
            {
                M1->setXY(pointM1.x, pointM1.y);

                first_done = true;

                addToTable(*M1, 0, column);
            }
            else
            {
                // Получается, что он не дойдет ни до какой другой точки в векторе
                qDebug() << "M1 стоит на месте";

                addToTable(*M1, 0, column);

            }
            // Второй едет на свою
            // Проверяем на то, что манипулятор может достать эту точку сейчас
            if (distancesM2[M2PointPosition] <= M2->getR())
            {
                M2->setXY(pointM2.x, pointM2.y);
                // Манипулятор на точке

                second_done = true;

                addToTable(*M2, 1, column);
            }
            else
            {
                // Получается, что он не дойдет ни до какой другой точки в векторе
                qDebug() << "M2 стоит на месте";

                addToTable(*M2, 1, column);
            }
            // Удаляем эту точку из points

            if (first_done && second_done && M1PointPosition > M2PointPosition)
            {
                points.remove(M1PointPosition);
                points.remove(M2PointPosition);
            }
            else if (second_done && first_done && M2PointPosition > M1PointPosition)
            {
                points.remove(M2PointPosition);
                points.remove(M1PointPosition);
            }
            coordsChanged();
            QThread::sleep(3);
        }
        column++;
    }
    QMessageBox::information(this, "Успешно!", "Оптимальные пути построены!", QMessageBox::Ok);
}

void MainWindow::addToTable(Manipulator manipulator, int row, int column)
{
    QString str_m = QString::number(manipulator.getX()) + "; " + QString::number(manipulator.getY());
    QTableWidgetItem * item = new QTableWidgetItem(str_m);
    ui->tableWidget_Points->setItem(row, column, item);
}

void MainWindow::coordsChanged()
{
    qDebug() << "M1: (" << M1->getX() << "; "<<M1->getY() << ")";
    qDebug() << "M2: (" << M2->getX() << "; "<<M2->getY() << ")";

    ui->textEdit->append("M1: (" + QString::number(M1->getX()) + "; " + QString::number(M1->getY()) + ")");
    ui->textEdit->append("M2: (" + QString::number(M2->getX()) + "; " + QString::number(M2->getY()) + ")");

    QMessageBox::information(this, "Изменения координат", QString("M1: (" + QString::number(M1->getX()) + "; " + QString::number(M1->getY()) + ")\n" + "M2: (" + QString::number(M2->getX()) + "; "
                                                                  + QString::number(M2->getY()) + ")"), QMessageBox::Ok);
}



