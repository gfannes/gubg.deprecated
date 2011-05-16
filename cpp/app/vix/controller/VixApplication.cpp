#include "controller/VixApplication.hpp"
#include <QVBoxLayout>
#include <iostream>
using namespace vix;
using namespace std;

VixApplication::VixApplication(int argc, char **argv):
    QApplication(argc, argv),
    selectionModel_("/home/gfannes")
{
    QWidget *centralWidget = new QWidget(&mainWindow_); 
    QVBoxLayout *vbox = new QVBoxLayout(centralWidget);
    mainWindow_.setCentralWidget(centralWidget);
    selectionView_.setModel(&stringListModel_);
    path_.setText(selectionModel_.path().c_str());
    vbox->addWidget(&path_);
    vbox->addWidget(&selectionView_);
    vbox->addWidget(&commandLine_);
    mainWindow_.setVisible(true);

    connect(&selectionView_, SIGNAL(keyPress(QChar)), this, SLOT(process4Commandline(QChar)));

    updateSelection_();
}

void VixApplication::process4Commandline(QChar ch)
{
    int i = (int)ch.toAscii();
    switch (i)
    {
        case 27:
            commandLine_.setText("");
            break;
        default:
            QString text = commandLine_.text() + ch;
            commandLine_.setText(text);
            break;
    }
    selectionModel_.setFilter(commandLine_.text().toStdString());
    updateSelection_();
}

void VixApplication::updateSelection_()
{
    auto files = selectionModel_.getFiles();
    QStringList stringList;
    for (auto file = files.begin(); file != files.end(); ++file)
        stringList << file->name.c_str();
    stringListModel_.setStringList(stringList);
}
