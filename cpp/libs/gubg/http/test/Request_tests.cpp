#include "gubg/http/Request.hpp"
#include "gubg/Testing.hpp"
using namespace gubg;

const auto request_str = "GET / HTTP/1.1\r\n\r\n";

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    http::Request request;
    TEST_OK(request.parse(request_str));
    return 0;
}
#include "gubg/log/end.hpp"
