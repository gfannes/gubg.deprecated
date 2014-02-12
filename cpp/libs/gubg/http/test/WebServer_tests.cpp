#include "gubg/http/WebServer.hpp"
#include <chrono>
using namespace gubg;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
class MyServer: public http::WebServer_crtp<MyServer>
{
    public:
        MyServer(): WebServer_crtp(8080) {}
        void webserver_process(http::Response &response, const http::Request &request)
        {
            S();L("Action");
            response.setStatusCode(200);
            response.setBody("<html><body>Hello world!</body></html>");
        }
};

int main()
{
    MyServer server;
    while (true)
    {
        server.process(std::chrono::seconds(1));
    }
    return 0;
}
#include "gubg/log/end.hpp"
