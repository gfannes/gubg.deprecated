//#define GUBG_LOG
#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
#include <fstream>
#ifdef GUBG_LINUX
#include <dirent.h>
#include <sys/stat.h>
#endif
using namespace gubg::file;
using namespace std;

#include "gubg/l.hpp"

ReturnCode gubg::file::size(size_t &fileSize, const File &file)
{
    MSS_BEGIN(ReturnCode);
    MSS(File::Unknown == file.type() || File::Regular ==  file.type(), ExpectedRegular);
    ifstream fi(file.name(), ios_base::in | ios_base::binary | ios_base::ate);
    MSS(bool(fi), CouldNotOpenFile);
    fileSize = fi.tellg();
    MSS_END();
}

ReturnCode gubg::file::read(string &content, const File &file)
{
    MSS_BEGIN(ReturnCode);
    MSS(File::Unknown == file.type() || File::Regular ==  file.type(), ExpectedRegular);
    ifstream fi(file.name(), ios_base::in | ios_base::binary | ios_base::ate);
    MSS(bool(fi), CouldNotOpenFile);
    const auto fileSize = fi.tellg();
    content.resize(fileSize);
    fi.seekg(0);
    fi.read(&content[0], fileSize);
    MSS_END();
}

namespace 
{
    struct Dir
    {
        DIR *h;
        Dir(DIR *d):h(d){}
        ~Dir()
        {
            if (h)
                ::closedir(h);
        }
    };
}
ReturnCode gubg::file::read(std::vector<File> &files, const File &file)
{
    MSS_BEGIN(ReturnCode, read, file.name());
    MSS(File::Unknown == file.type() || File::Directory ==  file.type(), ExpectedDirectory);
#ifdef GUBG_LINUX
    //Open the directory in a RAII
    Dir dir(::opendir(file.name().c_str()));
    MSS(dir.h, CouldNotOpenDir);
    //Create enough space for the dirent struct
    string entrypBuffer(offsetof(struct dirent, d_name)+NAME_MAX+1, '0');
    struct dirent *entryp = (struct dirent *)entrypBuffer.data();
    //Iterate over the directory entries
    struct dirent *tmp;
    for (;;)
    {
        MSS(0 == ::readdir_r(dir.h, entryp, &tmp), CouldNotReadEntry);
        if (!tmp)
            //If this is set to 0, we are done with iterating
            break;
        if (::strcmp(entryp->d_name, ".") == 0 || ::strcmp(entryp->d_name, "..") == 0)
            //We skip "." and ".."
            continue;

        {
            File::Type type = File::Unknown;
            switch (entryp->d_type)
            {
                case DT_DIR: type = File::Directory; break;
                case DT_REG: type = File::Regular; break;
                case DT_LNK: type = File::Symbolic; break;
                case DT_FIFO: type = File::FIFO; break;
            }
            if (File::Unknown != type)
                files.push_back(File(file).setType(type) << entryp->d_name);
        }
    }
#else
    MSS_L(NotImplemented);
#endif
    MSS_END();
}

ReturnCode gubg::file::determineType(File &file)
{
    MSS_BEGIN(ReturnCode);
    struct stat statbuf;
    MSS(0 == ::lstat(file.name().c_str(), &statbuf), FileDoesNotExist);
    switch (statbuf.st_mode & S_IFMT)
    {
        case S_IFREG: file.setType(File::Regular); break;
        case S_IFDIR: file.setType(File::Directory); break;
        case S_IFIFO: file.setType(File::FIFO); break;
        case S_IFLNK: file.setType(File::Symbolic); break;
        default: MSS_L(UnknownFileType); break;
    }
    MSS_END();
}
