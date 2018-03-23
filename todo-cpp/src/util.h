#pragma once

#include <coreds/mc.h>

namespace util {

inline void trim(std::string& str)
{
    str.erase(0, str.find_first_not_of(' ')); // space prefix
    str.erase(str.find_last_not_of(' ') + 1); // space suffix
}

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

// {1: "CgAAAAAAAAMZ", 3: "ss"}
void appendCreateReqTo(std::string& buf, const char* key, const std::string& title)
{
    if (key)
    {
        buf += R"({"1":")";
        buf.append(key, 12);
        buf += R"(","3":")";
    }
    else
    {
        buf += R"({"3":")";
    }
    
    coreds::appendJsonStrTo(buf, title);
    buf += R"("})";
}

void appendUpdateReqTo(std::string& buf, const char* key, coreds::MultiCAS& mc)
{
    buf += R"({"1":")";
    buf.append(key, 12);
    buf += R"(","2":)";
    mc.stringifyTo(buf);
    buf += '}';
}

// {1: "CgAAAAAAAAMZ", 2: {1: [{1: 4, 2: false, 3: true}]}}
void appendUpdateReqTo(std::string& buf, const char* key, int field, bool newVal)
{
    buf += R"({"1":")";
    buf.append(key, 12);
    buf += R"(","2":{"1":[{"1":)";
    buf += std::to_string(field);
    if (newVal)
        buf += R"(,"2":false,"3":true}]}})";
    else
        buf += R"(,"2":true,"3":false}]}})";
}

// {1: "CgAAAAAAAAMZ", 2: {3: [{1: 3, 2: "foo", 3: "bar"}]}}
void appendUpdateReqTo(std::string& buf, const char* key, int field, std::string& newVal, std::string& oldVal)
{
    buf += R"({"1":")";
    buf.append(key, 12);
    buf += R"(","2":{"3":[{"1":)";
    buf += std::to_string(field);
    buf += R"(,"2":")";
    coreds::appendJsonStrTo(buf, oldVal);
    buf += R"(","3":")";
    coreds::appendJsonStrTo(buf, newVal);
    buf += R"("}]}})";
}

} // util
