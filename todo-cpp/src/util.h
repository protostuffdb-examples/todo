#pragma once

#include <flatbuffers/util.h>

namespace util {

const int MARGIN = 5,
        LEFT = 360,
        #ifdef WIN32
        HEIGHT = 750,
        #else
        HEIGHT = 710,
        #endif
        WIDTH = 1005,
        // page
        PAGE_SIZE = 20,
        MULTIPLIER = 2;

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

void appendStrTo(std::string& text, const std::string &src)
{
    const char* utf8;
    int ucc;
    char c;
    //text += '"';
    for (size_t i = 0, len = src.size(); i < len; i++)
    {
        switch ((c = src[i]))
        {
            case '\n': text += "\\n"; break;
            case '\t': text += "\\t"; break;
            case '\r': text += "\\r"; break;
            case '\b': text += "\\b"; break;
            case '\f': text += "\\f"; break;
            case '\"': text += "\\\""; break;
            case '\\': text += "\\\\"; break;
            default:
                if (c >= ' ' && c <= '~')
                {
                    text += c;
                }
                else
                {
                    // Not printable ASCII data. Let's see if it's valid UTF-8 first:
                    utf8 = src.data() + i;
                    ucc = flatbuffers::FromUTF8(&utf8);
                    if (ucc >= 0x80 && ucc <= 0xFFFF)
                    {
                        // Parses as Unicode within JSON's \uXXXX range, so use that.
                        text += "\\u";
                        text += flatbuffers::IntToStringHex(ucc, 4);
                        // Skip past characters recognized.
                        i = (utf8 - src.data() - 1);
                    }
                    else
                    {
                        // It's either unprintable ASCII, arbitrary binary, or Unicode data
                        // that doesn't fit \uXXXX, so use \xXX escape code instead.
                        text += "\\x";
                        text += flatbuffers::IntToStringHex(static_cast<uint8_t>(c), 2);
                    }
                }
                break;
        }
    }
    //text += '"';
}

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
    
    appendStrTo(buf, title);
    buf += R"("})";
}

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
