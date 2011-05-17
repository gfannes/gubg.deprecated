/****************************************************************************
** Meta object code from reading C++ file 'Selection.hpp'
**
** Created: Tue May 17 22:25:42 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "/home/gfannes/gubg/cpp/app/vix/view/Selection.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Selection.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_vix__view__Selection[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,
      48,   21,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_vix__view__Selection[] = {
    "vix::view::Selection\0\0readableKeyPressed(QChar)\0"
    "keycodePressed(int)\0"
};

const QMetaObject vix::view::Selection::staticMetaObject = {
    { &QListView::staticMetaObject, qt_meta_stringdata_vix__view__Selection,
      qt_meta_data_vix__view__Selection, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &vix::view::Selection::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *vix::view::Selection::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *vix::view::Selection::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_vix__view__Selection))
        return static_cast<void*>(const_cast< Selection*>(this));
    return QListView::qt_metacast(_clname);
}

int vix::view::Selection::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: readableKeyPressed((*reinterpret_cast< QChar(*)>(_a[1]))); break;
        case 1: keycodePressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void vix::view::Selection::readableKeyPressed(QChar _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void vix::view::Selection::keycodePressed(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
