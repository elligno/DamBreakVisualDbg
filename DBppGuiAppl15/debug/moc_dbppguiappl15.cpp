/****************************************************************************
** Meta object code from reading C++ file 'dbppguiappl15.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../dbppguiappl15.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dbppguiappl15.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DBppGuiAppl15_t {
    QByteArrayData data[16];
    char stringdata0[195];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DBppGuiAppl15_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DBppGuiAppl15_t qt_meta_stringdata_DBppGuiAppl15 = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DBppGuiAppl15"
QT_MOC_LITERAL(1, 14, 7), // "operate"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 13), // "runSimulation"
QT_MOC_LITERAL(4, 37, 13), // "runStepbyStep"
QT_MOC_LITERAL(5, 51, 7), // "initSim"
QT_MOC_LITERAL(6, 59, 18), // "setNbIterationsMax"
QT_MOC_LITERAL(7, 78, 21), // "setInitialWaveProfile"
QT_MOC_LITERAL(8, 100, 11), // "setUpstream"
QT_MOC_LITERAL(9, 112, 13), // "setDownstream"
QT_MOC_LITERAL(10, 126, 11), // "setShockLoc"
QT_MOC_LITERAL(11, 138, 15), // "setDamBreakData"
QT_MOC_LITERAL(12, 154, 9), // "aDisrData"
QT_MOC_LITERAL(13, 164, 5), // "about"
QT_MOC_LITERAL(14, 170, 13), // "handleResults"
QT_MOC_LITERAL(15, 184, 10) // "aMsg2Write"

    },
    "DBppGuiAppl15\0operate\0\0runSimulation\0"
    "runStepbyStep\0initSim\0setNbIterationsMax\0"
    "setInitialWaveProfile\0setUpstream\0"
    "setDownstream\0setShockLoc\0setDamBreakData\0"
    "aDisrData\0about\0handleResults\0aMsg2Write"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DBppGuiAppl15[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   77,    2, 0x0a /* Public */,
       4,    0,   78,    2, 0x0a /* Public */,
       5,    0,   79,    2, 0x0a /* Public */,
       6,    0,   80,    2, 0x0a /* Public */,
       7,    0,   81,    2, 0x0a /* Public */,
       8,    0,   82,    2, 0x0a /* Public */,
       9,    0,   83,    2, 0x0a /* Public */,
      10,    0,   84,    2, 0x0a /* Public */,
      11,    1,   85,    2, 0x0a /* Public */,
      13,    0,   88,    2, 0x0a /* Public */,
      14,    1,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,

       0        // eod
};

void DBppGuiAppl15::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DBppGuiAppl15 *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->operate((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->runSimulation(); break;
        case 2: _t->runStepbyStep(); break;
        case 3: _t->initSim(); break;
        case 4: _t->setNbIterationsMax(); break;
        case 5: _t->setInitialWaveProfile(); break;
        case 6: _t->setUpstream(); break;
        case 7: _t->setDownstream(); break;
        case 8: _t->setShockLoc(); break;
        case 9: _t->setDamBreakData((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->about(); break;
        case 11: _t->handleResults((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DBppGuiAppl15::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DBppGuiAppl15::operate)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DBppGuiAppl15::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_DBppGuiAppl15.data,
    qt_meta_data_DBppGuiAppl15,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DBppGuiAppl15::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DBppGuiAppl15::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DBppGuiAppl15.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int DBppGuiAppl15::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void DBppGuiAppl15::operate(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
