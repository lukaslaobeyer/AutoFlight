/****************************************************************************
** Meta object code from reading C++ file 'afmainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.1.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/afmainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'afmainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.1.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AFMainWindow_t {
    QByteArrayData data[27];
    char stringdata[470];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_AFMainWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_AFMainWindow_t qt_meta_stringdata_AFMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 12),
QT_MOC_LITERAL(1, 13, 22),
QT_MOC_LITERAL(2, 36, 0),
QT_MOC_LITERAL(3, 37, 10),
QT_MOC_LITERAL(4, 48, 2),
QT_MOC_LITERAL(5, 51, 25),
QT_MOC_LITERAL(6, 77, 5),
QT_MOC_LITERAL(7, 83, 30),
QT_MOC_LITERAL(8, 114, 16),
QT_MOC_LITERAL(9, 131, 2),
QT_MOC_LITERAL(10, 134, 20),
QT_MOC_LITERAL(11, 155, 12),
QT_MOC_LITERAL(12, 168, 23),
QT_MOC_LITERAL(13, 192, 36),
QT_MOC_LITERAL(14, 229, 15),
QT_MOC_LITERAL(15, 245, 17),
QT_MOC_LITERAL(16, 263, 23),
QT_MOC_LITERAL(17, 287, 19),
QT_MOC_LITERAL(18, 307, 1),
QT_MOC_LITERAL(19, 309, 22),
QT_MOC_LITERAL(20, 332, 9),
QT_MOC_LITERAL(21, 342, 7),
QT_MOC_LITERAL(22, 350, 19),
QT_MOC_LITERAL(23, 370, 25),
QT_MOC_LITERAL(24, 396, 15),
QT_MOC_LITERAL(25, 412, 35),
QT_MOC_LITERAL(26, 448, 20)
    },
    "AFMainWindow\0navdataAvailableSignal\0"
    "\0AFNavdata*\0nd\0videoFrameAvailableSignal\0"
    "frame\0controllerInputAvailableSignal\0"
    "ControllerInput*\0in\0connectionLostSignal\0"
    "hideMessages\0flatTrimActionTriggered\0"
    "calibrateMagnetometerActionTriggered\0"
    "showAboutDialog\0attemptConnection\0"
    "showControlConfigDialog\0videoFrameAvailable\0"
    "f\0clearConfirmationFlags\0toggleHUD\0"
    "showHUD\0launchAutoScriptIDE\0"
    "launchSessionViewerDialog\0launch3DMapView\0"
    "launchImageProcessingPipelineEditor\0"
    "handleConnectionLost\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AFMainWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  104,    2, 0x05,
       5,    1,  107,    2, 0x05,
       7,    1,  110,    2, 0x05,
      10,    0,  113,    2, 0x05,

 // slots: name, argc, parameters, tag, flags
      11,    0,  114,    2, 0x0a,
      12,    0,  115,    2, 0x0a,
      13,    0,  116,    2, 0x0a,
      14,    0,  117,    2, 0x0a,
      15,    0,  118,    2, 0x08,
      16,    0,  119,    2, 0x08,
      17,    1,  120,    2, 0x08,
      19,    0,  123,    2, 0x08,
      20,    1,  124,    2, 0x08,
      22,    0,  127,    2, 0x08,
      23,    0,  128,    2, 0x08,
      24,    0,  129,    2, 0x08,
      25,    0,  130,    2, 0x08,
      26,    0,  131,    2, 0x08,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, QMetaType::QImage,    6,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage,   18,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   21,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void AFMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AFMainWindow *_t = static_cast<AFMainWindow *>(_o);
        switch (_id) {
        case 0: _t->navdataAvailableSignal((*reinterpret_cast< AFNavdata*(*)>(_a[1]))); break;
        case 1: _t->videoFrameAvailableSignal((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        case 2: _t->controllerInputAvailableSignal((*reinterpret_cast< ControllerInput*(*)>(_a[1]))); break;
        case 3: _t->connectionLostSignal(); break;
        case 4: _t->hideMessages(); break;
        case 5: _t->flatTrimActionTriggered(); break;
        case 6: _t->calibrateMagnetometerActionTriggered(); break;
        case 7: _t->showAboutDialog(); break;
        case 8: _t->attemptConnection(); break;
        case 9: _t->showControlConfigDialog(); break;
        case 10: _t->videoFrameAvailable((*reinterpret_cast< QImage(*)>(_a[1]))); break;
        case 11: _t->clearConfirmationFlags(); break;
        case 12: _t->toggleHUD((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->launchAutoScriptIDE(); break;
        case 14: _t->launchSessionViewerDialog(); break;
        case 15: _t->launch3DMapView(); break;
        case 16: _t->launchImageProcessingPipelineEditor(); break;
        case 17: _t->handleConnectionLost(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AFMainWindow::*_t)(AFNavdata * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AFMainWindow::navdataAvailableSignal)) {
                *result = 0;
            }
        }
        {
            typedef void (AFMainWindow::*_t)(QImage );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AFMainWindow::videoFrameAvailableSignal)) {
                *result = 1;
            }
        }
        {
            typedef void (AFMainWindow::*_t)(ControllerInput * );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AFMainWindow::controllerInputAvailableSignal)) {
                *result = 2;
            }
        }
        {
            typedef void (AFMainWindow::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AFMainWindow::connectionLostSignal)) {
                *result = 3;
            }
        }
    }
}

const QMetaObject AFMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_AFMainWindow.data,
      qt_meta_data_AFMainWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *AFMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AFMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AFMainWindow.stringdata))
        return static_cast<void*>(const_cast< AFMainWindow*>(this));
    if (!strcmp(_clname, "INavdataListener"))
        return static_cast< INavdataListener*>(const_cast< AFMainWindow*>(this));
    if (!strcmp(_clname, "IConnectionStatusListener"))
        return static_cast< IConnectionStatusListener*>(const_cast< AFMainWindow*>(this));
    if (!strcmp(_clname, "IVideoListener"))
        return static_cast< IVideoListener*>(const_cast< AFMainWindow*>(this));
    if (!strcmp(_clname, "IControllerInputListener"))
        return static_cast< IControllerInputListener*>(const_cast< AFMainWindow*>(this));
    if (!strcmp(_clname, "QVideoListener"))
        return static_cast< QVideoListener*>(const_cast< AFMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int AFMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void AFMainWindow::navdataAvailableSignal(AFNavdata * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AFMainWindow::videoFrameAvailableSignal(QImage _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AFMainWindow::controllerInputAvailableSignal(ControllerInput * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void AFMainWindow::connectionLostSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
