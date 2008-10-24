#include <sstream>

#include "treeview.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    const int nrLines = 40;
    const int width = 400;
    const int height = 400;
    TreeView tv(width, height, nrLines);
    bool fit;

    vector<SDL_keysym> keys;
    long iter = -1;
    while (!tv.quit())
    {
        ++iter;
        if (tv.checkKeys(keys))
        {
            for (int i=0; i<keys.size(); ++i)
            {
                SDL_keysym &key = keys[i];
                cout << key.sym << " " << key.mod << endl;
            }
        }

        for (int i=0; i<nrLines; ++i)
        {
            ostringstream ostr;
            ostr << "(" << i << ")  Going into iteration " << iter << " while printing a long line that extends to the right of the screen";
            tv.drawLine(i, iter%(i+1), ostr.str(), &fit);
        }

        tv.show();
    }

    return 0;
}
