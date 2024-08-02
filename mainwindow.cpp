#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Настройка таблицы
    QStringList verticalHeaderLabels;
    ui->tableWidget_Points->setVerticalHeaderLabels(verticalHeaderLabels << "М1" << "М2");
    
    // Дефолтные значения манипуляторов
    ui->doubleSpinBox_Radius->setValue(8);
    ui->doubleSpinBox_X_2->setValue(2);
    ui->doubleSpinBox_Y_2->setValue(1);
    ui->doubleSpinBox_Radius_2->setValue(10);
    
    ui->widget->yAxis->setRange(-10,10);
    ui->widget->xAxis->setRange(-10,10);
    
    curve1 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curve2 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curve1->setPen(QPen(Qt::blue));
    curve2->setPen(QPen(Qt::red));

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGraph);

    socket = new QTcpSocket;
    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    socket->connectToHost("127.0.0.1", 1234);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (socket)
    {
        socket->disconnectFromHost();
    }
}

void MainWindow::setManipulators() // настройка манипуляторов
{
    M1 = new Manipulator;
    M2 = new Manipulator;
    
    M1->setXY(ui->doubleSpinBox_X->value(), ui->doubleSpinBox_Y->value());
    M1->setR(ui->doubleSpinBox_Radius->value());
    
    M2->setXY(ui->doubleSpinBox_X_2->value(), ui->doubleSpinBox_Y_2->value());
    M2->setR(ui->doubleSpinBox_Radius_2->value());

    // Добавляем изначальные позиции в векторы
    M1->reachedPoints.append(M1->getXY());
    M2->reachedPoints.append(M2->getXY());
}

void MainWindow::setSpinBoxesEnability(bool state) // блок/разблок спинбоксов
{
    ui->doubleSpinBox_X->setEnabled(state);
    ui->doubleSpinBox_X_2->setEnabled(state);
    ui->doubleSpinBox_Y->setEnabled(state);
    ui->doubleSpinBox_Y_2->setEnabled(state);
    ui->doubleSpinBox_Radius->setEnabled(state);
    ui->doubleSpinBox_Radius_2->setEnabled(state);
}

void MainWindow::on_pushButton_LoadPoints_clicked() // начать чтение из файла
{
    bool isDouble;
    // выбор файла через диалог
    QString fileName = QFileDialog::getOpenFileName(this,"Открыть в формате TXT","C:\\Users\\student\\Documents","Text files (*.txt)");
    if (fileName.isEmpty())
    {
        QMessageBox::information(this,"Ошибка","Файл не выбран",QMessageBox::Ok);
    }
    else
    {
        // изменение интерфейса
        setManipulators();
        setSpinBoxesEnability(false);
        ui->pushButton_LoadPoints->setEnabled(false);
        
        QFile file;
        file.setFileName(fileName);
        file.open(QIODevice::ReadOnly);
        
        while (!file.atEnd()) // парсинг точек
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
    pathBuilding(); // вызов функции для построения пути
}

void MainWindow::pathBuilding() // Построение пути
{
    for (int i = 0; i < points.size(); ++i)
    {
        // Берём расстояние до i-ой точки от каждого из манипуляторов
        double distanceM1 = M1->getDistance(points[i]);
        double distanceM2 = M2->getDistance(points[i]);

        if (distanceM1 <= M1->getR()) // Точка в радиусе М1?
        {
            if (distanceM2 <= M2->getR()) // Стоит ли вообще в таком случае смотреть на М2?
            {
                if (distanceM1 < distanceM2) // M1 ближе
                {
                    M1->setXY(points[i].x, points[i].y); // М1 едет до этой точки
                    M1->reachedPoints.append(points[i]);

                    M2->reachedPoints.append(M2->getXY()); // M2 остаётся на месте
                }
                else if (distanceM2 < distanceM1) // M2 ближе
                {
                    M2->setXY(points[i].x, points[i].y); // M2 едет до этой точки
                    M2->reachedPoints.append(points[i]);

                    M1->reachedPoints.append(M1->getXY()); // M1 остаётся на месте
                }
                else
                {
                    M1->setXY(points[i].x, points[i].y); // Оба равноудалены, пусть едет 1-ый
                    M1->reachedPoints.append(points[i]);

                    M2->reachedPoints.append(M2->getXY()); // M2 остаётся на месте
                }
            }
            else
            {
                M1->setXY(points[i].x, points[i].y); // М1 едет до этой точки
                M1->reachedPoints.append(points[i]);

                M2->reachedPoints.append(M2->getXY()); // M2 остаётся на месте
            }
        }
        else if (distanceM2 <= M2->getR()) // Точка в радиусе М2, но вне радиуса М1
        {
            M2->setXY(points[i].x, points[i].y); // M2 едет до этой точки
            M2->reachedPoints.append(points[i]);

            M1->reachedPoints.append(M1->getXY()); // M1 остаётся на месте
        }

        sendData(M1->getXY(), M2->getXY()); // Отправляем точки на сервер
        addToTable(i);                      // Добавляем в таблицу
        // Добавляем в график
        if (!timer->isActive())
        {
            timer->start(50);               // Запускаем таймер только один раз
            animationDuration = 750;       // Длительность анимации в миллисекундах
            animationStartTime = QDateTime::currentMSecsSinceEpoch(); // количество миллисекунд, прошедших с 1 января 1970 года
        }
        coordsChanged();                    // Отчитываемся о координатах
        QThread::sleep(3);
    }
    QMessageBox::information(this, "Успешно!", "Оптимальные пути построены!", QMessageBox::Ok);
}

void MainWindow::addToTable(int column) // Добавляем точку в таблицу
{
    // Формируем подписи ячеек
    QString str_M1 = QString::number(M1->getX()) + "; " + QString::number(M1->getY());
    QString str_M2 = QString::number(M2->getX()) + "; " + QString::number(M2->getY());

    // Выделяем память и заполняем ячейки
    QTableWidgetItem * item1 = new QTableWidgetItem(str_M1);
    QTableWidgetItem * item2 = new QTableWidgetItem(str_M2);
    ui->tableWidget_Points->setItem(0, column, item1);
    ui->tableWidget_Points->setItem(1, column, item2);
}

void MainWindow::coordsChanged() // отчет
{
    qDebug() << "M1: (" << M1->getX() << "; "<<M1->getY() << ")";
    qDebug() << "M2: (" << M2->getX() << "; "<<M2->getY() << ")";
    
    ui->textEdit->append("M1: (" + QString::number(M1->getX()) + "; " + QString::number(M1->getY()) + ")");
    ui->textEdit->append("M2: (" + QString::number(M2->getX()) + "; " + QString::number(M2->getY()) + ")");

    ui->doubleSpinBox_X->setValue(M1->getX());
    ui->doubleSpinBox_Y->setValue(M1->getY());
    ui->doubleSpinBox_X_2->setValue(M2->getX());
    ui->doubleSpinBox_Y_2->setValue(M2->getY());
    
    QMessageBox::information(this, "Изменения координат", QString("M1: (" + QString::number(M1->getX()) + "; " + QString::number(M1->getY()) + ")\n" + "M2: (" + QString::number(M2->getX()) + "; "
                                                                  + QString::number(M2->getY()) + ")"), QMessageBox::Ok);
}

void MainWindow::on_pushButton_Reset_clicked() // сброс манипуляторво
{
    delete M1;
    delete M2;
    ui->doubleSpinBox_Radius->setValue(8);
    ui->doubleSpinBox_X_2->setValue(2);
    ui->doubleSpinBox_Y_2->setValue(1);
    ui->doubleSpinBox_Radius_2->setValue(10);
    setSpinBoxesEnability(true);
    ui->pushButton_LoadPoints->setEnabled(true);
}

void MainWindow::updateGraph()
{
    qint64 elapsedTime = QDateTime::currentMSecsSinceEpoch() - animationStartTime; // Вычисляем время, прошедшее с начала анимации
    double progress = static_cast<double>(elapsedTime) / animationDuration; // текущий прогресс анимации от 0.0 до 1.0 по времени,
    // прошедшем с начала анимации.
    progress = qMin(progress, 1.0); // Ограничиваем значение прогресса 1.0, чтобы не превышать его

    double pastX1 = M1->reachedPoints[M1->reachedPoints.size()-2].x;
    double pastX2 = M2->reachedPoints[M2->reachedPoints.size()-2].x;
    double pastY1 = M1->reachedPoints[M1->reachedPoints.size()-2].y;
    double pastY2 = M2->reachedPoints[M2->reachedPoints.size()-2].y;

    double targetX1 = M1->reachedPoints.last().x;
    double targetX2 = M2->reachedPoints.last().x;
    double targetY1 = M1->reachedPoints.last().y;
    double targetY2 = M2->reachedPoints.last().y;

    // Интерполяция координат
    double x1 = pastX1 * (1.0 - progress) + targetX1 * progress;
    double y1 = pastY1 * (1.0 - progress) + targetY1 * progress;
    double x2 = pastX2 * (1.0 - progress) + targetX2 * progress;
    double y2 = pastY2 * (1.0 - progress) + targetY2 * progress;

    // Обновление данных графика
    curve1->addData(x1, y1);
    curve2->addData(x2, y2);

    ui->widget->replot();

    // Проверка завершения анимации
    if (progress >= 1.0) {
        timer->stop();
    }
}



void MainWindow::sendData(Manipulator::Point pointM1, Manipulator::Point pointM2)
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Client is not connected";
        ui->textEdit_2->append("Client is not connected");
        return;
    }

    // Создаем массив для отправки данных
    Manipulator::Point points_arr[2] = {pointM1, pointM2};

    // Отправка данных
    socket->write(reinterpret_cast<const char*>(&points_arr), sizeof(points_arr));
    socket->flush();
}

void MainWindow::onConnected()
{
    ui->textEdit_2->append("Connected to server.");
}

void MainWindow::onReadyRead()
{
    while (socket->bytesAvailable() >= sizeof(Manipulator::Point) * 2)
    {
        Manipulator::Point points[2];
        socket->read(reinterpret_cast<char*>(&points), sizeof(points)); // read ожидает указатель на char, который представляет собой байты.
        qDebug() << "Received points:" << points[0].x << "," << points[0].y << "and" << points[1].x << "," << points[1].y;
        ui->textEdit_2->append(QString("Received points: M1: (%1, %2) and M2: (%3, %4)")
                               .arg(points[0].x).arg(points[0].y)
                .arg(points[1].x).arg(points[1].y));
        ui->doubleSpinBox_X_1_read->setValue(points[0].x);
        ui->doubleSpinBox_Y_1_read->setValue(points[0].y);
        ui->doubleSpinBox_X_2_read->setValue(points[1].x);
        ui->doubleSpinBox_Y_2_read->setValue(points[1].y);
    }
}




