#ifndef WIDGET_SYSTEM_H
#define WIDGET_SYSTEM_H

#include <QWidget>
class NetOnlineChecker;
namespace Ui {
class Widget_System;
}

class Widget_System : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_System(QWidget *parent = nullptr);

    ~Widget_System();

private slots:

    void on_btn_poweroff_clicked();

    void on_btn_restart_clicked();
    void netStatusUpdate(bool ok);

private:
    Ui::Widget_System *ui;
    NetOnlineChecker *netChecker;
};

#endif // WIDGET_SYSTEM_H
