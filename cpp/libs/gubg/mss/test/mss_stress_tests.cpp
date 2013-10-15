#include "Sum.hpp"
#if 1
#include "gubg/mss/mss_nostl.hpp"
#else
#include "gubg/mss/mss.hpp"
#endif
#include "gubg/chrono/Stopwatch.hpp"
#include <string>

//These tests give a bit strange results, maybe due to optimalization, better real-world tests are needed...

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
enum class ReturnCode
{
    MSS_DEFAULT_CODES,
    No,
    c0, c1, c2, c3, c4, c5, c6, c7, c8, c9,
};

template <int Denominator>
ReturnCode divisable_direct(int v)
{
    return (v%Denominator ? ReturnCode::No : ReturnCode::OK);
}
template <int Denominator>
ReturnCode divisable_mss(int v)
{
    MSS_BEGIN(ReturnCode);
    MSS(v%Denominator == 0, No);
    MSS_END();
}
template <int Denominator>
void divisable_exc(int v)
{
    if (v%Denominator)
        throw int(Denominator);
}
ReturnCode multipleOf6_direct(int v)
{
    MSS_BEGIN(ReturnCode);
    MSS(divisable_direct<2>(v));
    MSS(divisable_direct<3>(v));
    MSS_END();
}
ReturnCode multipleOf6_direct2(int v)
{
    try
    {
        MSS_BEGIN(ReturnCode);
        MSS(divisable_direct<2>(v));
        MSS(divisable_direct<3>(v));
        MSS_END();
    }
    catch (...)
    {
    }
}
ReturnCode multipleOf6_mss(int v)
{
    MSS_BEGIN(ReturnCode);
    MSS(divisable_mss<2>(v));
    MSS(divisable_mss<3>(v));
    MSS_END();
}
ReturnCode multipleOf6_exc(int v)
{
    try
    {
        divisable_exc<2>(v);
        divisable_exc<3>(v);
    }
    catch (int &)
    {
        return ReturnCode::No;
    }
    return ReturnCode::OK;
}

template <long V>
ReturnCode leq_direct(long v)
{
    return (v <= V ? ReturnCode::OK : ReturnCode::No);
}
template <long V>
ReturnCode geq_direct(long v)
{
    return (v >= V ? ReturnCode::OK : ReturnCode::No);
}
template <long V>
ReturnCode leq_mss(long v)
{
    MSS_BEGIN(ReturnCode);
    MSS(v <= V, No);
    MSS_END();
}
template <long V>
ReturnCode geq_mss(long v)
{
    MSS_BEGIN(ReturnCode);
    MSS(v >= V, No);
    MSS_END();
}
template <long V>
void leq_exc(long v)
{
    if (v > V)
        throw ReturnCode::No;
}
template <long V>
void geq_exc(long v)
{
    if (v < V)
        throw ReturnCode::No;
}
template <long L, long H>
ReturnCode inInterval_direct(long v)
{
    if (ReturnCode::OK != geq_direct<L>(v))
        return ReturnCode::No;
    if (ReturnCode::OK != leq_direct<H>(v))
        return ReturnCode::No;
    return ReturnCode::OK;
}
template <long L, long H>
ReturnCode inInterval_mss(long v)
{
    MSS_BEGIN(ReturnCode);
    MSS(geq_mss<L>(v));
    MSS(leq_mss<H>(v));
    MSS_END();
}
template <long L, long H>
ReturnCode inInterval_exc(long v)
{
    try
    {
        geq_exc<L>(v);
        leq_exc<H>(v);
    }
    catch (ReturnCode &rc) {return rc;}
    return ReturnCode::OK;
}

template <unsigned long Max>
ReturnCode sum_direct(int v)
{
    test::Sum sum(Max);
    for (int i = 0; i < v; ++i)
        if (test::ReturnCode::OK != sum.add_direct(i))
            return ReturnCode::No;
    return ReturnCode::OK;
}
template <unsigned long Max>
ReturnCode sum_mss(int v)
{
    MSS_BEGIN(ReturnCode);
    test::Sum sum(Max);
    for (int i = 0; i < v; ++i)
        MSS(sum.add_mss(i), No);
    MSS_END();
}
template <unsigned long Max>
ReturnCode sum_exc(int v)
{
    try
    {
        test::Sum sum(Max);
        for (int i = 0; i < v; ++i)
            sum.add_exc(i);
    }
    catch (test::Error &error){return ReturnCode::No;}
    return ReturnCode::OK;
}

template <int V>
bool step(int v)
{
    return v != V;
}
ReturnCode switch_direct(int v)
{
    if (!step<0>(v)) return ReturnCode::c0;
    if (!step<1>(v)) return ReturnCode::c1;
    if (!step<2>(v)) return ReturnCode::c2;
    if (!step<3>(v)) return ReturnCode::c3;
    if (!step<4>(v)) return ReturnCode::c4;
    if (!step<5>(v)) return ReturnCode::c5;
    if (!step<6>(v)) return ReturnCode::c6;
    if (!step<7>(v)) return ReturnCode::c7;
    if (!step<8>(v)) return ReturnCode::c8;
    if (!step<9>(v)) return ReturnCode::c9;
    return ReturnCode::OK;
}
ReturnCode switch_mss(int v)
{
    MSS_BEGIN(ReturnCode);
    MSS(step<0>(v), c0);
    MSS(step<1>(v), c1);
    MSS(step<2>(v), c2);
    MSS(step<3>(v), c3);
    MSS(step<4>(v), c4);
    MSS(step<5>(v), c5);
    MSS(step<6>(v), c6);
    MSS(step<7>(v), c7);
    MSS(step<8>(v), c8);
    MSS(step<9>(v), c9);
    MSS_END();
}
template <int V>
void step_(int v)
{
    if (v == V)
        throw ReturnCode::No;
}
ReturnCode switch_exc(int v)
{
    try
    {
        step_<0>(v);
        step_<1>(v);
        step_<2>(v);
        step_<3>(v);
        step_<4>(v);
        step_<5>(v);
        step_<6>(v);
        step_<7>(v);
        step_<8>(v);
        step_<9>(v);
    }
    catch (...) {return ReturnCode::No;}
    return ReturnCode::OK;
}

template <unsigned long NrIter, unsigned long Start, unsigned long Stop, typename Function>
bool test_(Function function, const std::string &msg)
{
    gubg::chrono::Stopwatch<> sw;
    bool ret = true;
    for (unsigned int j = 0; j < NrIter; ++j)
    {
        unsigned long nrOK = 0;
        for (unsigned long i = Start; i < Stop; ++i)
        {
            if (ReturnCode::OK == function(i))
                ++nrOK;
        }
    }
    sw.mark();
    L(msg << "\t took \t" << sw.total_elapse().count());
    return ret;
}

int main()
{
#if 0
    test_<1000, 500>(&divisable_direct<2>, "even_direct");
    test_<1000, 500>(&divisable_mss<2>, "even_mss");
    test_<1000, 500>(&multipleOf6_direct, "m6_direct");
    test_<1000, 500>(&multipleOf6_direct2, "m6_direct2");
    test_<1000, 500>(&multipleOf6_mss, "m6_mss  ");
    test_<1000, 500>(&multipleOf6_exc, "m6_exc  ");
    test_<10000, 10000>(&inInterval_direct<0, 10000>, "ii_direct");
    test_<10000, 10000>(&inInterval_mss<0, 10000>, "ii_mss  ");
    test_<10000, 10000>(&inInterval_exc<0, 10000>, "ii_exc  ");
    test_<100, 100>(&sum_direct<10000>, "sum_direct");
    test_<100, 100>(&sum_mss<10000>, "sum_mss  ");
    test_<100, 100>(&sum_exc<10000>, "sum_exc  ");
#endif
    test_<100000, 10, 100>(&switch_direct, "sw_direct");
    test_<100000, 10, 100>(&switch_mss, "sw_mss  ");
    test_<100000, 10, 100>(&switch_exc, "sw_exc  ");
    return 0;
}
#include "gubg/log/end.hpp"
