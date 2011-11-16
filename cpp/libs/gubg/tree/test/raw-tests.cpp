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

namespace
{
    template <typename Node>
    void iterate(Node &node, bool log = true)
    {
        if (log)
            LOG("\nStarting iteration from " << node.name);
        size_t i = 0;
        for (auto it = node.begin(); it != node.end(); ++it, ++i)
        {
            if (log)
                LOG("Iteration " << i << ": path_.size(): " << it.path_.size() << " name: " << it->name);
        }
    }
}

int main()
{
    {
        LOG("Testing raw");
        raw::Dir a, b, c;
        raw::Reg d, e, f;
#define CONNECT(p, c) p.add(&c)
        CONNECT(a, b);
        CONNECT(b, d);
        CONNECT(b, e);
        CONNECT(a, c);
        CONNECT(c, f);
        LOG(b.path());
#define SET_NAME(n) n.name = #n
        SET_NAME(a);
        SET_NAME(b);
        SET_NAME(c);
        SET_NAME(d);
        SET_NAME(e);
        SET_NAME(f);
        LOG(b.path());
        LOG(e.filename());
        iterate(a);
        iterate(b);
        iterate(e);
        iterate(f);
        for (auto i = 0; i < 100000; ++i)
            iterate(a, false);
    }
    {
        LOG("Testing RawDir");
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
