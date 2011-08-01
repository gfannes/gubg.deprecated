#include "view/Selection.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
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
    LOG_S_(Debug, keyPressEvent);
    auto text = ke->text();
    int modifiers = ke->modifiers();
    LOG_M_(Debug, "Received the following modifiers: " << hex << modifiers << " " << (modifiers & (Qt::ControlModifier | Qt::AltModifier)) << dec);
    if (!text.isEmpty() && (0 == (modifiers & (Qt::ControlModifier | Qt::AltModifier))))
    {
        LOG_M_(Debug, "Received the following readable key: " << text.toStdString());
        emit readableKeyPressed(text[0]);
        return;
    }
    int keycode = ke->key();
    LOG_M_(Debug, "Received the following key: " << hex << keycode << dec);
    emit keycodePressed(keycode, modifiers);
}
