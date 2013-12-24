/****************************************************************************
** Meta object code from reading C++ file 'sessionviewer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/dialogs/sessionviewer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sessionviewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SessionViewer_t {
    QByteArrayData data[16];
    char stringdata[214];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_SessionViewer_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_SessionViewer_t qt_meta_stringdata_SessionViewer = {
    {
QT_MOC_LITERAL(0, 0, 13),
QT_MOC_LITERAL(1, 14, 21),
QT_MOC_LITERAL(2, 36, 0),
QT_MOC_LITERAL(3, 37, 16),
QT_MOC_LITERAL(4, 54, 8),
QT_MOC_LITERAL(5, 63, 8),
QT_MOC_LITERAL(6, 72, 19),
QT_MOC_LITERAL(7, 92, 20),
QT_MOC_LITERAL(8, 113, 19),
QT_MOC_LITERAL(9, 133, 19),
QT_MOC_LITERAL(10, 153, 7),
QT_MOC_LITERAL(11, 161, 21),
QT_MOC_LITERAL(12, 183, 11),
QT_MOC_LITERAL(13, 195, 4),
QT_MOC_LITERAL(14, 200, 7),
QT_MOC_LITERAL(15, 208, 4)
    },
    "SessionViewer\0monthSelectionChanged\0"
    "\0QListWidgetItem*\0selected\0previous\0"
    "daySelectionChanged\0timeSelectionChanged\0"
    "loadSelectedSession\0timelineTimeUpdated\0"
    "newTime\0timelineMarkerPressed\0std::string\0"
    "type\0content\0time\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SessionViewer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    2,   44,    2, 0x08,
       6,    2,   49,    2, 0x08,
       7,    2,   54,    2, 0x08,
       8,    0,   59,    2, 0x08,
       9,    1,   60,    2, 0x08,
      11,    3,   63,    2, 0x08,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 3,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Float,   10,
    QMetaType::Void, 0x80000000 | 12, 0x80000000 | 12, QMetaType::Float,   13,   14,   15,

       0        // eod
};

void SessionViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SessionViewer *_t = static_cast<SessionViewer *>(_o);
        switch (_id) {
        case 0: _t->monthSelectionChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: _t->daySelectionChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 2: _t->timeSelectionChanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 3: _t->loadSelectedSession(); break;
        case 4: _t->timelineTimeUpdated((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 5: _t->timelineMarkerPressed((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< std::string(*)>(_a[2])),(*reinterpret_cast< float(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject SessionViewer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_SessionViewer.data,
      qt_meta_data_SessionViewer,  qt_static_metacall, 0, 0}
};


const QMetaObject *SessionViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SessionViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SessionViewer.stringdata))
        return static_cast<void*>(const_cast< SessionViewer*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int SessionViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
