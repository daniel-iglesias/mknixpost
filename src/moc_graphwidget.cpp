/****************************************************************************
** Meta object code from reading C++ file 'graphwidget.h'
**
** Created: Mon Dec 26 17:42:52 2011
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "graphwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'graphwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GraphWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      24,   12,   12,   12, 0x08,
      35,   12,   12,   12, 0x08,
      55,   12,   12,   12, 0x08,
      75,   12,   12,   12, 0x08,
      99,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GraphWidget[] = {
    "GraphWidget\0\0newGraph()\0addGraph()\0"
    "updateXNameBox(int)\0updateYNameBox(int)\0"
    "updateXDataBox(QString)\0updateYDataBox(QString)\0"
};

void GraphWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        GraphWidget *_t = static_cast<GraphWidget *>(_o);
        switch (_id) {
        case 0: _t->newGraph(); break;
        case 1: _t->addGraph(); break;
        case 2: _t->updateXNameBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->updateYNameBox((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->updateXDataBox((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 5: _t->updateYDataBox((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData GraphWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject GraphWidget::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_GraphWidget,
      qt_meta_data_GraphWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GraphWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GraphWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GraphWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GraphWidget))
        return static_cast<void*>(const_cast< GraphWidget*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int GraphWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
