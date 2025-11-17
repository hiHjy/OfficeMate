#include "register.h"
#include "ui_register.h"
#include <QFileDialog>
#include <QtDebug>
#include <QMessageBox>
#include "face_api.h"
#include "databasemanager.h"
Register::Register(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    DatabaseManager::getInstance()->initDataBase();
}

Register::~Register()
{
    delete ui;
}

void Register::on_Btn_Clear_clicked()
{
    ui->Le_Name->clear();
    ui->Le_WorkID->clear();
    ui->Le_Identity->clear();
    ui->Le_Path->clear();

}

void Register::on_Btn_AddImg_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this);

    //qDebug() << filePath << endl;
    ui->Le_Path->setText(filePath);

    QPixmap mmp(filePath);
    mmp = mmp.scaled(384, 387);
    //    mmp = mmp.scaledToWidth(ui->Lb_Img->width());
    //    mmp = mmp.scaledToHeight(ui->Lb_Img->height());
    ui->Lb_Img->setPixmap(mmp);
}

void Register::on_Btn_Register_clicked()
{
    // 1. 获取用户输入
    QString name = ui->Le_Name->text().trimmed();
    QString workId = ui->Le_WorkID->text().trimmed();
    QString identity = ui->Le_Identity->text().trimmed();
    QString imgPath = ui->Le_Path->text().trimmed();

    qDebug() << name << endl;
    if (name.isEmpty() || workId.isEmpty() || identity.isEmpty() || imgPath.isEmpty()) {
        QMessageBox::warning(this, "提示", "信息填写不完整！");
        Register::on_Btn_Clear_clicked();
        return;
    }

    QFile file(imgPath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法读取图片文件！");
        return;
    }

    QByteArray imgData = file.readAll();
    QString imgBase64 = imgData.toBase64();
    getAccessToken([=](QString token){
         faceRegister(imgBase64, token, name, workId, identity, imgPath);

    });





}
