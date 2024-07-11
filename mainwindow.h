#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include "tcp_thread.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QHostAddress get_local_host_ip();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Tcp_Thread *tcp_thread;


private slots:
    void print_tcp_msg();




    void timerEvent( QTimerEvent *event );
    void on_ringbtton_clicked();
    void on_fansbtton_clicked();
};
#endif // MAINWINDOW_H
