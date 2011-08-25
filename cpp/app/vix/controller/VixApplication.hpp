#ifndef controller_VixApplication_hpp
#define controller_VixApplication_hpp

#include "controller/Commander.hpp"
#include "model/Selection.hpp"
#include "view/Selection.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QStringListModel>
#include <QLabel>
#include <QTabBar>
#include <QLineEdit>
#include "boost/signals2.hpp"
#include <array>

namespace vix
{
    class VixApplication: public QApplication
    {
        Q_OBJECT

        public:
            VixApplication(int argc, char **argv);

        public slots:
            void process4Commandline(QChar);
            void process4Commandline(int, int);
            void changeCurrent(int);
            void setSelected(const QModelIndex &, const QModelIndex &);

        private:
            void updateSelection_(vix::model::Selection *);
            void updateCommander_(int, const std::string *);

            bool gotoSelection_();

            QMainWindow mainWindow_;
            QTabBar tabBar_;
            QLabel pathLabel_;
            vix::view::Selection selectionView_;
            typedef std::array<QLabel, 1> Labels;
            Labels labels_;
            QLineEdit filter_;
            QLineEdit content_;
            QLineEdit command_;

            QStringListModel stringListModel_;
            vix::model::Selections selectionModels_;
            vix::Commander commander_;
            boost::signals2::connection selectionModelsUpdatedConnection_;
            boost::signals2::connection commanderUpdatedConnection_;
    };
}

#endif
