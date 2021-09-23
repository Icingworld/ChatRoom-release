#ifndef STATUS_H
#define STATUS_H

#include <QWidget>
#include <QUdpSocket>
#include <QTimer>

struct Server{
    QString name;
    QString realm;
    QString addr;
    QString port;
};

struct Ttime{
    int days;
    int hours;
    int minutes;
    int seconds;
};

enum MessageType {
    Message,
    NewParticipant,
    ParticipantLeft,
    UsrList,
    Check
};

namespace Ui {
class Status;
}

class Status : public QWidget
{
    Q_OBJECT

public:
    explicit Status(QWidget *parent = nullptr, Server s={"","","",""});
    ~Status();

protected:
    void setServer(Server s);
    void sendMessage(MessageType type,QString ClientName,QString ClientAddr,QString SendTime,int ClientPort,QString SendData,int flag);
    void log(MessageType type,QString data);
    void savelog();

private slots:
    void on_closeBtn_clicked();
    void processPendingDatagrams();
    void display();
    void on_change_clicked();

private:
    Ui::Status *ui;
    Server server;
    Ttime ttime;
    QUdpSocket *udpSocket;
    QUdpSocket *udpSocket_test;
    QTimer *timer;
    QList<int> port_list;
    QList<int> temp_port;
    bool ischanged = 0;
    int port;
    int client_port;
    int messageType;
};

#endif // STATUS_H
