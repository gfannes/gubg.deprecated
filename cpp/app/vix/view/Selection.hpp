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

            public:
                bool selectedIX(int &) const;

            signals:
                void readableKeyPressed(QChar);
                void keycodePressed(int);
                virtual void currentChanged(const QModelIndex &, const QModelIndex &);

            protected:
                void keyPressEvent(QKeyEvent *);
        };
    }
}

#endif
