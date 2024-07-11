#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dht11_lx3.h"
#include <wiringPi.h>
#include <QDebug>
#include "sr04.h"
#include "IconController.h"

#include <QNetworkInterface>


double hum = 0;
double tem = 0;
dht11_lx3 dht11;
int m_nTimerId;

SR04 sr04;
 IconController controller;

 bool flag_fan=false;
 bool flag_ring=false;
 bool flag_sr04=false;

bool flag_humi_1=false;
bool flag_tem_1=false;
bool flag_humi_2=false;
bool flag_tem_2=false;
bool flag_humi_3=false;
bool flag_tem_3=false;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QWidget w;
    w.showFullScreen();



    QMap<int, QIcon> ringIcons = {{0, QIcon(":/png/beep_ok.png")}, {1, QIcon(":/png/beep_no.png")}};

    QMap<int, QIcon> fanIcons = {{0, QIcon(":/png/fans_ok.png")}, {1, QIcon(":/png/fans_no .png")}};

    QMap<int, QIcon> sr04Icons={{0,QIcon(":/png/water_0.png")},{1,QIcon(":/png/water_20.png")},{2,QIcon(":/png/water_50.png")},{3,QIcon(":/png/water_100.png")}};

    QMap<int, QIcon> humiIcons = {{0, QIcon(":/png/humi_ok.png")}, {1,QIcon(":/png/humi_no.png")}};


    QMap<int, QIcon> temIcons = {{0, QIcon(":/png/tem_ok.png")}, {1,QIcon(":/png/tem_no.png")}};

    controller.registerWidget("fan", ui->fansbtton, fanIcons);
    controller.registerWidget("ring", ui->ringbtton, ringIcons);
    controller.registerWidget("sr04", ui->watericon, sr04Icons);

    controller.registerWidget("humi_1", ui->humicon_1, humiIcons);
    controller.registerWidget("tem_1", ui->temicon_1, temIcons);

    controller.registerWidget("humi_2", ui->humicon_2, humiIcons);
    controller.registerWidget("tem_2", ui->temicon_2, temIcons);

    controller.registerWidget("humi_3", ui->humicon_3, humiIcons);
    controller.registerWidget("tem_3", ui->temicon_3, temIcons);


    QObject::connect(&controller, &IconController::updateIcon, [](const QString &widgetId, QIcon icon) {
        if (controller.widgets.contains(widgetId)) {
            QWidget *widget = controller.widgets[widgetId];
            if (QPushButton *button = qobject_cast<QPushButton*>(widget)) {
                button->setIcon(icon);
                button->setIconSize(button->size());
                button->update();
            } else if (QLabel *label = qobject_cast<QLabel*>(widget)) {
                QPixmap pixmap = icon.pixmap(label->size());
                label->setPixmap(pixmap);
                label->update();

            }
        }
    });



    setWindowTitle("planting systerm");

    QHostAddress local_ip = get_local_host_ip();

    QString tcp_port = "9998";

    ui->label_ip_addr->setText(local_ip.toString());

    ui->label_tcp_port->setText(tcp_port);

    tcp_thread=new Tcp_Thread(local_ip,tcp_port);

    tcp_thread->start();

    qDebug()<<local_ip.toString()<<"  "<<"tcp_port"<<tcp_port;
    connect(tcp_thread,&Tcp_Thread::signal_msg,this,&MainWindow::print_tcp_msg);


    m_nTimerId = startTimer(3000);

    dht11.pininit();
    sr04.pininit();
    wiringPiSetup();

    pinMode(25, OUTPUT);
    digitalWrite(25,HIGH);
    delay(500);
}

void MainWindow::timerEvent(QTimerEvent *event) {

    uchar buffer[5];
    if (dht11.dht11_read_data(buffer) == 0) {
        hum = buffer[0] + buffer[1] / 10.0;
        tem = buffer[2] + buffer[3] / 10.0;
        hum-=10;
        tem-=5;
        if(hum>100) hum=90;
        if(tem>50)  tem=40;
        if(hum<30) hum=20;
        if(tem<15)  tem=15;
    } else {
        qDebug() << "Failed to read from DHT11";
        dht11.pininit();
        return;
    }

    if(hum>80)
    {
        controller.triggerUpdate("humi_1",1);
        controller.triggerUpdate("humi_2",1);
        controller.triggerUpdate("humi_3",1);
        controller.triggerUpdate("ring",1);
        flag_ring=true;
        digitalWrite(25,LOW);
    }
    else
    {
        controller.triggerUpdate("humi_1",0);
        controller.triggerUpdate("humi_2",0);
        controller.triggerUpdate("humi_3",0);
        controller.triggerUpdate("ring",0);
        flag_ring=false;
        digitalWrite(25,HIGH);
    }
    if(tem>30)
    {
        controller.triggerUpdate("tem_1",1);
        controller.triggerUpdate("tem_2",1);
        controller.triggerUpdate("tem_3",1);
        controller.triggerUpdate("ring",1);
         controller.triggerUpdate("fan",1);
        flag_ring=true;
        flag_fan=true;
        digitalWrite(25,LOW);
    }
    else {
        controller.triggerUpdate("tem_1",0);
        controller.triggerUpdate("tem_2",0);
        controller.triggerUpdate("tem_3",0);
        controller.triggerUpdate("ring",0);
         controller.triggerUpdate("fan",0);
        flag_ring=false;
        flag_fan=false;
        digitalWrite(25,HIGH);
    }

    QString humtmp = QString::number(hum, 'f', 1);
    qDebug() << "Humidity:" << humtmp;
    ui->humi->setText(humtmp);
    QString temtmp = QString::number(tem, 'f', 1);
    qDebug() << "Temperature:" << temtmp;
    ui->temp->setText(temtmp);
    qDebug()<<humtmp.toUtf8();

    sr04.read_distance();

    if( sr04.dist>20)  sr04.dist=15;
    if( sr04.dist<0)  sr04.dist=0;




    QString sr04_data=QString::number(sr04.dist,'f',1);

    ui->distance->setText(sr04_data);


    // Retrieve the current distance value
    double distance = sr04.dist;

    // Check in which range the distance value falls and perform corresponding operations
    if (distance >= 0 && distance <= 2) {

        controller.triggerUpdate("ring",1);
        flag_ring=true;
         digitalWrite(25,LOW);

        controller.triggerUpdate("sr04", 0);  // Update icon for the range 0-3
    } else if (distance > 2 && distance <= 4) {
        controller.triggerUpdate("ring",1);
        flag_ring=true;
         digitalWrite(25,LOW);
    } else if (distance > 4 && distance <= 6) {
        controller.triggerUpdate("ring",0);
        flag_ring=false;
         digitalWrite(25,HIGH);

        controller.triggerUpdate("sr04", 2);
    } else if (distance > 6&& distance <= 15) {

        controller.triggerUpdate("ring",0);
        flag_ring=false;
         digitalWrite(25,HIGH);

        controller.triggerUpdate("sr04", 3);  // Update icon for the range 10-18
    }



    if(tcp_thread->connect_flag)
    {
       tcp_thread->socket->write(humtmp.toUtf8()+"&"+temtmp.toUtf8()+"&"+sr04_data.toUtf8());
       tcp_thread->socket->flush();
       qDebug()<<"success";
       tcp_thread->recv_flag = true;

    }

    ui->humi_2->setText(humtmp);
    ui->temp_2->setText(temtmp);

    ui->humi_3->setText(humtmp);
    ui->temp_3->setText(temtmp);


    pinMode(dht11_pin, OUTPUT);
    digitalWrite(dht11_pin, HIGH);
}

MainWindow::~MainWindow() {

    delete ui;

    if (m_nTimerId != 0) {
        killTimer(m_nTimerId);
    }
}


QHostAddress MainWindow::get_local_host_ip()
{
  QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
  QHostAddress result;
  foreach(QHostAddress address, AddressList){
      if(address.protocol() == QAbstractSocket::IPv4Protocol &&
         address != QHostAddress::Null &&
         address != QHostAddress::LocalHost){
          if (address.toString().contains("127.0.")){
            continue;
          }

          if(address.toString().contains("192.168.1.")){
            result = address;
            break;
          }
      }
  }
  return result;
}

void MainWindow::print_tcp_msg()
{
    if(!tcp_thread->msg_queue.isEmpty())
    {
//        qDebug()<<"rece:   ";

        QString msg=tcp_thread->msg_queue.dequeue();

        qDebug()<<msg;

        if(msg=="1")
        {
            controller.triggerUpdate("ring",1);
            controller.triggerUpdate("fan",1);
           flag_ring=true;
            flag_fan=true;
           digitalWrite(25,LOW);
        }
        else if(msg=="0")
        {
            controller.triggerUpdate("ring",0);
             controller.triggerUpdate("fan",0);
            flag_ring=false;
             flag_fan=false;
            digitalWrite(25,HIGH);
        }


    }
}

void MainWindow::on_ringbtton_clicked()
{



    if(flag_ring==false)
    {
        qDebug()<<"ringbutton false";
        controller.triggerUpdate("ring",1);
        flag_ring=true;
        flag_fan=true;
         digitalWrite(25,LOW);


    }
    else
    {
        qDebug()<<"ringbutton true";
        controller.triggerUpdate("ring",0);
        flag_ring=false;
        flag_fan=false;
        digitalWrite(25,HIGH);


    }
}


void MainWindow::on_fansbtton_clicked()
{
    if(flag_fan==false)
    {
        qDebug()<<"ringbutton false";
        controller.triggerUpdate("fan",1);
        flag_fan=true;



    }
    else
    {
        qDebug()<<"ringbutton true";
        controller.triggerUpdate("fan",0);
        flag_fan=false;


    }

}

