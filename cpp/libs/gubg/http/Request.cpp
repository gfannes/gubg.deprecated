#include "gubg/http/Request.hpp"
#include "gubg/Strange.hpp"
#include "gubg/String.hpp"
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
    const std::string eol2 = "\r\n\r\n";
} 

#define GUBG_MODULE_ "HTTP"
#include "gubg/log/begin.hpp"
namespace gubg { namespace http { 
    Request Request::Get(const string &uri)
    {
        Request res;
        res.setVerb(Verb::Get).setUri(uri);
        return res;
    }
    Request Request::Post(const string &uri)
    {
        Request res;
        res.setVerb(Verb::Post).setUri(uri);
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
    ReturnCode Request::parseHeader(size_t &headerSize, const string &str)
    {
        MSS_BEGIN(ReturnCode);
        L(gubg::toHex(str));

        Request request;

        Strange strange(str);
        Strange header;
        MSS_Q(strange.popUntil(header, eol2, true), NotEnoughData);
        MSS(header.popBack() and header.popBack());

        {
            Strange verb;
            MSS(header.popUntil(verb, sp));
            request.setVerb(from_s_(verb.str()));
        }
        {
            Strange uri;
            MSS(header.popUntil(uri, sp));
            request.setUri(uri.str());
        }
        {
            Strange version;
            MSS(header.popUntil(version, eol));
            request.setVersion(version.str());
        }

        //Read the headers
        while (!header.empty())
        {
            S();L(header.str());
            Strange headerLine;
            MSS(header.popUntil(headerLine, eol));
            Strange key;
            MSS(headerLine.popUntil(key, ':'));
            //Remove leading and trailing whitespaces
            while (headerLine.popCharIf(sp)){}
            while (headerLine.popCharBack(sp)){}
            request.setParameter(key.str(), headerLine.str());
        }

        headerSize = str.size()-strange.size();
        swap(request);

        MSS_END();
    }
    ReturnCode Request::parse(const std::string &str)
    {
        MSS_BEGIN(ReturnCode);

        http::Request request;
        size_t header_len;
        const auto rc = request.parseHeader(header_len, str);
        L(STREAM((int)rc, header_len));
        switch (rc)
        {
            case ReturnCode::NotEnoughData: MSS_Q(rc); break;
            default:                        MSS(rc);   break;
        }

        if (!request.hasParameter("Content-Length"))
        {
            L("We assume no body will me present");
            swap(request);
            MSS_RETURN_OK();
        }

        long content_len;
        MSS(request.getParameter(content_len, "Content-Length"));
        MSS_Q(str.size() >= header_len + content_len, NotEnoughData);

        request.setBody(str.substr(header_len, content_len));
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
    bool Request::hasParameter(const Key &key) const
    {
        auto it = parameters_.find(key);
        return it != parameters_.end();
    }
    ReturnCode Request::getParameter(string &value, const Key &key) const
    {
        MSS_BEGIN(ReturnCode);
        auto it = parameters_.find(key);
        MSS(it != parameters_.end());
        value = it->second;
        MSS_END();
    }
    ReturnCode Request::getParameter(long &value, const Key &key) const
    {
        MSS_BEGIN(ReturnCode);
        auto it = parameters_.find(key);
        MSS(it != parameters_.end());
        const auto &v = it->second;
        MSS(!v.empty());
        size_t pos = 0;
        value = std::stol(it->second, &pos);
        MSS(pos != 0);
        MSS_END();
    }

} } 
#include "gubg/log/end.hpp"
