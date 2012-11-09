#ifndef HEADER_gubg_with_hpp_ALREADY_INCLUDED
#define HEADER_gubg_with_hpp_ALREADY_INCLUDED

//Usage:
//
//struct S
//{
//    S():v(10){cout << "ctor" << endl;}
//    ~S(){cout << "dtor" << endl;}
//    int v;
//};
//
//int main()
//{
//    WITH(S())
//    {
//        _.v = 12;
//        cout << "value: " << _.v << endl;
//    }
//    return 0;
//}

#define WITH(expr) \
    if (bool local_first_time_ = true) \
    for (auto _ = (expr); local_first_time_; local_first_time_ = false)

#endif
