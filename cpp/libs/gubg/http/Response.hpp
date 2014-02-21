#ifndef HEADER_gubg_http_Response_hpp_ALREADY_INCLUDED
#define HEADER_gubg_http_Response_hpp_ALREADY_INCLUDED

#include "gubg/http/Codes.hpp"
#include "gubg/http/Parameters.hpp"
#include <map>
#include <string>

namespace gubg { namespace http { 

    class Response
    {
        public:
            typedef unsigned long StatusCode;
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
            bool hasParameter(const Key &key) const;
            ReturnCode getParameter(std::string &value, const Key &key) const;
            ReturnCode getParameter(long &value, const Key &key) const;

            const std::string &body() const {return body_;}
			//Sets Content-Length parameter accordingly
            Response &setBody(const std::string &body) {body_ = body; return setParameter("Content-Length", std::to_string(body_.size()));}
			//Sets Content-Length and Content-Type parameters accordingly
            Response &setBody(const std::string &body, const std::string &type) {return setBody(body).setParameter("Content-Type", type);}

            ReturnCode parse(const std::string &);

            void swap(Response &);

        private:
            StatusCode status_ = 200;
            std::string reason_ = "OK";
            std::string version_ = "http/1.1";
            Parameters parameters_;
            std::string body_;
    };

} } 
#endif
