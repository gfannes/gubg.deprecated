#ifndef vix_view_Selection_hpp
#define vix_view_Selection_hpp

#include <QListView>

namespace vix
{
    namespace view
    {
        class Selection: public QListView
        {
            Q_OBJECT

            signals:
                void keyPress(QChar);

            protected:
                void keyPressEvent(QKeyEvent *);
        };
    }
}

#endif
