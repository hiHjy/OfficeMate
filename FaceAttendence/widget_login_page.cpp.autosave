#include "widget_login_page.h"
#include "ui_widget_login_page.h"
#include "dialog_admin.h"
#include <QSettings>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#define CONFIG_PATH "/etc/faceattendence/admin.conf"
Widget_Login_Page::Widget_Login_Page(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_Login_Page)
{
    ui->setupUi(this);
}

Widget_Login_Page::~Widget_Login_Page()
{
    delete ui;
}

void Widget_Login_Page::on_pushButton_clicked()
{
    if (!QFile::exists(CONFIG_PATH)) {
        qWarning() << CONFIG_PATH << " not found";
        QMessageBox::critical(this, "错误", "配置文件缺失,请联系管理员");
        return;
    }
    if (ui->le_account->text().trimmed().isEmpty() || ui->le_passwd->text().isEmpty()) {
        QMessageBox::information(this, "提示", "输入不合法");
        ui->le_account->clear();
        ui->le_passwd->clear();
        ui->le_account->setFocus();
        return;
    }
    QSettings admin_conf(CONFIG_PATH, QSettings::IniFormat);

    QString user = admin_conf.value("admin/username").toString();
    QString passwd = admin_conf.value("admin/passwd").toString();
    if (user == ui->le_account->text().trimmed() && passwd == ui->le_passwd->text()) {
        Dialog_Admin page(this);
        ui->le_account->clear();
        ui->le_passwd->clear();
        this->hide();
        page.exec();
        this->show();
    } else {
        QMessageBox::information(this, "提示", "用户名或密码错误");
        ui->le_account->clear();
        ui->le_passwd->clear();
        ui->le_account->setFocus();
    }







}

