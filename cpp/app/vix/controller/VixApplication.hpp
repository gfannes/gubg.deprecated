#ifndef controller_VixApplication_hpp
#define controller_VixApplication_hpp

#include "model/Selection.hpp"
#include "view/Selection.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QStringListModel>
#include <QLabel>
#include <QLineEdit>

namespace vix
{
    class VixApplication: public QApplication
    {
        Q_OBJECT

        public:
            VixApplication(int argc, char **argv);

        public slots:
            void process4Commandline(QChar);

        private:
            void updateSelection_();

            QMainWindow mainWindow_;
            QStringListModel stringListModel_;
            QLabel path_;
            QLineEdit commandLine_;
            vix::model::Selection selectionModel_;
            vix::view::Selection selectionView_;
    };
}

#endif
