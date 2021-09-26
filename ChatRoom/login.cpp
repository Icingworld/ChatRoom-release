#include "login.h"
#include "ui_login.h"
#include <QtNetwork>
#include <QDebug>
#include<QMessageBox>

QString getIP()
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

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(),this->height());
}

login::~login()
{
    delete ui;
}

void login::on_pushButton_2_clicked()
{
    users.name = ui->lineEdit->text();
    if(ui->lineEdit->text() == ""){
        QMessageBox::warning(0,tr("警告"),
                             tr("请输入用户名"),QMessageBox::Ok);
        return;
    } else if (ui->lineEdit_2->text() == "") {
        QMessageBox::warning(0,tr("警告"),
                             tr("请输入服务器端口"),QMessageBox::Ok);
        return;
    } else if (ui->ip_server->text() == "")
    {
        QMessageBox::warning(0,tr("警告"),
                             tr("请输入服务器IP"),QMessageBox::Ok);
        return;
    } else {
        this->close();
        users.hostaddress = getIP();
        users.address = getIP();
        mainwindow = new MainWindow(0, users, ui->lineEdit_2->text().toInt(), ui->ip_server->text());
        mainwindow->show();
    }
}
