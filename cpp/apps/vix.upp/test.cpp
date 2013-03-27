#include <QApplication>
#include <QMainWindow>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QThread>
#include "boost/thread.hpp"
#include <iostream>
using namespace std;

namespace
{
    const int NrEdits = 10;
}

typedef std::vector<QTextEdit*> TextEdits;
typedef std::vector<QTextDocument*> TextDocuments;
struct Documents
{
    Documents():
        quit_(false)
    {
        thread_ = boost::thread(boost::ref(*this));
    }
    ~Documents()
    {
        quit_ = true;
        thread_.join();
    }
    void operator()()
    {
        cout << "Thread is starting up" << endl;
        for (int i = 0; i < NrEdits; ++i)
            documents_.push_back(new QTextDocument);

        cout << "Thread is about to spin" << endl;
        while (!quit_)
        {
            for (TextDocuments::iterator td = documents_.begin(); td != documents_.end(); ++td)
            {
                QTextDocument &doc = **td;
                doc.setHtml("asdfasf");
                emit
            }
        }
    }
    TextEdits edits_;
    TextDocuments documents_;
    bool quit_;
    boost::thread thread_;
};

int main(int argc, char **argv)
{
    qRegisterMetaType<QTextCursor>("QTextCursor");

    QApplication app(argc, argv);
    QMainWindow mw;

    QWidget cw(&mw); 
    QVBoxLayout vbox(&cw);
    mw.setCentralWidget(&cw);

    TextEdits textEdits;
    for (int i = 0; i < NrEdits; ++i)
    {
        auto te = new QTextEdit;
        textEdits.push_back(te);
        vbox.addWidget(te);
    }

    mw.showMaximized();
    mw.setVisible(true);

    Documents documents(textEdits);

    app.exec();
    return 0;
}
