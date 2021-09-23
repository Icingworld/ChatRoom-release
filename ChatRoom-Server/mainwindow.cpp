#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtNetwork>
#include <QMessageBox>

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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(),this->height());
    ui->pswd->setFocus();
    ui->ip_addr->setText(getIP());
    connect(ui->actionReadMe, SIGNAL(triggered()), this, SLOT(actionReadMe_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->ip_addr->setText(getIP());
}

void MainWindow::on_pushButton_clicked()
{
    if(ui->role->text()==""){
        QMessageBox::warning(0,tr("警告"),
                             tr("请输入角色"),QMessageBox::Ok);
        return;
    }
    if(ui->ip_addr->text()==""){
        QMessageBox::warning(0,tr("警告"),
                             tr("IP不能为空！"),QMessageBox::Ok);
        return;
    }
    if(ui->port_num->text()==""){
        QMessageBox::warning(0,tr("警告"),
                             tr("端口不能为空！"),QMessageBox::Ok);
        return;
    }
    servers.name=ui->role->text();
    servers.realm=ui->realm_name->text();
    servers.addr=ui->ip_addr->text();
    servers.port=ui->port_num->text();
    status = new Status(0,servers);
    status->show();
    this->close();
}

void MainWindow::actionReadMe_clicked()
{
    CopyRight = new copyright;
    CopyRight->show();
}
