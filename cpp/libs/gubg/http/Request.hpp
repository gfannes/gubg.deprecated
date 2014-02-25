#ifndef HEADER_gubg_http_Request_hpp_ALREADY_INCLUDED
#define HEADER_gubg_http_Request_hpp_ALREADY_INCLUDED

#include "gubg/http/Codes.hpp"
#include "gubg/http/Parameters.hpp"
#include <string>

namespace gubg { namespace http { 

    enum class Verb {Unknown, Get, Head, Post, Put, Delete, Trace, Options, Connect, Patch};

    class Request
    {
        public:
            static Request Get(const std::string &uri);
            static Request Post(const std::string &uri);

            bool valid() const {return verb_ != Verb::Unknown and !uri_.empty() and !version_.empty();}

            std::string serialize() const;

            const std::string &uri() const {return uri_;}
            Request &setUri(const std::string &str) {uri_ = str; return *this;}

            Verb verb() const {return verb_;}
            Request &setVerb(Verb v) {verb_ = v; return *this;}

            const std::string &version() const {return version_;}
            Request &setVersion(const std::string &v) {version_ = v; return *this;}

            Request &setParameter(const Key &key, const Value &value) {parameters_[key] = value; return *this;}
            bool hasParameter(const Key &key) const;
            ReturnCode getParameter(std::string &value, const Key &key) const;
            ReturnCode getParameter(long &value, const Key &key) const;

            const std::string &body() const {return body_;}
			//Sets Content-Length parameter accordingly
            Request &setBody(const std::string &body) {body_ = body; return setParameter("Content-Length", std::to_string(body_.size()));}
			//Sets Content-Length and Content-Type parameters accordingly
            Request &setBody(const std::string &body, const std::string &type) {return setBody(body).setParameter("Content-Type", type);}

            //Returns silent NotEnoughData
            ReturnCode parseHeader(size_t &headerSize, const std::string &msg);
            ReturnCode parse(const std::string &msg);

            void swap(Request &);

        private:
            Verb verb_ = Verb::Unknown;
            std::string uri_;
            std::string version_ = "http/1.1";
            Parameters parameters_;
            std::string body_;
    };

} } 
#endif
