#include "test/data.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
using namespace std;

//We can have basic memory management, e.g., by deriving from a shared_ptr<>
//shared_ptr<> is probably not meant as a base class, but it seems to work (not the non-virtual destructor)
struct RawDir: public shared_ptr<raw::Dir>
{
    typedef shared_ptr<raw::Dir> Base;
    RawDir(): Base(new raw::Dir){}
    ~RawDir(){}
    void setName(const string &name){get()->name = name;}
    void add(RawDir &dir){get()->add(dir.get());}
    string path() const {return get()->path();}
};

int main()
{
    {
        LOG("Testing raw");
        RawDir root, home, gfannes;
        root.setName("");
        home.setName("home");
        gfannes.setName("gfannes");
        root.add(home);
        home.add(gfannes);
        LOG(gfannes.path());
    }
    return 0;
}
