#include "controller/VixApplication.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
#include <QVBoxLayout>
#include <iostream>
#include "boost/filesystem.hpp"
using namespace vix;
using namespace std;

VixApplication::VixApplication(int argc, char **argv):
    QApplication(argc, argv),
#ifdef __linux
    selectionModel_("/home/gfannes")
#else
    selectionModel_("h:/")
#endif
{
    QWidget *centralWidget = new QWidget(&mainWindow_); 
    QVBoxLayout *vbox = new QVBoxLayout(centralWidget);
    mainWindow_.setCentralWidget(centralWidget);
    selectionView_.setModel(&stringListModel_);
    tabBar_.addTab("Initializing");
    vbox->addWidget(&tabBar_);
    vbox->addWidget(&pathLabel_);
    vbox->addWidget(&selectionView_);
    vbox->addWidget(&commandLine_);
    mainWindow_.setVisible(true);

    connect(&selectionView_, SIGNAL(readableKeyPressed(QChar)), this, SLOT(process4Commandline(QChar)));
    connect(&selectionView_, SIGNAL(keycodePressed(int)), this, SLOT(process4Commandline(int)));
    selectionModelUpdatedConnection_ = selectionModel_.connect(boost::bind(&VixApplication::updateSelection_, this));

    updateSelection_();
}

void VixApplication::process4Commandline(QChar ch)
{
    LOG_S_(Debug, process4Commandline_QChar);
    int i = (int)ch.toAscii();
    LOG_M_(Debug, "Process key " << i);
    switch (i)
    {
        case 27:
            commandLine_.setText("");
            break;
        case 13:
            {
                boost::signals2::shared_connection_block block(selectionModelUpdatedConnection_);
                if (selectionModel_.gotoSelected())
                    commandLine_.setText("");
            }
            break;
        default:
            QString text = commandLine_.text() + ch;
            commandLine_.setText(text);
            break;
    }
    selectionModel_.setFilter(commandLine_.text().toStdString());
}
enum class KeyCode: int
{
    Left = 0x1000012,
    Right = 0x1000014,
    Up = 0x1000013,
    Down = 0x1000015
};
void VixApplication::process4Commandline(int keycode)
{
    LOG_SM_(Debug, process4Commandline_keycode, "Process keycode " << hex << keycode << dec);
    switch (keycode)
    {
        case (int)KeyCode::Up:
            selectionModel_.move(model::Selection::Direction::Up);
            return;
            break;
        case (int)KeyCode::Down:
            selectionModel_.move(model::Selection::Direction::Down);
            return;
            break;
        case (int)KeyCode::Left:
            {
                auto parent = selectionModel_.path()->location();
                if (parent)
                {
                    commandLine_.setText("");
                    boost::signals2::shared_connection_block block(selectionModelUpdatedConnection_);
                    selectionModel_.setPath(parent);
                }
            }
            break;
        case (int)KeyCode::Right:
            {
                boost::signals2::shared_connection_block block(selectionModelUpdatedConnection_);
                if (selectionModel_.gotoSelected())
                    commandLine_.setText("");
            }
            break;
        default:
            LOG_M_(Debug, "Doing nothing");
            return;
            break;
    }
    selectionModel_.setFilter(commandLine_.text().toStdString());
}

void VixApplication::setSelected(const QModelIndex &current, const QModelIndex &prev)
{
    LOG_S_(Debug, setSelected);
    auto selected = stringListModel_.data(current, Qt::DisplayRole).toString().toStdString();
    LOG_M_(Debug, "setSelected to " << selected);
    selectionModel_.setSelected(selected);
}

void VixApplication::updateSelection_()
{
    LOG_S_(Debug, VixApplication::updateSelection_);
    pathLabel_.setText(selectionModel_.path()->path().c_str());
    vix::model::Files files;
    int selectedIX;
    selectionModel_.getFiles(files, selectedIX);
    QStringList stringList;
    for (auto file = files.begin(); file != files.end(); ++file)
        stringList << (*file)->name().c_str();
    stringListModel_.setStringList(stringList);
    LOG_M_(Debug, "selectedIX: " << selectedIX);
    auto ix = stringListModel_.index(selectedIX);
    selectionView_.selectionModel()->select(ix, QItemSelectionModel::Select);
    selectionView_.scrollTo(ix, QAbstractItemView::EnsureVisible);
}
