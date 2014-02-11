#ifndef HEADER_gubg_http_Request_hpp_ALREADY_INCLUDED
#define HEADER_gubg_http_Request_hpp_ALREADY_INCLUDED

#include "gubg/http/Codes.hpp"
#include <map>
#include <string>

namespace gubg { namespace http { 

    enum class Verb {Unknown, Get, Head, Post, Put, Delete, Trace, Options, Connect, Patch};

    class Request
    {
        public:
            typedef std::string Key;
            typedef std::string Value;

            static Request Get(const std::string &uri);

            bool valid() const {return verb_ != Verb::Unknown and !uri_.empty() and !version_.empty();}

            std::string serialize() const;

            const std::string &uri() const {return uri_;}
            Request &setUri(const std::string &str) {uri_ = str; return *this;}

            Verb verb() const {return verb_;}
            Request &setVerb(Verb v) {verb_ = v; return *this;}

            const std::string &version() const {return version_;}
            Request &setVersion(const std::string &v) {version_ = v; return *this;}

            Request &setParameter(const Key &key, const Value &value) {parameters_[key] = value; return *this;}
            Request &setContentLength() {return setParameter("Content-Length", std::to_string(body_.size()));}

            const std::string &body() const {return body_;}
            Request &setBody(const std::string &b) {body_ = b; return *this;}

            ReturnCode parse(const std::string &);

            void swap(Request &);

        private:
            Verb verb_ = Verb::Unknown;
            std::string uri_;
            std::string version_ = "http/1.1";
            typedef std::map<Key, Value> Parameters;
            Parameters parameters_;
            std::string body_;
    };

} } 
#endif
