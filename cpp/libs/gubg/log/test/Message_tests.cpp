#include "gubg/log/Message.hpp"

struct Module: gubg::log::Cout
{
    Module()
    {
        os << "Header ";
    }
    ~Module()
    {
        os << "Footer " << std::endl;
    }
};
struct M_always: Module, gubg::log::Always { };
struct M_never: Module, gubg::log::Never { };
struct M_above_2: Module, gubg::log::IfAboveLevel<2> { };

int main()
{
    gubg::log::Message<M_always>() << "this is a message";
    gubg::log::Message<M_always>(2) << "this is a message at level 2";
    gubg::log::Message<M_never>() << "this is a message";
    gubg::log::Message<M_never>(2) << "this is a message at level 2";
    gubg::log::Message<M_above_2>() << "this is a message";
    gubg::log::Message<M_above_2>(2) << "this is a message at level 2";
    return 0;
}
