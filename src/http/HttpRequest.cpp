
#include "HttpRequest.h"

#include <string>
#include <map>
#include <iostream>

HttpRequest::HttpRequest(){
};

HttpRequest::~HttpRequest(){};

void HttpRequest::SetVersion(const std::string & ver){
    version_ = std::move(ver);
}

std::string HttpRequest::version() const{
    return version_;
}

void HttpRequest::SetMethod(const std::string &_method){
    method_ = _method;
}

std::string HttpRequest::method() const{
    return method_;
}

void HttpRequest::SetUrl(const std::string &url){
    url_ = std::move(url);
}
const std::string & HttpRequest::url() const{
    return url_;
}

void HttpRequest::SetRequestParams(const std::string &key, const std::string &value){
    request_params_[key] = value;
}
std::string HttpRequest::GetRequestValue(const std::string &key) const{
    std::string ret;
    auto it = headers_.find(key);
    return it == headers_.end() ? ret : it->second;
}
const std::map<std::string, std::string> & HttpRequest::request_params() const{
    return request_params_;
}

void HttpRequest::SetProtocol(const std::string &str){
    protocol_ = std::move(str);
}
const std::string & HttpRequest::protocol() const{
    return protocol_;
}

void HttpRequest::AddHeader(const std::string &field, const std::string &value){
    headers_[field] = value;
}
std::string HttpRequest::GetHeader(const std::string &field) const{
    std::string result;
    auto it = headers_.find(field);
    if(it!=headers_.end()){
        result = it->second;
    }
    return result;
}
const std::map<std::string, std::string> & HttpRequest::headers() const{
    return headers_;
}


void HttpRequest::SetBody(const std::string &str){
    body_ = std::move(str);
}
const std::string & HttpRequest::body() const{
    return body_;
}
