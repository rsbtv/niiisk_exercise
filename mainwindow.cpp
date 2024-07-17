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
    QString fileName = QFileDialog::getOpenFileName(this,"Открыть в формате TXT","C:\\Users\\student\\Documents","Text files (*.txt)");
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
                Point point;
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
                else {
                    QMessageBox::warning(this, "Ошибка!", "Неверный формат", QMessageBox::Ok);
                    points.clear();
                    break;
                }
            }
        }

        file.close();

        for (int i = 0; i < points.size(); i++) {
            qDebug() << points[i].x;
            qDebug() << points[i].y << "\n";
        }
    }
}
