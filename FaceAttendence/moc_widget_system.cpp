/****************************************************************************
** Meta object code from reading C++ file 'widget_system.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "widget_system.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget_system.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Widget_System_t {
    QByteArrayData data[11];
    char stringdata0[129];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Widget_System_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Widget_System_t qt_meta_stringdata_Widget_System = {
    {
QT_MOC_LITERAL(0, 0, 13), // "Widget_System"
QT_MOC_LITERAL(1, 14, 12), // "updateStatus"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "Date"
QT_MOC_LITERAL(4, 33, 4), // "temp"
QT_MOC_LITERAL(5, 38, 23), // "on_btn_poweroff_clicked"
QT_MOC_LITERAL(6, 62, 22), // "on_btn_restart_clicked"
QT_MOC_LITERAL(7, 85, 15), // "netStatusUpdate"
QT_MOC_LITERAL(8, 101, 2), // "ok"
QT_MOC_LITERAL(9, 104, 15), // "envStatusUpdate"
QT_MOC_LITERAL(10, 120, 8) // "humidity"

    },
    "Widget_System\0updateStatus\0\0Date\0temp\0"
    "on_btn_poweroff_clicked\0on_btn_restart_clicked\0"
    "netStatusUpdate\0ok\0envStatusUpdate\0"
    "humidity"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Widget_System[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x08 /* Private */,
       5,    0,   44,    2, 0x08 /* Private */,
       6,    0,   45,    2, 0x08 /* Private */,
       7,    1,   46,    2, 0x08 /* Private */,
       9,    2,   49,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    4,   10,

       0        // eod
};

void Widget_System::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Widget_System *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->updateStatus((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: _t->on_btn_poweroff_clicked(); break;
        case 2: _t->on_btn_restart_clicked(); break;
        case 3: _t->netStatusUpdate((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->envStatusUpdate((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Widget_System::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_Widget_System.data,
    qt_meta_data_Widget_System,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Widget_System::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget_System::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Widget_System.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Widget_System::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
