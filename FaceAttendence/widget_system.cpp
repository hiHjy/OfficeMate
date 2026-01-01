#include "widget_system.h"
#include "ui_widget_system.h"
#include <QProcess>
#include "netonlinechecker.h"
#include <QDebug>
#include "systemmonitor.h"
#include <envdecter.h>
#include <QThread>

#define PERIOD 3000
Widget_System::Widget_System(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_System)
{
    ui->setupUi(this);
    netChecker = new NetOnlineChecker(this);
    monitor = new SystemMonitor(this);


    connect(netChecker, &NetOnlineChecker::netStatusChanged, this, &Widget_System::netStatusUpdate);
    connect(monitor, &SystemMonitor::updated, this, &Widget_System::updateStatus);
    //connect(envDector, &EnvDecter::envStatusUpdated, this, &Widget_System::envStatusUpdate);
    //envDector = new EnvDecter();

    /* 将读取温度移动到线程去执行 */


    QThread *t = new QThread(this);

    //定时器依赖时间循环，moveToThread默认带事件循环，定时器在新的线程生效
    QTimer *timer = new QTimer();
    timer->setInterval(PERIOD);
    timer->moveToThread(t);

    //移动到新线程的函数的槽函数会在新的线程执行
    EnvDecter *decter = new EnvDecter(); //移动到新线程的对象不能有父亲否则会导致移动到新线程失败
    decter->moveToThread(t);


    //执行新的线程中的EnvDecter的槽函数
    connect(timer, &QTimer::timeout, decter, &EnvDecter::readEnvStatus);

    //QvOerload<>::of(&QTimer::start) 当要用的槽函数有重载的时候就用这个QvOerload<重载的参数列表或无>::of(&类名::槽函数名)
    connect(t, &QThread::started, timer, QOverload<>::of(&QTimer::start));
    connect(decter, &EnvDecter::envStatusUpdated, this, &Widget_System::envStatusUpdate);



    //由于移动到新线程的对象不能有父亲，所以不会被自动回收，所以必须连接个信号与槽去回收对象
    connect(t, &QThread::finished, timer, &QTimer::deleteLater);
    connect(t, &QThread::finished, decter, &EnvDecter::deleteLater);
    t->start();

}

Widget_System::~Widget_System()
{
    delete ui;
}

void Widget_System::updateStatus(QString Date, QString temp)
{
    ui->lb_cpu_tem->setText(temp);
    ui->lb_sys_date->setText(Date);
}

void Widget_System::on_btn_poweroff_clicked()
{
    QProcess::startDetached("poweroff");
}

void Widget_System::on_btn_restart_clicked()
{
    QProcess::startDetached("reboot");
}

void Widget_System::netStatusUpdate(bool ok)
{
     qDebug() << "net ok:" << ok;
     if (ok) {
         ui->lb_net_status->setStyleSheet("color:green;");
         ui->lb_net_status->setText("已连接");
     } else {
         ui->lb_net_status->setStyleSheet("color:red;");
         ui->lb_net_status->setText("异常,请联系管理员");
     }
}

void Widget_System::envStatusUpdate(QString temp, QString humidity)
{
       ui->lb_sensor_hum->setText(humidity);
       ui->lb_sensor_tem->setText(temp);
}
