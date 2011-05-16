/****************************************************************************
** Meta object code from reading C++ file 'Selection.hpp'
**
** Created: Mon May 16 23:09:01 2011
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
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_vix__view__Selection[] = {
    "vix::view::Selection\0\0keyPress(QChar)\0"
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
        case 0: keyPress((*reinterpret_cast< QChar(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void vix::view::Selection::keyPress(QChar _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
