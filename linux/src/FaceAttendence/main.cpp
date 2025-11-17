#include "faceattendence.h"
#include "register.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FaceAttendence w;
//    Register reg;
//    reg.show();
    w.show();

    return a.exec();
}
