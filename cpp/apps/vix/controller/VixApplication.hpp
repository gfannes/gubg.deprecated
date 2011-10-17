#ifndef vix_controller_VixApplication_hpp
#define vix_controller_VixApplication_hpp

#ifdef VIX_WITH_UPP
#include "vix/controller/Commander.hpp"
#include "vix/model/Selection.hpp"
#include "CtrlLib/CtrlLib.h"
namespace Upp
{
#define LAYOUTFILE <vix/vix.lay>
#include <CtrlCore/lay.h>
}

class VixApplication: public Upp::WithMainLayout<Upp::TopWindow>
{
    public:
        typedef VixApplication CLASSNAME;
        VixApplication();
        virtual bool Key(Upp::dword key, int count);
        
        void sel();
        void doubleClick();
        void nothing(){}
    private:
        void updateSelectionSlot_(vix::model::ChangedItem);
        void updateSelection_(vix::model::ChangedItem);
        void updateCommanderSlot_(int, const std::string *);
        void updateCommander_(int, const std::string *);
                
    	vix::controller::Commander commander_;
    	vix::model::Selections selectionModels_;
        boost::signals2::connection selectionModelsUpdatedConnection_;    	
        boost::signals2::connection commanderUpdatedConnection_;
};
#endif

#ifdef VIX_WITH_QT
#include "controller/Commander.hpp"
#include "model/Selection.hpp"
#include "view/Selection.hpp"
#include <QApplication>
#include <QMainWindow>
#include <QStringListModel>
#include <QLabel>
#include <QTabBar>
#include <QLineEdit>
#include <QTextEdit>
#include "boost/signals2.hpp"
#include <array>

namespace vix
{
    class VixApplication: public QApplication
    {
        Q_OBJECT

        public:
            VixApplication(int argc, char **argv);

        signals:
            void updateSelectionSignal(vix::model::Selection *);

        public slots:
            void process4Commandline(QChar);
            void process4Commandline(int, int);
            void changeCurrent(int);
            void setSelected(const QModelIndex &, const QModelIndex &);
            void updateSelectionSlot(vix::model::Selection *);

        private:
            void updateSelection_(vix::model::Selection *);
            void updateCommander_(int, const std::string *);

            bool gotoSelection_();

            QMainWindow mainWindow_;
            QTabBar tabBar_;
            QLabel pathLabel_;
            vix::view::Selection selectionView_;
            QTextEdit textEdit_;
            QTextDocument textDocument_;
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
#endif
