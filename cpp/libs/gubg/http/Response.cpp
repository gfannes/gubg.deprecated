#include "gubg/http/Response.hpp"
#include "gubg/Strange.hpp"
#include <sstream>
using namespace std;

namespace  { 
    const char sp = ' ';
    const std::string eol = "\r\n";
} 

#define GUBG_MODULE "HTTP"
#include "gubg/log/begin.hpp"
namespace gubg { namespace http { 

    string Response::serialize() const
    {
        if (!valid())
            return "";
        ostringstream oss;
        oss  << version_ << sp << status_ << sp << reason_ << eol;
        for (auto p: parameters_)
            oss << p.first << ": " << p.second << eol;
        oss << eol;
        oss << body_;
        return oss.str();
    }
    ReturnCode Response::parse(const string &str)
    {
        MSS_BEGIN(ReturnCode);

        Response response;

        Strange strange(str);
        {
            Strange version;
            MSS(strange.popUntil(version, sp));
            response.setVersion(version.str());
        }
        {
            Strange status;
            MSS(strange.popUntil(status, sp));
            long s;
            MSS(status.popDecimal(s));
            response.setStatusCode(s);
        }
        {
            Strange reason;
            MSS(strange.popUntil(reason, eol));
        }

        //Read the headers
        while (true)
        {
            Strange header;
            MSS(strange.popUntil(header, eol));
            Strange key;
            if (!header.popUntil(key, ':'))
            {
                //We are done reading headers
                MSS(key.empty());
                break;
            }
            //Remove leading and trailing whitespaces
            while (header.popCharIf(sp)){}
            while (header.popCharBack(sp)){}
            response.setParameter(key.str(), header.str());
        }

        //The rest belongs to the body
		//We do not use setBody, that API call sets Content-Length accordingly
        response.body_ = strange.str();

        swap(response);

        MSS_END();
    }
    void Response::swap(Response &other)
    {
        { const auto v = status_; status_ = other.status_; other.status_ = v; }
#define L_SWAP(n) n.swap(other.n)
        L_SWAP(reason_);
        L_SWAP(version_);
        L_SWAP(parameters_);
        L_SWAP(body_);
    }

} } 
#include "gubg/log/end.hpp"
