#include "status.h"
#include "ui_status.h"
#include "sstream"
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QColor>
#include <QList>
#include <QMovie>
#include <QFileDialog>
#include <QFile>

Status::Status(QWidget *parent, Server s) :
    QWidget(parent),
    ui(new Ui::Status)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(),this->height());
    setServer(s);
    ui->waitinglabel->setVisible(0);
    ui->Log->setVisible(0);
    ui->role->setText(server.name);
    ui->ip_addr->setText(server.addr);
    ui->port_num->setText(server.port);
    ui->usr_online->setText("0");
    ui->usr_info->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->usr_info->horizontalHeader()->setStretchLastSection(true);
    if(server.realm.isEmpty()){
        ui->realm_name->setText("未绑定域名");
    }else {
        ui->realm_name->setText(server.realm);
    }
    udpSocket = new QUdpSocket(this);
    bool result = udpSocket->bind(server.port.toInt());
    if(!result)
    {
        QMessageBox::information(this,tr("错误"),
                                 tr("UDP SOCKET连接失败!"),QMessageBox::Ok);
        ui->server_stat->setText("ERROR，请检查网络状态");
        this->close();
        return;
    }else {
        ui->server_stat->setText("检测中");
        sendMessage(NewParticipant,server.name,server.addr,"",server.port.toInt(),"",2);
    }
    connect(udpSocket,&QUdpSocket::readyRead,this,&Status::processPendingDatagrams);
    ttime.days=ttime.hours=ttime.minutes=ttime.seconds=0;
    display();
    timer = new QTimer();
    connect(timer,&QTimer::timeout,this,&Status::display);
    timer->start(1000);
}

Status::~Status()
{
    delete ui;
}

void Status::sendMessage(MessageType type,QString ClientName,QString ClientAddr,QString SendTime,int ClientPort,QString SendData,int flag = 0)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    out << type << ClientName << ClientAddr << ClientPort;

    switch (type) {
    case Message:
    {
        QString temp = QString::number(ClientPort,10);
        log(Message,"发送消息： " + ClientName + "   " + ClientAddr + "   " + temp + "   " + SendTime + "   " + SendData);
        out << SendTime << SendData;
        break;
    }
    case NewParticipant:
        break;
    case ParticipantLeft:
        break;
    case UsrList:
        out << ui->usr_online->text().toInt();
        out << server.addr;
        for (int xx = 0;xx < ui->usr_online->text().toInt();xx++) {
            out << ui->usr_info->item(xx,0)->text() << ui->usr_info->item(xx,1)->text() << ui->usr_info->item(xx,2)->text().toInt();
            log(UsrList,"在线列表：" + ui->usr_info->item(xx,0)->text() + "   " + ui->usr_info->item(xx,1)->text() + "   " + ui->usr_info->item(xx,2)->text());
        }
        break;
    case Check:
        break;
    }
    if(flag == 0)
    {
        for (int i = 0;i < ui->usr_online->text().toInt();i++) {
            if(type == Check){
                log(Check,"询问用户：" + ui->usr_info->item(i,0)->text() + "   " + ui->usr_info->item(i,1)->text() + "   " + ui->usr_info->item(i,2)->text());
            }
            udpSocket->writeDatagram(data,data.length(),QHostAddress(ui->usr_info->item(i,1)->text()),ui->usr_info->item(i,2)->text().toInt());
        }
    }else if (flag == 1) {
        QString temp = QString::number(ClientPort,10);
        log(UsrList,"发送列表给："  + ClientName + "   " + ClientAddr + "   " + temp);
        udpSocket->writeDatagram(data,data.length(),QHostAddress(ClientAddr),ClientPort);
    }else if (flag == 2) {
        udpSocket->writeDatagram(data,data.length(),QHostAddress(server.addr),server.port.toInt());
    }
}

void Status::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
#if 1
        QHostAddress client_address;
        quint16 recPort = 0;
#endif
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size(),&client_address, &recPort);
        QString temp_port=QString("%1").arg(recPort);
        QDataStream in(&datagram,QIODevice::ReadOnly);
        in >> messageType;
        QString client_ip;
        QString client_name;
        QString client_time;
        QString client_content;
        int client_port = 0;
        in >> client_name >> client_ip;
        client_ip = client_address.toString().right(client_address.toString().size() - 7);
        client_port = temp_port.toInt();
        switch (messageType)
        {
        case Message:
        {
            in >> client_port >> client_time >> client_content;
            QString temp = QString::number(client_port,10);
            log(Message,"接收信息： " + client_name + "   " + client_ip + "   " + temp + "   " + client_time + "   " + client_content);
            sendMessage(Message,client_name,client_ip,client_time,client_port,client_content);
            break;
        }
        case NewParticipant:
        {
            //这个判断的设计实在糟糕，太**容易出事了，早晚给他扬了
            if(client_ip == "127.0.0.1" || client_port != server.port.toInt())
            {
                log(NewParticipant,"新成员：" + client_name + "   " + client_ip + "   " + temp_port);
                int new_num = ui->usr_online->text().toInt() + 1;
                QString new_num_ = QString::number(new_num,10);
                ui->usr_online->setText(new_num_);
                QTableWidgetItem *user = new QTableWidgetItem(client_name);
                QTableWidgetItem *ip = new QTableWidgetItem(client_ip);
                QTableWidgetItem *port = new QTableWidgetItem(temp_port);
                ui->usr_info->insertRow(0);
                ui->usr_info->setItem(0,0,user);
                ui->usr_info->setItem(0,1,ip);
                ui->usr_info->setItem(0,2,port);
                QString client_status = "1";
                QTableWidgetItem *status = new QTableWidgetItem(client_status);
                ui->usr_info->setItem(0,3,status);
                sendMessage(NewParticipant,client_name,client_ip,"",client_port,"");
                if(client_ip != server.addr || client_port != server.port)
                {
                    sendMessage(UsrList,client_name,client_ip,"",client_port,"",1);
                }
            }else {
                ui->server_stat->setText("正常");
                log(NewParticipant,"服务器：" + server.name + "   " + server.addr + "   " + server.port);
            }
            break;
        }
        case ParticipantLeft:
        {
            in >> client_port;
            QString temp = QString::number(client_port,10);
            log(ParticipantLeft,"成员离开：" + client_name + "   " + client_ip + "   " + temp);
            sendMessage(ParticipantLeft,client_name,client_ip,"",client_port,"");
            for (int i = 0;i < ui->usr_info->rowCount();i++) {
                if(ui->usr_info->item(i,0)->text() == client_name)
                {
                    if(ui->usr_info->item(i,1)->text() == client_ip && ui->usr_info->item(i,2)->text().toInt() == client_port)
                    {
                        ui->usr_info->removeRow(i);
                        break;
                    }
                }
            }
            int new_num = ui->usr_online->text().toInt() - 1;
            QString new_num_ = QString::number(new_num,10);
            ui->usr_online->setText(new_num_);
            break;
        }
        case Check:
            in >> client_port;
            QString temp = QString::number(client_port,10);
            log(Check,"确认存活：" +  client_name + "   " + client_ip + "   " + temp);
            bool isFound = 0;
            for (int i = 0;i < ui->usr_info->rowCount();i++)
            {
                if(ui->usr_info->item(i,0)->text() == client_name)
                {
                    if(ui->usr_info->item(i,1)->text() == client_ip && ui->usr_info->item(i,2)->text().toInt() == client_port)
                    {
                        QString status = "1";
                        QTableWidgetItem *status_ = new QTableWidgetItem(status);
                        ui->usr_info->setItem(i,3,status_);
                        isFound = 1;
                        break;
                    }
                }else{
                    continue;
                }
            }
            if(!isFound)
            {
                sendMessage(NewParticipant,client_name,client_ip,"",client_port,"",2);
            }
            break;
        }
    }
}

void Status::on_closeBtn_clicked()
{
    timer->stop();
    ui->waitinglabel->setVisible(1);
    QMovie *movie = new QMovie(":/images/loading.gif");
    ui->waitinglabel->setMovie(movie);
    movie->start();
    savelog();
    sendMessage(ParticipantLeft,"服务器",server.addr,"",server.port.toInt(),"");
    log(ParticipantLeft,"关闭服务器");
    close();
}

void Status::setServer(Server s)
{
    server.name = s.name;
    server.realm = s.realm;
    server.addr = s.addr;
    server.port = s.port;
}

void Status::display()
{
    if(ttime.seconds<=59)
    {
        ttime.seconds+=1;
    }
    if(ttime.seconds==60)
    {
        ttime.seconds=0;
        ttime.minutes+=1;
    }
    if(ttime.minutes==60)
    {
        ttime.minutes=0;
        ttime.hours+=1;
    }
    if(ttime.hours==24)
    {
        ttime.hours=0;
        ttime.days+=1;
    }
    QString day_ = QString::number(ttime.days,10);
    ui->day->setText(day_);
    QString hour_ = QString::number(ttime.hours,10);
    ui->hour->setText(hour_);
    QString minute_ = QString::number(ttime.minutes,10);
    ui->minute->setText(minute_);
    QString second_ = QString::number(ttime.seconds,10);
    ui->second->setText(second_);
    if(ttime.minutes % 1 == 0 && ttime.seconds == 0)
    {
        log(Check,"开始询问用户状态");
        sendMessage(Check,server.name,server.addr,"",server.port.toInt(),"");
        for (int i = 0;i < ui->usr_info->rowCount();i++) {
            QString zero = "0";
            QTableWidgetItem *temp = new QTableWidgetItem(zero);
            ui->usr_info->setItem(i,3,temp);
        }
    }
    if(ttime.minutes % 1 == 0 && ttime.seconds == 15)
    {
        while (clearUsers()) {
        }
    }
}

bool Status::clearUsers()
{
    for (int i = 0;i< ui->usr_info->rowCount();i++) {
        if(ui->usr_info->item(i,3)->text() == "0")
        {
            log(Check,"清理用户：" + ui->usr_info->item(i,0)->text() + "   " + ui->usr_info->item(i,1)->text() + "   " + ui->usr_info->item(i,2)->text());
            sendMessage(ParticipantLeft,ui->usr_info->item(i,0)->text(),ui->usr_info->item(i,1)->text(),"",ui->usr_info->item(i,2)->text().toInt(),"");
            ui->usr_info->removeRow(i);
            int new_num = ui->usr_online->text().toInt() - 1;
            QString new_num_ = QString::number(new_num,10);
            ui->usr_online->setText(new_num_);
            if(ui->usr_online->text().toInt() != ui->usr_info->rowCount())
            {
                QString new_num1 = QString::number(ui->usr_info->rowCount(),10);
                ui->usr_online->setText(new_num1);
            }
            return 1;
        }
    }
    return 0;
}

void Status::log(MessageType type,QString data)
{
    switch (type) {
    case Message:
        ui->logMessage->setTextColor(Qt::blue);
        ui->logMessage->setCurrentFont(QFont("STHeiti",13));
        ui->logMessage->append(data);
        break;
    case NewParticipant:
        ui->logMessage->setTextColor(QColor(255,0,0));
        ui->logMessage->setCurrentFont(QFont("STHeiti",13));
        ui->logMessage->append(data);
        break;
    case ParticipantLeft:
        ui->logMessage->setTextColor(Qt::red);
        ui->logMessage->setCurrentFont(QFont("STHeiti",13));
        ui->logMessage->append(data);
        break;
    case UsrList:
        ui->logMessage->setTextColor(QColor(51, 204, 51));
        ui->logMessage->setCurrentFont(QFont("STHeiti",13));
        ui->logMessage->append(data);
        break;
    case Check:
        ui->logMessage->setTextColor(Qt::black);
        ui->logMessage->setCurrentFont(QFont("STHeiti",13));
        ui->logMessage->append(data);
        break;
    }
}

void Status::on_change_clicked()
{
    if(!ischanged)
    {
        ischanged = 1;
        ui->Log->setVisible(1);
        ui->usr_info->setVisible(0);
        ui->closeBtn->setVisible(0);
        ui->change->setText("状态面板");
    }else {
        ischanged = 0;
        ui->Log->setVisible(0);
        ui->usr_info->setVisible(1);
        ui->closeBtn->setVisible(1);
        ui->change->setText("查看日志");
    }
}

void Status::savelog()
{
    QDateTime time_now = QDateTime::currentDateTime();
    QString time_str = time_now.toString("yyyy.MM.dd hh:mm:ss");
    QString filename = QDir::currentPath() + "/Log.txt";
    QFile file(filename);
    file.open(QFile::ReadWrite|QIODevice::Text|QIODevice::Append);
    QTextStream out(&file);
    out << time_str + "\n" << ui->logMessage->toPlainText().toUtf8() << "\n";
    file.close();
}
