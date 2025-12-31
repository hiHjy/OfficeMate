/********************************************************************************
** Form generated from reading UI file 'dialog_admin.ui'
**
** Created by: Qt User Interface Compiler version 5.12.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_ADMIN_H
#define UI_DIALOG_ADMIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <record.h>
#include <register.h>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tab_record;
    Register *tab_register;
    Record *tab_record_2;
    QPushButton *btn_exit;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(800, 480);
        Dialog->setStyleSheet(QString::fromUtf8(""));
        layoutWidget = new QWidget(Dialog);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 821, 481));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        tab_record = new QTabWidget(layoutWidget);
        tab_record->setObjectName(QString::fromUtf8("tab_record"));
        tab_register = new Register();
        tab_register->setObjectName(QString::fromUtf8("tab_register"));
        tab_record->addTab(tab_register, QString());
        tab_record_2 = new Record();
        tab_record_2->setObjectName(QString::fromUtf8("tab_record_2"));
        tab_record->addTab(tab_record_2, QString());

        verticalLayout->addWidget(tab_record);

        btn_exit = new QPushButton(layoutWidget);
        btn_exit->setObjectName(QString::fromUtf8("btn_exit"));
        btn_exit->setMinimumSize(QSize(80, 30));
        btn_exit->setMaximumSize(QSize(70, 30));

        verticalLayout->addWidget(btn_exit, 0, Qt::AlignHCenter|Qt::AlignVCenter);


        retranslateUi(Dialog);

        tab_record->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
        tab_record->setTabText(tab_record->indexOf(tab_register), QApplication::translate("Dialog", "\346\263\250\345\206\214", nullptr));
        tab_record->setTabText(tab_record->indexOf(tab_record_2), QApplication::translate("Dialog", "\350\256\260\345\275\225", nullptr));
        btn_exit->setText(QApplication::translate("Dialog", "\351\200\200\345\207\272\347\256\241\347\220\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_ADMIN_H
