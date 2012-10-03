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
        using namespace gubg::tree;
        if (log)
            LOG("");
        {
            if (log)
                LOG("Starting iteration from " << node.name << " by component");
            size_t i = 0;
            for (auto it = node.begin(ByNode); it != node.end(ByNode); ++it, ++i)
            {
                if (log)
                    LOG("Iteration " << i << ": path_.size(): " << it.path_.size() << " name: " << it->name << " isLeaf():" << it->isLeaf());
            }
        }
        {
            if (log)
                LOG("Starting iteration from " << node.name << " by data");
            size_t i = 0;
            for (auto it = node.begin(ByData); it != node.end(ByData); ++it, ++i)
            {
                //isLeaf is not accessible, *it is of type Data
                if (log)
                    LOG("Iteration " << i << ": path_.size(): " << it.path_.size() << " name: " << it->name);
            }
        }
        {
            if (log)
                LOG("Starting iteration from " << node.name << " composite only");
            size_t i = 0;
            for (auto it = node.begin(NoLeaf); it != node.end(NoLeaf); ++it, ++i)
            {
                if (log)
                    LOG("Iteration " << i << ": path_.size(): " << it.path_.size() << " name: " << it->name << " isLeaf():" << it->isLeaf());
            }
        }
        {
            if (log)
                LOG("Starting iteration from " << node.name << " leaf only");
            size_t i = 0;
            for (auto it = node.begin(LeafOnly); it != node.end(LeafOnly); ++it, ++i)
            {
                if (log)
                    LOG("Iteration " << i << ": path_.size(): " << it.path_.size() << " name: " << it->name << " isLeaf():" << it->isLeaf());
            }
        }
    }
}

int main()
{
    {
        LOG("Testing raw");
        raw::Dir a, b, c, singleRoot;
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
        SET_NAME(singleRoot);
        LOG(b.path());
        LOG(e.filename());
        LOG(singleRoot.path());
        iterate(a);
        iterate(b);
        iterate(e);
        iterate(f);
        iterate(singleRoot);
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
