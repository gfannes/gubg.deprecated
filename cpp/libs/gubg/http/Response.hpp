#ifndef HEADER_gubg_http_Response_hpp_ALREADY_INCLUDED
#define HEADER_gubg_http_Response_hpp_ALREADY_INCLUDED

#include "gubg/http/Codes.hpp"
#include <map>
#include <string>

namespace gubg { namespace http { 

    class Response
    {
        public:
            typedef unsigned long StatusCode;
            typedef std::string Key;
            typedef std::string Value;
            static const StatusCode InvalidStatusCode = -1;

            bool valid() const {return status_ != InvalidStatusCode and !version_.empty();}

            std::string serialize() const;

            StatusCode statusCode() const {return status_;}
            Response &setStatusCode(StatusCode sc) {status_ = sc; return *this;}

            const std::string &reason() const {return reason_;}
            Response &setReason(const std::string &r) {reason_ = r; return *this;}

            const std::string &version() const {return version_;}
            Response &setVersion(const std::string &v) {version_ = v; return *this;}

            Response &setParameter(const Key &key, const Value &value) {parameters_[key] = value; return *this;}
            Response &setContentLength() {return setParameter("Content-Length", std::to_string(body_.size()));}

            const std::string &body() const {return body_;}
            Response &setBody(const std::string &b) {body_ = b; return *this;}

            ReturnCode parse(const std::string &);

            void swap(Response &);

        private:
            StatusCode status_;
            std::string reason_;
            std::string version_ = "http/1.1";
            typedef std::map<Key, Value> Parameters;
            Parameters parameters_;
            std::string body_;
    };

} } 
#endif
