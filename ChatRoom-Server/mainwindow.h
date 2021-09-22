#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "status.h"
#include "copyright.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void actionReadMe_clicked();

private:
    Ui::MainWindow *ui;
    copyright *CopyRight;
    Server servers;
    Status *status;
};
#endif // MAINWINDOW_H
