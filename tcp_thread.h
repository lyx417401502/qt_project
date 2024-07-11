#ifndef TCP_THREAD_H
#define TCP_THREAD_H

#include <QThread>
#include <QHostAddress>
#include <QQueue>
#include <QMutex>
#include <QTcpSocket>
#include <QTcpServer>

class Tcp_Thread : public QThread
{
    Q_OBJECT
public:
    explicit Tcp_Thread(QHostAddress ip,QString port,QObject *parent = nullptr);
    ~Tcp_Thread();

    bool start_flag; //
    bool connect_flag; //
    bool recv_flag; //
    QQueue<QByteArray> bytes_queue;
    QQueue<QString> msg_queue;

    QTcpSocket *socket;
    QTcpServer *server;
    QHostAddress client_ip;
    qint16 client_port;

    void stop();
    void go_on();
    void new_client_connect();

protected:
    void run();

signals:
    void signal_msg();
private:
    QMutex mutex;

private slots:
    void read_tcp();
    void socket_disconnect();

};

#endif // TCP_THREAD_H
