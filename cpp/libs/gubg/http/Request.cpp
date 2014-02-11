#include "gubg/http/Request.hpp"
#include "gubg/Strange.hpp"
#include <sstream>
using namespace std;

namespace  { 

    const char *to_s_(gubg::http::Verb verb)
    {
        switch (verb)
        {
#define L_CASE(n, v) case gubg::http::Verb::v: return #n
            L_CASE(GET, Get);
            L_CASE(HEAD, Head);
            L_CASE(POST, Post);
            L_CASE(PUT, Put);
            L_CASE(DELETE, Delete);
            L_CASE(TRACE, Trace);
            L_CASE(OPTIONS, Options);
            L_CASE(CONNECT, Connect);
            L_CASE(PATCH, Patch);
        }
        return "";
    }
    gubg::http::Verb from_s_(const std::string &str)
    {
        if (false) {}
#define L_ELSE_IF(n, v)	else if (str == #n) return gubg::http::Verb::v
        L_ELSE_IF(GET, Get);
        L_ELSE_IF(HEAD, Head);
        L_ELSE_IF(POST, Post);
        L_ELSE_IF(PUT, Put);
        L_ELSE_IF(DELETE, Delete);
        L_ELSE_IF(TRACE, Trace);
        L_ELSE_IF(OPTIONS, Options);
        L_ELSE_IF(CONNECT, Connect);
        L_ELSE_IF(PATCH, Patch);
        return gubg::http::Verb::Unknown;
    }

    const char sp = ' ';
    const std::string eol = "\r\n";
} 

#define GUBG_MODULE "HTTP"
#include "gubg/log/begin.hpp"
namespace gubg { namespace http { 
    Request Request::Get(const string &uri)
    {
        Request res;
        res.setVerb(Verb::Get).setUri(uri);
        return res;
    }
    string Request::serialize() const
    {
        if (!valid())
            return "";
        ostringstream oss;
        oss << to_s_(verb_) << sp << uri_ << sp << version_ << eol;
        for (auto p: parameters_)
            oss << p.first << ": " << p.second << eol;
        oss << eol;
        oss << body_;
        return oss.str();
    }
    ReturnCode Request::parse(const string &str)
    {
        MSS_BEGIN(ReturnCode);

        Request request;

        Strange strange(str);
        {
            Strange verb;
            MSS(strange.popUntil(verb, sp));
            request.setVerb(from_s_(verb.str()));
        }
        {
            Strange uri;
            MSS(strange.popUntil(uri, sp));
            request.setUri(uri.str());
        }
        {
            Strange version;
            MSS(strange.popUntil(version, eol));
            request.setVersion(version.str());
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
            request.setParameter(key.str(), header.str());
        }

        //The rest belongs to the body
        request.setBody(strange.str());

        swap(request);

        MSS_END();
    }
    void Request::swap(Request &other)
    {
        { const auto v = verb_; verb_ = other.verb_; other.verb_ = v; }
#define L_SWAP(n) n.swap(other.n)
        L_SWAP(uri_);
        L_SWAP(version_);
        L_SWAP(parameters_);
        L_SWAP(body_);
    }

} } 
#include "gubg/log/end.hpp"
