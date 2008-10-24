#ifndef dir_h
#define dir_h

#include <dirent.h>
#ifdef MINGW32
#include <sys/stat.h>
#endif
#include <string>
#include <vector>

#include "regex.hpp"

using namespace std;

// type: 4 == dir, 8 == file

// Using this class, you can change to other directories and once it is deleted, will jump back to its original dir
class Dir
{
public:
    Dir(string dir)
        {
            char *str;
            mDirOrig = str = getcwd(NULL, 0);
            free(str);
            if (!changeDir(dir))
                cerr << "ERROR::Could not change to " << dir << " in constructor" << endl;
        }
    ~Dir()
        {
            cout << "Going back to " << mDirOrig << "" << endl;
            changeDir(mDirOrig);
        }

    bool changeDir(string dir)
        {
            return chdir(dir.c_str()) == 0;
        }

    bool entries(vector<string> *files = NULL, vector<string> *dirs = NULL, string pattern = ".")
        {
            DIR *dp;
            struct dirent *ep;
            Regex re(pattern);
            string str;
            
            dp = opendir("./");
            if (dp == NULL)
            {
                cerr << "Couldn't open the directory" << endl;
                return false;
            }
             if (dirs)
                dirs->resize(0);
             if (files)
                files->resize(0);
            while ((ep = readdir(dp)))
            {
                char fileType = 'u';
#ifdef MINGW32
                struct stat filestat;
                if (stat(ep->d_name, &filestat) != 0)
                {
                    cerr << "Could not stat " << ep->d_name << endl;
                    return false;
                }
                if (S_ISREG(filestat.st_mode))
                    fileType = 'f';
                else if (S_ISDIR(filestat.st_mode))
                    fileType = 'd';
#else
                switch (ep->d_type)
                {
                case 4:
                    fileType = 'd';
                    break;
                case 8:
                    fileType = 'f';
                    break;
                }
#endif

                switch (fileType)
                {
                case 'd':
                    if (dirs && strcmp(ep->d_name,".")!=0 && strcmp(ep->d_name,"..")!=0)
                    {
                        str = ep->d_name;
                        if (re.match(str))
                            dirs->push_back(str);
                    }
                    break;
                case 'f':
                    if (files)
                    {
                        str = ep->d_name;
                        if (re.match(str))
                            files->push_back(str);
                    }
                    break;
                default:
                    cerr << "WARNING::Unknown entry type for " << ep->d_name << endl;
                    break;
                };
            }
            (void) closedir (dp);
            return true;
        }
    
    string origDir()
        {
            return mDirOrig;
        }
    string toString()
        {
            char *str = getcwd(NULL, 0);
            string res(str);
            free(str);
            return res;
        }
    string pwd()
        {
            char *str = getcwd(NULL, 0);
            string res(str);
            free(str);
            return res;
        }
private:
    string mDirOrig;
};

#endif
