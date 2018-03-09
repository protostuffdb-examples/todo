#pragma once

namespace util {

struct Request
{
    const char* uri;
    const std::string body;
    const char* res_type;
    std::string* errmsg;
    std::function<void(void* res)> cb;
    
    Request(const char* uri, const std::string body,
            const char* res_type, std::string* errmsg, std::function<void(void* res)> cb):
        uri(uri),
        body(body),
        res_type(res_type),
        errmsg(errmsg),
        cb(cb)
    {
        
    }
};

struct RequestQueue
{
    std::queue<Request> queue;
    std::function<void()> send;
};

} // util
