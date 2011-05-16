#include "view/Selection.hpp"
#include <QKeyEvent>
#include <iostream>
using namespace vix::view;
using namespace std;

void Selection::keyPressEvent(QKeyEvent *ke)
{
    auto text = ke->text();
    if (!text.isEmpty())
    {
        cout << "Received the following key: " << text.toStdString() << endl;
        emit keyPress(text[0]);
        return;
    }
    QListView::keyPressEvent(ke);
}
