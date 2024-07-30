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
    
    // Инициализация клиента
    client = new QModbusTcpClient(this);
    client->setConnectionParameter(QModbusDevice::NetworkAddressParameter, "127.0.0.1");
    client->setConnectionParameter(QModbusDevice::NetworkPortParameter, 502);
    
    connect(client, &QModbusTcpClient::stateChanged, this, &MainWindow::onStateChanged);
    connect(client, &QModbusTcpClient::errorOccurred, this, &MainWindow::onErrorOccurred);
    
    if (!client->connectDevice()) {
        qDebug() << "Failed to connect to server:" << client->errorString();
        ui->textEdit_2->append("Failed to connect to server: ");
        ui->textEdit_2->append(client->errorString());
    }
    else {
        qDebug() << "Connected to server";
        ui->textEdit_2->append("Connected to server");
    }
    ui->widget->yAxis->setRange(-10,10);
    ui->widget->xAxis->setRange(-10,10);
    
    curve1 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curve2 = new QCPCurve(ui->widget->xAxis, ui->widget->yAxis);
    curve1->setPen(QPen(Qt::blue));
    curve2->setPen(QPen(Qt::red));

    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &MainWindow::updateGraph);


}

MainWindow::~MainWindow()
{
    delete ui;
    if (client)
        client->disconnectDevice();
}

void MainWindow::setManipulators() // настройка манипуляторов
{
    M1 = new Manipulator;
    M2 = new Manipulator;
    
    M1->setXY(ui->doubleSpinBox_X->value(), ui->doubleSpinBox_Y->value());
    M1->setR(ui->doubleSpinBox_Radius->value());
    
    M2->setXY(ui->doubleSpinBox_X_2->value(), ui->doubleSpinBox_Y_2->value());
    M2->setR(ui->doubleSpinBox_Radius_2->value());
    
    // на графике
    M1->reached_x.append(M1->getX());
    M1->reached_y.append(M1->getY());
    
    M2->reached_x.append(M2->getX());
    M2->reached_y.append(M2->getY());
    
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
                        sendData(point);
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
    // QThread::sleep(3);
    ui->tableWidget_Points->setColumnCount(points.size());
    pathBuilding(); // вызов функции для построения пути
}


void MainWindow::pathBuilding() // построение пути
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
                    M1->setXY(pointM1.x, pointM1.y); // Манипулятор на точке
                    M1->reached_x.append(pointM1.x);
                    M1->reached_y.append(pointM1.y);
                    //                    curve1->setData(M1->reached_x, M1->reached_y);
                    
                    //                    ui->widget->replot();

                    if (!timer->isActive()) {
                        timer->start(50); // Запускаем таймер только один раз
                        animationDuration = 3000; // Длительность анимации в миллисекундах
                        animationStartTime = QDateTime::currentMSecsSinceEpoch();
                    }

                    points.remove(M1PointPosition); // Удаляем эту точку из points
                    
                    addToTable(*M1, 0, column);
                    addToTable(*M2, 1, column); // добавляем точки в таблицу
                    coordsChanged(); // отчет
                    // QThread::sleep(3);
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
                    
                    M2->setXY(pointM2.x, pointM2.y); // Манипулятор на точке
                    points.remove(M2PointPosition); // Удаляем эту точку из points
                    M2->reached_x.append(pointM2.x);
                    M2->reached_y.append(pointM2.y);
                    if (!timer->isActive()) {
                        timer->start(50); // Запускаем таймер только один раз
                        animationDuration = 3000; // Длительность анимации в миллисекундах
                        animationStartTime = QDateTime::currentMSecsSinceEpoch();
                    }

                    addToTable(*M1, 0, column);
                    addToTable(*M2, 1, column); // добавляем точки в таблицу
                    coordsChanged(); // отчет
                    // QThread::sleep(3);
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
                M1->reached_x.append(pointM1.x);
                M1->reached_y.append(pointM1.y);
                if (!timer->isActive()) {
                    timer->start(50); // Запускаем таймер только один раз
                    animationDuration = 3000; // Длительность анимации в миллисекундах
                    animationStartTime = QDateTime::currentMSecsSinceEpoch();
                }

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
                M2->setXY(pointM2.x, pointM2.y); // Манипулятор на точке
                M2->reached_x.append(pointM2.x);
                M2->reached_y.append(pointM2.y);
                if (!timer->isActive()) {
                    timer->start(50); // Запускаем таймер только один раз
                    animationDuration = 3000; // Длительность анимации в миллисекундах
                    animationStartTime = QDateTime::currentMSecsSinceEpoch();
                }

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
            // QThread::sleep(3);
        }
        column++;
    }
    QMessageBox::information(this, "Успешно!", "Оптимальные пути построены!", QMessageBox::Ok);
}

void MainWindow::addToTable(Manipulator manipulator, int row, int column) // добавляем точку в таблицу
{
    QString str_m = QString::number(manipulator.getX()) + "; " + QString::number(manipulator.getY());
    QTableWidgetItem * item = new QTableWidgetItem(str_m);
    ui->tableWidget_Points->setItem(row, column, item);
}

void MainWindow::coordsChanged() // отчет
{
    qDebug() << "M1: (" << M1->getX() << "; "<<M1->getY() << ")";
    qDebug() << "M2: (" << M2->getX() << "; "<<M2->getY() << ")";
    
    ui->textEdit->append("M1: (" + QString::number(M1->getX()) + "; " + QString::number(M1->getY()) + ")");
    ui->textEdit->append("M2: (" + QString::number(M2->getX()) + "; " + QString::number(M2->getY()) + ")");
    
    QMessageBox::information(this, "Изменения координат", QString("M1: (" + QString::number(M1->getX()) + "; " + QString::number(M1->getY()) + ")\n" + "M2: (" + QString::number(M2->getX()) + "; "
                                                                  + QString::number(M2->getY()) + ")"), QMessageBox::Ok);
}

void MainWindow::sendData(Manipulator::Point point) // отправка на сервер
{
    if (client->state() != QModbusDevice::ConnectedState)
    {
        qDebug() << "Client is not connected";
        ui->textEdit_2->append("Client is not connected");
        return;
    }
    client->setTimeout(3000); // Установка таймаута в 3000 мс (3 секунды)
    
    
    // Отправка данных
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters, 0, 2);
    writeUnit.setValue(0, static_cast<quint16>(point.x * 1000));
    writeUnit.setValue(1, static_cast<quint16>(point.y * 1000));
    
    if (auto *reply = client->sendWriteRequest(writeUnit, 1))
    {
        connect(reply, &QModbusReply::finished, this, [this, reply]()
        {
            if (reply->error() == QModbusDevice::NoError)
            {
                qDebug() << "Write request succeeded";
                ui->textEdit_2->append("Write request succeeded");
                readData();
            }
            else
            {
                qDebug() << "Write request error:" << reply->errorString();
                ui->textEdit_2->append("Write request error:" + reply->errorString());
            }
            reply->deleteLater();
        });
    }
    else
    {
        qDebug() << "Failed to send write request:" << client->errorString();
        ui->textEdit_2->append("Failed to send write request:" + client->errorString());
    }
}

void MainWindow::readData() // чтение точек
{
    // Создается объект readUnit, который определяет, что мы хотим прочитать 2 регистра хранения, начиная с адреса 0.
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, 0, 2);
    // Отправляется запрос на чтение к устройству с адресом 1. Если запрос успешно отправлен, возвращается объект reply.
    if (auto *reply = client->sendReadRequest(readUnit, 1))
    {
        // Устанавливается соединение, чтобы обработать ответ, когда он будет получен.
        connect(reply, &QModbusReply::finished, this, [this, reply]()
        {
            if (reply->error() == QModbusDevice::NoError)
            {
                // Если ошибок нет, извлекаются значения из полученных данных и преобразуются в числа с плавающей точкой.
                const QModbusDataUnit unit = reply->result();
                double x = static_cast<double>(unit.value(0)) / 1000.0;
                double y = static_cast<double>(unit.value(1)) / 1000.0;
                qDebug() << "Received data:" << x << y;
                ui->textEdit_2->append("Received data" + QString::number(x) + QString::number(y));
            }
            else
            {
                qDebug() << "Read request error:" << reply->errorString();
                ui->textEdit_2->append("Read request error:" + reply->errorString());
            }
            // планируется на удаление, чтобы избежать утечек памяти.
            reply->deleteLater();
        });
    }
    else
    {
        qDebug() << "Failed to send read request:" << client->errorString();
        ui->textEdit_2->append("Failed to send read request:" + client->errorString());
    }
}

void MainWindow::onStateChanged(int state) // при изменении состояния соединения
{
    if (state == QModbusDevice::ConnectedState)
    {
        qDebug() << "Connected to server";
        ui->textEdit_2->append("Connected to server");
    }
    else if (state == QModbusDevice::UnconnectedState)
    {
        qDebug() << "Disconnected from server";
        ui->textEdit_2->append("Disconnected from server");
    }
}

void MainWindow::onErrorOccurred(QModbusDevice::Error error) // при возникновении ошибки
{
    qDebug() << "Error occurred:" << client->errorString();
    ui->textEdit_2->append("Error occurred:" + client->errorString());
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

void MainWindow::updateGraph() {
    qint64 elapsedTime = QDateTime::currentMSecsSinceEpoch() - animationStartTime;
    double progress = static_cast<double>(elapsedTime) / animationDuration;

    if (progress >= 1.0) {
        timer->stop();
        progress = 1.0;
    }

    curve1->addData(M1->getX(), M1->getY());
    curve2->addData(M2->getX(), M2->getY());

    ui->widget->replot();
}





