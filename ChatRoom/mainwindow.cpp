#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUdpSocket>
#include <QHostInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QKeyEvent>
#include <QColorDialog>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent, User u, int port_, QString ip_server) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionReadMe, SIGNAL(triggered()), this, SLOT(actionReadMe_clicked()));
    setUser(u);
    setWindowTitle(user.name+" 的聊天室");
    ui->userTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->userTableWidget->horizontalHeader()->setStretchLastSection(true);
    init_info();
    udpSocket = new QUdpSocket(this);
    port = port_ + 1;
    server_port = port_;
    //getServerIP();
    server_ip = ip_server;
    IP = ip_server;
    timer = new QTimer();
    connect(timer,&QTimer::timeout,this,&MainWindow::Call);
    ui->server_stst->setText("连接中");
    while(true)
    {
        bool result = udpSocket->bind(port);
        if(!result)
        {
            ui->server_stst->setText("ERROR");
            port += 1;
        }else {
            connect(udpSocket,&QUdpSocket::readyRead,this,&MainWindow::processPendingDatagrams);
            timer->start(1000);
            break;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendMessage(MessageType type,QString ClientAddr="")
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QDateTime time_now = QDateTime::currentDateTime();
    QString time_str = time_now.toString("yyyy.MM.dd hh:mm:ss");
    ClientAddr = getIP();
    out << type << user.name << ClientAddr << port;
    switch (type) {
    case Message:
        if(ui->messageTextEdit->toPlainText() != "")
        {
            out << time_str << ui->messageTextEdit->toPlainText();
            ui->messageTextEdit->clear();
        }
        break;
    case NewParticipant:
        break;
    case ParticipantLeft:
        break;
    case UsrList:
        break;
    case Check:
        break;
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress(server_ip),server_port);
}

void MainWindow::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        in >> messageType;
        QString get_ip;
        QString get_name;
        QString get_time;
        QString get_content;
        int get_port = 0;
        in >> get_name >> get_ip >> get_port;
        switch (messageType) {
        case Message:
            in >> get_time >> get_content;
            if(ui->server_stst->text() != "正常")
            {
                ui->server_stst->setText("正常");
            }
            disPlay(Message,get_name,get_time,get_content);
            break;
        case NewParticipant:
            isConn = 1;
            getNewParticipant(get_name,get_ip,get_port);
            break;
        case ParticipantLeft:
            if(get_name == "服务器" && get_ip == server_ip && get_port == server_port)
            {
                disPlay(ParticipantLeft,get_name,"","");
                isConn = 0;
                ui->server_stst->setText("连接中");
                timer->start(1000);
            }else {
                getParticipantLeft(get_name,get_ip,get_port);
            }
            break;
        case UsrList:
            int all_num;
            in >> all_num;
            in >> server_ip;
            server_ip = IP;
            for (int x = 0;x < all_num;x++)
            {
                in >> get_name >> get_ip >> get_port;
                bool isEmpty = ui->userTableWidget->findItems(get_name,Qt::MatchExactly).isEmpty();
                if(isEmpty)
                {
                    QTableWidgetItem *user = new QTableWidgetItem(get_name);
                    ui->userTableWidget->insertRow(0);
                    ui->userTableWidget->setItem(0,0,user);
                    QTableWidgetItem *user1 = new QTableWidgetItem(get_name);
                    QTableWidgetItem *ip1 = new QTableWidgetItem(get_ip);
                    QString port_ = QString::number(get_port,10);
                    QTableWidgetItem *port1 = new QTableWidgetItem(port_);
                    ui->userTableWidget_raw->insertRow(0);
                    ui->userTableWidget_raw->setItem(0,0,user1);
                    ui->userTableWidget_raw->setItem(0,1,ip1);
                    ui->userTableWidget_raw->setItem(0,2,port1);
                    new_num = ui->usr_num->text().toInt() + 1;
                    QString new_num_ = QString::number(new_num,10);
                    ui->usr_num->setText(new_num_);
                }
            }
            break;
        case Check:
            sendMessage(Check);
            break;
        }
    }
}

void MainWindow::getNewParticipant(QString usr_name,QString usr_ip,int usr_port)
{
    bool isEmpty = ui->userTableWidget->findItems(usr_name,Qt::MatchExactly).isEmpty();
    if(isEmpty)
    {
        QTableWidgetItem *user = new QTableWidgetItem(usr_name);
        ui->userTableWidget->insertRow(0);
        ui->userTableWidget->setItem(0,0,user);
        QTableWidgetItem *user1 = new QTableWidgetItem(usr_name);
        QTableWidgetItem *ip1 = new QTableWidgetItem(usr_ip);
        QString port_ = QString::number(usr_port,10);
        QTableWidgetItem *port1 = new QTableWidgetItem(port_);
        ui->userTableWidget_raw->insertRow(0);
        ui->userTableWidget_raw->setItem(0,0,user1);
        ui->userTableWidget_raw->setItem(0,1,ip1);
        ui->userTableWidget_raw->setItem(0,2,port1);
        new_num = ui->usr_num->text().toInt() + 1;
        QString new_num_ = QString::number(new_num,10);
        ui->usr_num->setText(new_num_);
        disPlay(NewParticipant,usr_name,"","");
    }
}

void MainWindow::getParticipantLeft(QString usr_name, QString usr_ip, int usr_port)
{
    for (int i = 0;i < ui->usr_num->text().toInt();i++)
    {
        if(ui->userTableWidget_raw->item(i,0)->text() == usr_name)
        {
            if(ui->userTableWidget_raw->item(i,1)->text() == usr_ip && ui->userTableWidget_raw->item(i,2)->text().toInt() == usr_port)
            {
                ui->userTableWidget->removeRow(i);
                ui->userTableWidget_raw->removeRow(i);
                new_num = ui->usr_num->text().toInt() - 1;
                QString new_num_ = QString::number(new_num,10);
                ui->usr_num->setText(new_num_);
                disPlay(ParticipantLeft,usr_name,"","");
                return;
            }
        }else
        {
            continue;
        }
    }
}

void MainWindow::disPlay(MessageType type, QString usr_name, QString sendTime,QString content)
{
    switch (type) {
    case Message:
        ui->messageBrowser->setTextColor(Qt::blue);
        ui->messageBrowser->setCurrentFont(QFont("STHeiti",14));
        ui->messageBrowser->append("[" + usr_name + "]" + sendTime);
        ui->messageBrowser->setTextColor(Qt::black);
        ui->messageBrowser->append(content);
        ui->messageBrowser->setCurrentFont(QFont("STHeiti",4));
        ui->messageBrowser->append("\n");
        break;
    case NewParticipant:
        ui->messageBrowser->setCurrentFont(QFont("STHeiti",10));
        ui->messageBrowser->append(usr_name + " 加入了群聊");
        ui->messageBrowser->setCurrentFont(QFont("STHeiti",4));
        ui->messageBrowser->append("\n");
        break;
    case ParticipantLeft:
        ui->messageBrowser->setCurrentFont(QFont("STHeiti",10));
        ui->messageBrowser->append(usr_name + " 离开了群聊");
        ui->messageBrowser->setCurrentFont(QFont("STHeiti",4));
        ui->messageBrowser->append("\n");
        break;
    case UsrList:
        break;
    case Check:
        break;
    }
}

QString MainWindow::getIP()
{
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    QString addr;
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        addr = address.toString();
    }
    return addr;
}

void MainWindow::on_sendButton_clicked()
{
    if(ui->messageTextEdit->toPlainText() != "")
    {
        sendMessage(Message);
    }
}

void MainWindow::on_clearToolBtn_clicked()
{
    if(ui->messageBrowser->toPlainText()=="")
    {
        return;
    }else {
        QMessageBox:: StandardButton result = QMessageBox::information(this,tr("警告"),
                                 tr("清空后无法恢复"),QMessageBox::Cancel|QMessageBox::Ok,QMessageBox::Ok);
        switch (result)
            {
            case QMessageBox::Ok:
            ui->messageBrowser->clear();
                break;
            case QMessageBox::Cancel:
                break;
            default:
                break;
            }
    }
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Return)
        {
            on_sendButton_clicked();
        }
}

void MainWindow::on_exitButton_clicked()
{
    sendMessage(ParticipantLeft,"");
    close();
}

void MainWindow::setUser(User u)
{
    user.name = u.name;
    user.address = u.address;
    user.hostaddress = u.hostaddress;
}

void MainWindow::init_info()
{
    ui->usr_name->setText(user.name);
    ui->ip_addr->setText(user.address);
    ui->server_stst->setText("正常");
    ui->usr_num->setText("0");
}

void MainWindow::actionReadMe_clicked()
{
    Copyright = new copyright;
    Copyright->show();
}

void MainWindow::Call()
{
    time_out++;
    if(isConn)
    {
        timer->stop();
        ui->server_stst->setText("正常");
    }else {
        sendMessage(NewParticipant);
        if(time_out == 10)
        {
            timer->stop();
            ui->server_stst->setText("ERROR");
            QMessageBox::warning(0,tr("警告"),
                                 tr("连接失败"),QMessageBox::Ok);
            return;
        }
    }
}

//这个因为域名要备案，暂时不考虑了
void MainWindow::getServerIP()
{
    QNetworkAccessManager *m_pHttpMgr = new QNetworkAccessManager();
        QString url = "http://fducse.top/ChatRoom.php";
        QNetworkRequest requestInfo;
        requestInfo.setUrl(QUrl(url));
        QEventLoop eventLoop;
        QNetworkReply *reply =  m_pHttpMgr->get(requestInfo);
        connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();

        QByteArray responseByte = reply->readAll();
        responseByte = responseByte.left(responseByte.size() - 1);
        //server_ip = responseByte;
        qDebug() << 1 << server_ip;
        qDebug() << 2 << responseByte;
}

void MainWindow::on_sendPicBtn_clicked()
{
    QMessageBox::warning(0,tr("警告"),
                         tr("没有做这个功能！"),QMessageBox::Ok);
    return;
}

void MainWindow::on_saveToolBtn_clicked()
{
    QMessageBox::warning(0,tr("警告"),
                         tr("没有做这个功能！"),QMessageBox::Ok);
    return;
}
