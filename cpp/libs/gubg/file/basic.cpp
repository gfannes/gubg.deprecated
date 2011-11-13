#include "gubg/file/basic.hpp"
#include "gubg/parse/basic.hpp"
#include "gubg/Platform.hpp"
using namespace std;

namespace
{
    typedef gubg::file::basic::ReturnCode RC;
    template <typename Parts>
        RC tokenizeAbsolutePath(Parts &parts, const string &path)
        {
            MSS_BEGIN(RC);
            MSS_T(!path.empty(), PathIsEmpty);
            parts = gubg::parse::tokenize(path, "/");
            MSS_T(!parts.empty(), PartsIsEmpty);
            MSS_T(parts[0] == "", PathIsNotAbsolute);
            parts.pop_front();
            MSS_END();
        }
}

gubg::file::basic::ReturnCode gubg::file::basic::expandPath(string &pathE, const string &path)
{
    MSS_BEGIN(ReturnCode);

    //Split path into a vector of parts
    auto parts = gubg::parse::tokenize(path, "/");
    MSS_T(!parts.empty(), InvalidPath);

    //We construct the expanded path by gradually changing the working directory based on parts from path
    deque<string> partsE;
    MSS(tokenizeAbsolutePath(partsE, getCurrentWorkingDirectory()));
    bool firstTime = true;
    while (!parts.empty())
    {
        auto head = parts[0];
        if (head == "")
        {
            if (firstTime)
                partsE.clear();
            //We ignore "//" or trailing "/" that are not leading
        }
        else if (head == ".")
        {
            //Nothing to be done, we started from the current dir and "." means no change
        }
        else if (head == "..")
        {
            //Go up one dir, we don't fail if partsE is empty (alread at root)
            if (!partsE.empty())
                partsE.pop_back();
        }
        else if (head == "~")
        {
            if (firstTime)
            {
                //We change to the $HOME directory
                MSS(tokenizeAbsolutePath(partsE, getHomeDirectory()));
            }
        }
        else
            partsE.push_back(head);

        parts.pop_front();
        firstTime = false;
    }

    //Convert the parts into an absolute path
    {
        ostringstream oss;
        if (partsE.empty())
            oss << "/";
        else
            for (auto partE = partsE.begin(); partE != partsE.end(); ++partE)
                oss << "/" << *partE;
        pathE = oss.str();
    }

    MSS_END();
}
