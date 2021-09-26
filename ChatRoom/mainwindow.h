#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QVariant>
#include <QByteArray>
#include <QString>
#include "copyright.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum MessageType {
    Message,
    NewParticipant,
    ParticipantLeft,
    UsrList,
    Check
};

struct User{
    QString name;
    QString hostaddress;
    QString address;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, User u = {"","",""}, int port_ = 0, QString ip_server = "");
    ~MainWindow();
    void setUser(User u);

protected:
    QString getIP();
    void init_info();
    void sendMessage(MessageType type, QString ClientAddr);
    void getNewParticipant(QString usr_name,QString usr_ip,int usr_port);
    void getParticipantLeft(QString usr_name,QString usr_ip,int usr_port);
    void disPlay(MessageType type,QString usr_name,QString sendTime,QString content);
    void keyPressEvent(QKeyEvent *event);
    void getServerIP();

private slots:
    void on_clearToolBtn_clicked();
    void on_sendButton_clicked();
    void on_exitButton_clicked();
    void on_sendPicBtn_clicked();
    void on_saveToolBtn_clicked();
    void processPendingDatagrams();
    void actionReadMe_clicked();
    void Call();

private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    copyright *Copyright;
    QTimer *timer;
    User user;
    QColor color;
    QString server_ip;
    QString IP;
    int messageType;
    int port;
    int server_port;
    int new_num;
    int time_out = 0;
    bool isConn = 0;
};
#endif // MAINWINDOW_H
