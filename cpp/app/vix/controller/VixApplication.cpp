#include "controller/VixApplication.hpp"
#include "controller/Commander.hpp"
#define LOG_LEVEL Debug
#include "logging/Log.hpp"
#include <QVBoxLayout>
#include <iostream>
#include "boost/filesystem.hpp"
using namespace vix;
using namespace std;

VixApplication::VixApplication(int argc, char **argv):
    QApplication(argc, argv),
    commander_(selectionModels_)
{
    LOG_S_(Debug, VixApplication::VixApplication);
    QWidget *centralWidget = new QWidget(&mainWindow_); 
    QVBoxLayout *vbox = new QVBoxLayout(centralWidget);
    mainWindow_.setCentralWidget(centralWidget);
    selectionView_.setModel(&stringListModel_);
    tabBar_.setFocusPolicy(Qt::NoFocus);
    vbox->addWidget(&tabBar_);
    vbox->addWidget(&pathLabel_);
    vbox->addWidget(&selectionView_);
    vbox->addWidget(&commandLine_);
    mainWindow_.setVisible(true);

    connect(&selectionView_, SIGNAL(readableKeyPressed(QChar)), this, SLOT(process4Commandline(QChar)));
    connect(&selectionView_, SIGNAL(keycodePressed(int)), this, SLOT(process4Commandline(int)));
    connect(&tabBar_, SIGNAL(currentChanged(int)), this, SLOT(changeCurrent(int)));
    selectionModelsUpdatedConnection_ = selectionModels_.connect(boost::bind(&VixApplication::updateSelection_, this, _1));

#ifdef __linux
    const string path("/home/gfannes");
#else
    const string path("h:/");
#endif
    selectionModels_.addSelection(path);
}

void VixApplication::process4Commandline(QChar ch)
{
    LOG_S_(Debug, process4Commandline_QChar);
    int i = (int)ch.toAscii();
    LOG_M_(Debug, "Process key " << i);
    {
        switch (i)
        {
            case 27://Escape
                commander_.clear();
                break;
            case 13://Enter
                commander_.activate(Commander::Key::Enter);
                break;
            default:
                commander_.add(ch.toAscii());
                break;
        }
    }
    auto text = commander_.text();
    commandLine_.setText(QString(text.c_str()));
    if (commander_.isFilter())
        selectionModels_.current()->setFilter(text);
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
    if (selectionModels_.empty())
        return;

    model::Selection &selection = *selectionModels_.current();
    {
        switch (keycode)
        {
            case (int)KeyCode::Up:
                selection.move(model::Selection::Direction::Up);
                return;
                break;
            case (int)KeyCode::Down:
                selection.move(model::Selection::Direction::Down);
                return;
                break;
            case (int)KeyCode::Left:
                {
                    auto parent = vix::model::Path::Unlock(selection.path())->location();
                    if (parent)
                    {
                        commander_.clear();
                        boost::signals2::shared_connection_block block(selectionModelsUpdatedConnection_);
                        selection.setPath(parent);
                    }
                }
                break;
            case (int)KeyCode::Right:
                {
                    boost::signals2::shared_connection_block block(selectionModelsUpdatedConnection_);
                    commander_.activate(Commander::Key::Arrow);
                }
                break;
            default:
                LOG_M_(Debug, "Doing nothing");
                return;
                break;
        }
    }
    selection.setFilter(commander_.text());
}

void VixApplication::changeCurrent(int ix)
{
    selectionModels_.setCurrent(ix);
}

void VixApplication::setSelected(const QModelIndex &current, const QModelIndex &prev)
{
    LOG_S_(Debug, setSelected);
    auto selected = stringListModel_.data(current, Qt::DisplayRole).toString().toStdString();
    LOG_M_(Debug, "setSelected to " << selected);
    if (selectionModels_.empty())
        return;
    selectionModels_.current()->setSelected(selected);
}

void VixApplication::updateSelection_(vix::model::Selection *selectionModel)
{
    LOG_SM_(Debug, VixApplication::updateSelection_, "selectionModel: " << selectionModel);
    pathLabel_.setText(vix::model::Path::Unlock(selectionModel->path())->path().c_str());

    //Expand or shrink the tab bar if necessary and populate it
    {
        auto selections = selectionModel->selections();
        while (tabBar_.count() != selections.size())
        {
            if (tabBar_.count() < selections.size())
                tabBar_.addTab("");
            else
                tabBar_.removeTab(0);
        }
        for (auto selection = selections.begin(); selection != selections.end(); ++selection)
        {
            auto ix = selection-selections.begin();
            tabBar_.setTabText(ix, vix::model::Path::Unlock((*selection)->path())->name().c_str());
            if (selectionModel == *selection)
                tabBar_.setCurrentIndex(ix);
        }
    }
    vix::model::Files files;
    int selectedIX;
    selectionModel->getFiles(files, selectedIX);
    QStringList stringList;
    for (auto it = files.begin(); it != files.end(); ++it)
    {
        auto &file = *it;
        vix::model::File::Unlock unlockedFile(file);
        if (unlockedFile->isDirectory())
            stringList << (unlockedFile->name() + "/").c_str();
        else
            stringList << unlockedFile->name().c_str();
    }
    stringListModel_.setStringList(stringList);
    LOG_M_(Debug, "selectedIX: " << selectedIX);
    auto ix = stringListModel_.index(selectedIX);
    selectionView_.selectionModel()->select(ix, QItemSelectionModel::Select);
    selectionView_.scrollTo(ix, QAbstractItemView::EnsureVisible);
}
