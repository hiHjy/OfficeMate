QT       += core gui multimedia multimediawidgets sql qml quickwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#添加opencv，seetaface头文件
INCLUDEPATH += /opt/opencv4.5.2-arm/include
INCLUDEPATH += /opt/opencv4.5.2-arm/include/opencv4/opencv2
INCLUDEPATH += /opt/opencv4.5.2-arm/include/opencv4

#添加opencv，seetace的库
LIBS += -L/opt/opencv4.5.2-arm/lib -lopencv_world
SOURCES += \
    databasemannager.cpp \
    dialog_admin.cpp \
    face_api.cpp \
    form.cpp \
    main.cpp \
    faceattendence.cpp \
    netonlinechecker.cpp \
    record.cpp \
    register.cpp \
    widget_login_page.cpp \
    widget_system.cpp

HEADERS += \
    databasemannager.h \
    dialog_admin.h \
    face_api.h \
    faceattendence.h \
    form.h \
    netonlinechecker.h \
    record.h \
    register.h \
    widget_login_page.h \
    widget_system.h


FORMS += \
    dialog_admin.ui \
    faceattendence.ui \
    form.ui \
    record.ui \
    register.ui \
    widget_login_page.ui \
    widget_system.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

DISTFILES +=
