#include "controller/VixApplication.hpp"
#include <QVBoxLayout>
#include <iostream>
#include "boost/filesystem.hpp"
using namespace vix;
using namespace std;

namespace
{
    struct Scoper
    {
        Scoper(const string &str):
            str_(str)
        {
            cout << "Begin::" << str_ << endl; 
        }
        ~Scoper()
        {
            cout << "End::" << str_ << endl;
        }
        string str_;
    };
}
#define SCOPE(msg) Scoper local_scoper(msg)

VixApplication::VixApplication(int argc, char **argv):
    QApplication(argc, argv),
    selectionModel_("/home/gfannes")
{
    QWidget *centralWidget = new QWidget(&mainWindow_); 
    QVBoxLayout *vbox = new QVBoxLayout(centralWidget);
    mainWindow_.setCentralWidget(centralWidget);
    selectionView_.setModel(&stringListModel_);
    vbox->addWidget(&pathLabel_);
    vbox->addWidget(&selectionView_);
    vbox->addWidget(&commandLine_);
    mainWindow_.setVisible(true);

    connect(&selectionView_, SIGNAL(readableKeyPressed(QChar)), this, SLOT(process4Commandline(QChar)));
    connect(&selectionView_, SIGNAL(keycodePressed(int)), this, SLOT(process4Commandline(int)));
    selectionModelUpdatedConnection_ = selectionModel_.connect(boost::bind(&VixApplication::updateSelection_, this));
    connect(&selectionView_, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(setSelected(const QModelIndex &, const QModelIndex &)));

    updateSelection_();
}

void VixApplication::process4Commandline(QChar ch)
{
    SCOPE("process4Commandline(QChar)");
    int i = (int)ch.toAscii();
    cout << "Process key " << i << endl;
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
enum class KeyCode
{
    Left = 0x1000012,
    Right = 0x1000014,
    Up = 0x1000013,
    Down = 0x1000015
};
void VixApplication::process4Commandline(int keycode)
{
    SCOPE("process4Commandline(keycode)");
    cout << "Process keycode " << hex << keycode << dec << endl;
    switch (keycode)
    {
        case KeyCode::Left:
            {
                auto p = boost::filesystem::path(selectionModel_.path());
                if (p.has_parent_path())
                {
                    commandLine_.setText("");
                    boost::signals2::shared_connection_block block(selectionModelUpdatedConnection_);
                    selectionModel_.setPath(p.parent_path().string());
                }
            }
            break;
        case KeyCode::Right:
            {
                boost::signals2::shared_connection_block block(selectionModelUpdatedConnection_);
                if (selectionModel_.gotoSelected())
                    commandLine_.setText("");
            }
            break;
        default:
            cout << "Doing nothing" << endl;
            return;
            break;
    }
    selectionModel_.setFilter(commandLine_.text().toStdString());
}

void VixApplication::setSelected(const QModelIndex &current, const QModelIndex &prev)
{
    SCOPE("setSelected()");
    auto selected = stringListModel_.data(current, Qt::DisplayRole).toString().toStdString();
    cout << "setSelected to " << selected << endl;
    selectionModel_.setSelected(selected);
}

void VixApplication::updateSelection_()
{
    SCOPE("updateSelection_()");
    pathLabel_.setText(selectionModel_.path().c_str());
    vix::model::Files files;
    int selectedIX;
    selectionModel_.getFiles(files, selectedIX);
    QStringList stringList;
    for (auto file = files.begin(); file != files.end(); ++file)
        stringList << file->name.c_str();
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        stringListModel_.setStringList(stringList);
    }
    cout << "selectedIX: " << selectedIX << endl;
    auto ix = stringListModel_.index(selectedIX);
    selectionView_.selectionModel()->select(ix, QItemSelectionModel::Select);
}
