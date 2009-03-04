#include <iostream>

#include "file.hpp"
#include "dir.hpp"

using namespace std;

int main()
{
    {
        Dir dir("/home/gfannes/data/raw");
        cout << dir.origDir() << endl;
        cout << dir.toString() << endl;
        vector<string> files;
        vector<string> dirs;
        int i;
        dir.entries(&files,&dirs);
        for (i=0;i<files.size();i++)
            cout << "file " << i << " = " << files[i] << "" << endl;
        for (i=0;i<dirs.size();i++)
            cout << "dir " << i << " = " << dirs[i] << "" << endl;
    }
    Dir dir2("./");
    cout << "" << dir2.toString() << "" << endl;
    return 0;
}
