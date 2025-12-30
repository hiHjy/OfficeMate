/********************************************************************************
** Form generated from reading UI file 'widget_system.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_SYSTEM_H
#define UI_WIDGET_SYSTEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget_System
{
public:
    QWidget *widget;
    QWidget *widget1;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_7;
    QPushButton *btn_net;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_6;
    QPushButton *btn_poweroff;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QPushButton *btn_restart;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_8;
    QLabel *label_2;
    QWidget *widget_status;
    QWidget *widget2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label;
    QLabel *lb_net_status;
    QLabel *label_4;
    QLabel *lb_cpu_tem;
    QLabel *label_10;
    QLabel *lb_cpu_use;
    QLabel *label_12;
    QLabel *lb_sensor_tem;
    QLabel *label_14;
    QLabel *lb_sensor_hum;
    QLabel *label_16;
    QLabel *lb_sys_date;

    void setupUi(QWidget *Widget_System)
    {
        if (Widget_System->objectName().isEmpty())
            Widget_System->setObjectName(QString::fromUtf8("Widget_System"));
        Widget_System->resize(800, 480);
        Widget_System->setStyleSheet(QString::fromUtf8("QWidget #widget_status{\n"
"	background-color:;\n"
"	background-color: rgb(211, 215, 207);\n"
"	border: 1px solid black;\n"
"\n"
"}\n"
"\n"
"QWidget #Widget_System{\n"
"	background-color:;\n"
"\n"
"	background-color: white;\n"
"\n"
"}\n"
"\n"
"QWidget #widget{\n"
"	\n"
"		border: 5px solid  #696969;\n"
"		border-radius:10px 		\n"
"}\n"
"\n"
"\n"
""));
        widget = new QWidget(Widget_System);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(260, 120, 291, 271));
        widget->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border-radius: 10px;\n"
"    background-color: rgb(46, 52, 54);  /* \346\267\273\345\212\240\350\203\214\346\231\257\350\211\262 */\n"
"    color: white;              /* \346\226\207\345\255\227\351\242\234\350\211\262 */\n"
"}"));
        widget1 = new QWidget(widget);
        widget1->setObjectName(QString::fromUtf8("widget1"));
        widget1->setGeometry(QRect(9, 9, 281, 261));
        verticalLayout = new QVBoxLayout(widget1);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 30, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, -1, 0, -1);
        label_7 = new QLabel(widget1);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMaximumSize(QSize(50, 16777215));
        QFont font;
        font.setPointSize(16);
        label_7->setFont(font);
        label_7->setStyleSheet(QString::fromUtf8("image: url(:/global-network.png);"));

        horizontalLayout->addWidget(label_7);

        btn_net = new QPushButton(widget1);
        btn_net->setObjectName(QString::fromUtf8("btn_net"));
        btn_net->setMinimumSize(QSize(0, 50));

        horizontalLayout->addWidget(btn_net);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_6 = new QLabel(widget1);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMaximumSize(QSize(50, 16777215));
        label_6->setStyleSheet(QString::fromUtf8("image: url(:/power-on.png);"));

        horizontalLayout_2->addWidget(label_6);

        btn_poweroff = new QPushButton(widget1);
        btn_poweroff->setObjectName(QString::fromUtf8("btn_poweroff"));
        btn_poweroff->setMinimumSize(QSize(0, 50));

        horizontalLayout_2->addWidget(btn_poweroff);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_5 = new QLabel(widget1);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMaximumSize(QSize(50, 16777215));
        label_5->setStyleSheet(QString::fromUtf8("image: url(:/power.png);"));

        horizontalLayout_3->addWidget(label_5);

        btn_restart = new QPushButton(widget1);
        btn_restart->setObjectName(QString::fromUtf8("btn_restart"));
        btn_restart->setMinimumSize(QSize(0, 50));

        horizontalLayout_3->addWidget(btn_restart);


        verticalLayout->addLayout(horizontalLayout_3);

        layoutWidget = new QWidget(Widget_System);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(300, 20, 161, 51));
        horizontalLayout_4 = new QHBoxLayout(layoutWidget);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(layoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMaximumSize(QSize(100, 100));
        label_8->setStyleSheet(QString::fromUtf8("image: url(:/configuration.png);"));

        horizontalLayout_4->addWidget(label_8);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMaximumSize(QSize(100, 100));
        QFont font1;
        font1.setPointSize(25);
        font1.setBold(true);
        font1.setItalic(true);
        font1.setWeight(75);
        label_2->setFont(font1);

        horizontalLayout_4->addWidget(label_2);

        widget_status = new QWidget(Widget_System);
        widget_status->setObjectName(QString::fromUtf8("widget_status"));
        widget_status->setGeometry(QRect(-1, 414, 811, 41));
        widget_status->setMinimumSize(QSize(0, 15));
        widget_status->setStyleSheet(QString::fromUtf8("QLabel{\n"
"\n"
"font-size: 10pt;\n"
"}\n"
"background-color: rgb(85, 87, 83);"));
        widget2 = new QWidget(widget_status);
        widget2->setObjectName(QString::fromUtf8("widget2"));
        widget2->setGeometry(QRect(9, 9, 801, 22));
        horizontalLayout_5 = new QHBoxLayout(widget2);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(40, 0));
        label->setMaximumSize(QSize(33, 16777215));
        QFont font2;
        font2.setPointSize(10);
        label->setFont(font2);

        horizontalLayout_5->addWidget(label);

        lb_net_status = new QLabel(widget2);
        lb_net_status->setObjectName(QString::fromUtf8("lb_net_status"));
        lb_net_status->setFont(font2);

        horizontalLayout_5->addWidget(lb_net_status);

        label_4 = new QLabel(widget2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font2);

        horizontalLayout_5->addWidget(label_4);

        lb_cpu_tem = new QLabel(widget2);
        lb_cpu_tem->setObjectName(QString::fromUtf8("lb_cpu_tem"));
        lb_cpu_tem->setFont(font2);

        horizontalLayout_5->addWidget(lb_cpu_tem);

        label_10 = new QLabel(widget2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setFont(font2);

        horizontalLayout_5->addWidget(label_10);

        lb_cpu_use = new QLabel(widget2);
        lb_cpu_use->setObjectName(QString::fromUtf8("lb_cpu_use"));
        lb_cpu_use->setFont(font2);

        horizontalLayout_5->addWidget(lb_cpu_use);

        label_12 = new QLabel(widget2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setFont(font2);

        horizontalLayout_5->addWidget(label_12);

        lb_sensor_tem = new QLabel(widget2);
        lb_sensor_tem->setObjectName(QString::fromUtf8("lb_sensor_tem"));
        lb_sensor_tem->setFont(font2);

        horizontalLayout_5->addWidget(lb_sensor_tem);

        label_14 = new QLabel(widget2);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setFont(font2);

        horizontalLayout_5->addWidget(label_14);

        lb_sensor_hum = new QLabel(widget2);
        lb_sensor_hum->setObjectName(QString::fromUtf8("lb_sensor_hum"));
        lb_sensor_hum->setFont(font2);

        horizontalLayout_5->addWidget(lb_sensor_hum);

        label_16 = new QLabel(widget2);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setFont(font2);

        horizontalLayout_5->addWidget(label_16);

        lb_sys_date = new QLabel(widget2);
        lb_sys_date->setObjectName(QString::fromUtf8("lb_sys_date"));
        lb_sys_date->setFont(font2);
        lb_sys_date->setAutoFillBackground(false);

        horizontalLayout_5->addWidget(lb_sys_date);


        retranslateUi(Widget_System);

        QMetaObject::connectSlotsByName(Widget_System);
    } // setupUi

    void retranslateUi(QWidget *Widget_System)
    {
        Widget_System->setWindowTitle(QApplication::translate("Widget_System", "Form", nullptr));
        label_7->setText(QString());
        btn_net->setText(QApplication::translate("Widget_System", "\351\207\215\350\277\236\347\275\221\347\273\234", nullptr));
        label_6->setText(QString());
        btn_poweroff->setText(QApplication::translate("Widget_System", "\345\205\263\351\227\255\350\256\276\345\244\207", nullptr));
        label_5->setText(QString());
        btn_restart->setText(QApplication::translate("Widget_System", "\351\207\215\345\220\257\350\256\276\345\244\207", nullptr));
        label_8->setText(QString());
        label_2->setText(QApplication::translate("Widget_System", "\347\263\273\347\273\237", nullptr));
        label->setText(QApplication::translate("Widget_System", "\347\275\221\347\273\234:", nullptr));
        lb_net_status->setText(QApplication::translate("Widget_System", "\345\267\262\350\277\236\346\216\245", nullptr));
        label_4->setText(QApplication::translate("Widget_System", "CPU\346\270\251\345\272\246:", nullptr));
        lb_cpu_tem->setText(QApplication::translate("Widget_System", "90", nullptr));
        label_10->setText(QApplication::translate("Widget_System", "CPU\345\215\240\347\224\250:", nullptr));
        lb_cpu_use->setText(QApplication::translate("Widget_System", "90", nullptr));
        label_12->setText(QApplication::translate("Widget_System", "\347\216\257\345\242\203\346\270\251\345\272\246:", nullptr));
        lb_sensor_tem->setText(QApplication::translate("Widget_System", "90", nullptr));
        label_14->setText(QApplication::translate("Widget_System", "\347\216\257\345\242\203\346\271\277\345\272\246:", nullptr));
        lb_sensor_hum->setText(QApplication::translate("Widget_System", "90", nullptr));
        label_16->setText(QApplication::translate("Widget_System", "\347\263\273\347\273\237\346\227\266\351\227\264:", nullptr));
        lb_sys_date->setText(QApplication::translate("Widget_System", "00:00", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget_System: public Ui_Widget_System {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_SYSTEM_H
