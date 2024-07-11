#include "tcp_thread.h"
#include <QDebug>

Tcp_Thread::Tcp_Thread(QHostAddress ip,QString port,QObject *parent) : QThread(parent)
{
    start_flag = true;
    connect_flag = false;
    recv_flag = false;



    server = new QTcpServer();
    server->listen(ip, port.toInt());
    connect(server,&QTcpServer::newConnection,this,&Tcp_Thread::new_client_connect);
}


void Tcp_Thread::run()
{
    QByteArray data_temp;
    while(start_flag)
    {
        mutex.lock();
        if(!bytes_queue.isEmpty())
        {
            data_temp = bytes_queue.dequeue();
            msg_queue.enqueue(QString(data_temp));
            emit signal_msg();
        }
        mutex.unlock();
    }
}

void Tcp_Thread::new_client_connect()
{
    socket = server->nextPendingConnection();
    client_ip = socket->peerAddress();
    client_port = socket->peerPort();
    connect(socket,SIGNAL(readyRead()),this,SLOT(read_tcp()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(socket_disconnect()));
    connect_flag = true;
    msg_queue.enqueue(QString("ok [%1]").arg(client_ip.toString()));
    emit signal_msg();
}

void Tcp_Thread::read_tcp()
{
    QByteArray buffer;
    try {
        buffer = socket->readAll();
        qDebug()<<buffer;
    } catch (const std::exception& e) {
        qDebug() << "Exception caught while reading from socket:" << e.what();
        return;
    }

    if (recv_flag && !buffer.isEmpty()) {
         qDebug() << "read tcp";
         mutex.lock();
        bytes_queue.enqueue(buffer);
        mutex.unlock();

    }
}

void Tcp_Thread::socket_disconnect()
{
    connect_flag = false;
    msg_queue.enqueue(QString("close"));
    emit signal_msg();
}

void Tcp_Thread::stop()
{
    mutex.lock();
}

void Tcp_Thread::go_on()
{
    mutex.unlock();
}

Tcp_Thread::~Tcp_Thread()
{

}
