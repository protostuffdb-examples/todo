#pragma once

namespace util {

const int MARGIN = 5,
        #ifdef WIN32
        HEIGHT = 750,
        #else
        HEIGHT = 710,
        #endif
        WIDTH = 1005;

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

// {1: "CgAAAAAAAAMZ", 2: {1: [{1: 4, 2: false, 3: true}]}}
void appendUpdateReqTo(std::string& buf, const char* key, int field, bool newVal)
{
    buf += R"({"1":")";
    buf.append(key, 12);
    buf += R"(","2":{"1":[{"1":)";
    buf += std::to_string(field);
    buf += R"(,"2":)";
    buf += newVal ? "false" : "true";
    buf += R"(,"3":)";
    buf += !newVal ? "false" : "true";
    buf += R"(}]}})";
}

template <typename T>
struct HasState
{
    virtual void update(T state) = 0;
};

} // util
