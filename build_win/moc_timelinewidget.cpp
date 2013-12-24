/****************************************************************************
** Meta object code from reading C++ file 'timelinewidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/widgets/timelinewidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timelinewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_TimelineWidget_t {
    QByteArrayData data[9];
    char stringdata[85];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_TimelineWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_TimelineWidget_t qt_meta_stringdata_TimelineWidget = {
    {
QT_MOC_LITERAL(0, 0, 14),
QT_MOC_LITERAL(1, 15, 13),
QT_MOC_LITERAL(2, 29, 0),
QT_MOC_LITERAL(3, 30, 11),
QT_MOC_LITERAL(4, 42, 4),
QT_MOC_LITERAL(5, 47, 7),
QT_MOC_LITERAL(6, 55, 4),
QT_MOC_LITERAL(7, 60, 15),
QT_MOC_LITERAL(8, 76, 7)
    },
    "TimelineWidget\0markerPressed\0\0std::string\0"
    "type\0content\0time\0newTimeSelected\0"
    "newTime\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TimelineWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x05,
       7,    1,   31,    2, 0x05,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3, QMetaType::Float,    4,    5,    6,
    QMetaType::Void, QMetaType::Float,    8,

       0        // eod
};

void TimelineWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        TimelineWidget *_t = static_cast<TimelineWidget *>(_o);
        switch (_id) {
        case 0: _t->markerPressed((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< std::string(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        case 1: _t->newTimeSelected((*reinterpret_cast< float(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (TimelineWidget::*_t)(std::string , std::string , float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimelineWidget::markerPressed)) {
                *result = 0;
            }
        }
        {
            typedef void (TimelineWidget::*_t)(float );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&TimelineWidget::newTimeSelected)) {
                *result = 1;
            }
        }
    }
}

const QMetaObject TimelineWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TimelineWidget.data,
      qt_meta_data_TimelineWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *TimelineWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TimelineWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimelineWidget.stringdata))
        return static_cast<void*>(const_cast< TimelineWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int TimelineWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void TimelineWidget::markerPressed(std::string _t1, std::string _t2, float _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TimelineWidget::newTimeSelected(float _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
