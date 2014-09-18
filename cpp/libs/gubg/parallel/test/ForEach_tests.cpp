#include "catch/catch.hpp"
#include "gubg/parallel/ForEach.hpp"
#include <vector>
#include <iostream>
#include <chrono>
using namespace std;

#define RANGE(ctr) ctr.begin(), ctr.end()

TEST_CASE("ForEach", "[parallel]")
{
    vector<int> vs(100);
    iota(RANGE(vs), 0);

    auto ftor = [](int v)
    {
        {
            static mutex m;
            lock_guard<mutex> l(m);
            cout << std::this_thread::get_id() << " " << v << endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };

    std::for_each(RANGE(vs), ftor);
    cout << string(100, '*') << endl;
    gubg::parallel::for_each(RANGE(vs), ftor, 40);
}
