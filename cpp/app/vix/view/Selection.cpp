#include "view/Selection.hpp"
#include <QKeyEvent>
#include <iostream>
using namespace vix::view;
using namespace std;

bool Selection::selectedIX(int &ix) const
{
    auto ixs = selectedIndexes();
    switch (ixs.size())
    {
        case 0:
            return false;
            break;
        case 1:
            ix = ixs[0].row();
            return true;
            break;
        default:
            throw "There can be at most one item selected";
            break;
    }
    return false;
}

void Selection::keyPressEvent(QKeyEvent *ke)
{
    auto text = ke->text();
    if (!text.isEmpty())
    {
        cout << "Received the following readable key: " << text.toStdString() << endl;
        emit readableKeyPressed(text[0]);
        return;
    }
    int keycode = ke->key();
    cout << "Received the following key: " << hex << keycode << dec << endl;
    emit keycodePressed(keycode);
    QListView::keyPressEvent(ke);
}
