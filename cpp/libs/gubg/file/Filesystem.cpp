#include "gubg/file/Filesystem.hpp"
#include "gubg/Platform.hpp"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstddef>
#ifdef GUBG_API_POSIX
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#ifdef GUBG_API_LINUX
#include <stdlib.h>
#endif
#ifdef GUBG_API_WIN32
#include "Windows.h"
#endif
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE "Filesystem"
#include "gubg/log/begin.hpp"

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
    MSS_BEGIN(ReturnCode, "filename: [" << file.name() << "]");
    MSS(File::Unknown == file.type() || File::Regular ==  file.type(), ExpectedRegular);
    ifstream fi(file.name(), ios_base::in | ios_base::binary | ios_base::ate);
    MSS(bool(fi), CouldNotOpenFile);
    const auto fileSize = fi.tellg();
    content.resize(fileSize);
    fi.seekg(0);
    fi.read(&content[0], fileSize);
    MSS_END();
}
ReturnCode gubg::file::read(SmartRange<string> &range, const File &file)
{
    MSS_BEGIN(ReturnCode);
    string content;
    MSS(read(content, file));
    range = std::move(content);
    MSS_END();
}

#ifdef GUBG_API_POSIX
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
#endif
#if defined(GUBG_API_POSIX) && !defined(GUBG_API_LINUX)
//Some things are missing for MinGW
#define NAME_MAX FILENAME_MAX
int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result)
{
	std::mutex m;
	std::lock_guard<std::mutex> lock(m);
	struct dirent *p;
	if (!(p = ::readdir(dirp)))
	{
		*result = 0;
		return 0;
	}
	*entry = *p;
	*result = entry;
	return 0;
}
#endif
ReturnCode gubg::file::read(std::vector<File> &files, const File &file)
{
    MSS_BEGIN(ReturnCode, file.name());
    MSS(File::Unknown == file.type() || File::Directory ==  file.type(), ExpectedDirectory);
#ifdef GUBG_API_POSIX
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
#ifdef GUBG_API_LINUX
            switch (entryp->d_type)
            {
                case DT_DIR: type = File::Directory; break;
                case DT_REG: type = File::Regular; break;
                case DT_LNK: type = File::Symbolic; break;
                case DT_FIFO: type = File::FIFO; break;
            }
            if (File::Unknown != type)
                files.push_back(File(file).setType(type) << entryp->d_name);
#else
	    //MinGW does not have the dirent.d_type data member
	    {
		    File f(file);
		    f << entryp->d_name;
		    if (mss::isOK(determineType(f)))
			    files.push_back(f);
	    }
#endif
        }
    }
#else
    MSS_L(NotImplemented);
#endif
    MSS_END();
}

ReturnCode gubg::file::write(const std::string &content, const File &file)
{
    MSS_BEGIN(ReturnCode);
    MSS(File::Unknown == file.type() || File::Regular ==  file.type(), ExpectedRegular);
    ofstream fo(file.name(), ios_base::out | ios_base::binary | ios_base::trunc);
    MSS(bool(fo), CouldNotWriteFile);
    fo.write(content.data(), content.size());
    MSS(bool(fo), CouldNotWriteFile);
    MSS_END();
}

ReturnCode gubg::file::remove(const File &file)
{
    return (::remove(file.name().c_str()) == 0 ? ReturnCode::OK : ReturnCode::CouldNotRemove);
}

ReturnCode gubg::file::copy(const File &from, const File &to)
{
    MSS_BEGIN(ReturnCode);
    string content;
    MSS(read(content, from));
    MSS(write(content, to));
    MSS_END();
}

ReturnCode gubg::file::determineType(File &file)
{
    MSS_BEGIN(ReturnCode);
    struct stat statbuf;
#ifdef GUBG_API_LINUX
    auto res = ::lstat(file.name().c_str(), &statbuf);
#else
#ifdef __GNUC__
    auto res = ::stat(file.name().c_str(), &statbuf);
#endif
#ifdef _MSC_VER
    auto res = _stat(file.name().c_str(), &statbuf);
#endif
#endif
    if (res != 0)
    {
        L_("File \"" << file.name() << "\" does not exist");
        MSS_L(FileDoesNotExist);
    }
    switch (statbuf.st_mode & S_IFMT)
    {
        case S_IFREG: file.setType(File::Regular); break;
        case S_IFDIR: file.setType(File::Directory); break;
        case S_IFIFO: file.setType(File::FIFO); break;
#ifdef GUBG_API_LINUX
        case S_IFLNK: file.setType(File::Symbolic); break;
#endif
        default: MSS_L(UnknownFileType); break;
    }
    MSS_END();
}

ReturnCode gubg::file::resolve(File &file)
{
    MSS_BEGIN(ReturnCode);
    char buffer[PATH_MAX];
#ifdef GUBG_API_LINUX
    MSS(::realpath(file.name().c_str(), buffer));
#endif
#ifdef GUBG_API_WIN32
	const auto len = ::GetFullPathName(file.name().c_str(), PATH_MAX, buffer, 0);
    MSS(len < PATH_MAX);
#endif
    file.setName(buffer);
    MSS(determineType(file));
    MSS_END();
}

bool gubg::file::exists(const File &file)
{
    struct stat statbuf;
#ifdef GUBG_API_LINUX
    if (0 != ::lstat(file.name().c_str(), &statbuf))
        return false;
#else
    if (0 != ::stat(file.name().c_str(), &statbuf))
        return false;
#endif
    return true;
}
bool gubg::file::isRegular(const File &file)
{
    struct stat statbuf;
#ifdef GUBG_API_LINUX
    if (0 != ::lstat(file.name().c_str(), &statbuf))
        return false;
#else
    if (0 != ::stat(file.name().c_str(), &statbuf))
        return false;
#endif
    return (statbuf.st_mode & S_IFMT) == S_IFREG;
}
bool gubg::file::isDirectory(const File &file)
{
    struct stat statbuf;
#ifdef GUBG_API_LINUX
    if (0 != ::lstat(file.name().c_str(), &statbuf))
        return false;
#else
    if (0 != ::stat(file.name().c_str(), &statbuf))
        return false;
#endif
    return (statbuf.st_mode & S_IFMT) == S_IFDIR;
}

ReturnCode gubg::file::getcwd(File &file)
{
    MSS_BEGIN(ReturnCode);
    const size_t size = 4096;
    string cwd(size, '\0');
    MSS(0 != ::getcwd(&cwd[0], size), CouldNotGetCWD);
    cwd.resize(strlen(&cwd[0]));
    file.setName(cwd);
    MSS(determineType(file));
    MSS_END();
}
gubg::file::File gubg::file::getcwd()
{
    File file;
    if (!mss::isOK(getcwd(file)))
        return File();
    return file;
}

ReturnCode gubg::file::chmod(const File &file, const Mode &mode)
{
    MSS_BEGIN(ReturnCode);
#ifdef GUBG_API_LINUX
    mode_t m = 0;
    if (mode.user  & Read)    m |= S_IRUSR;
    if (mode.user  & Write)   m |= S_IWUSR;
    if (mode.user  & Execute) m |= S_IXUSR;
    if (mode.group & Read)    m |= S_IRGRP;
    if (mode.group & Write)   m |= S_IWGRP;
    if (mode.group & Execute) m |= S_IXGRP;
    if (mode.other & Read)    m |= S_IROTH;
    if (mode.other & Write)   m |= S_IWOTH;
    if (mode.other & Execute) m |= S_IXOTH;
    const auto ret = ::chmod(file.name().c_str(), m);
    MSS(ret == 0);
#endif
    MSS_END();
}

#include "gubg/log/end.hpp"
