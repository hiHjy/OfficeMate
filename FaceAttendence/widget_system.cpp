#include "widget_system.h"
#include "ui_widget_system.h"
#include <QProcess>
#include "netonlinechecker.h"
#include <QDebug>
Widget_System::Widget_System(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_System)
{
    ui->setupUi(this);
    netChecker = new NetOnlineChecker(this);
    connect(netChecker, &NetOnlineChecker::netStatusChanged, this, &Widget_System::netStatusUpdate);
}

Widget_System::~Widget_System()
{
    delete ui;
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
