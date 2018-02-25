#pragma once

#include <cstring>
#include <string>

#include <chrono>

namespace util {

const char* const DEFAULT_HOST = "127.0.0.1";

const char* resolveIpPort(char* arg, int* port)
{
    if (arg == nullptr)
    {
        return DEFAULT_HOST;
    }
    
    char* colon = std::strchr(arg, ':');
    if (colon == nullptr)
    {
        return arg;
    }
    
    if (':' == *arg)
    {
        // assume local host
        *port = std::atoi(arg + 1);
        return DEFAULT_HOST;
    }
    
    *(colon++) = '\0';
    *port = std::atoi(colon);
    
    return arg;
}

const char* resolveEndpoint(char* arg, int* port, bool* secure)
{
    if (arg == nullptr)
    {
        *secure = false;
        return DEFAULT_HOST;
    }
    
    char* slash = std::strchr(arg, '/');
    if (slash == nullptr)
    {
        *secure = false;
        return resolveIpPort(arg, port);
    }
    
    if (':' != *(slash - 1))
    {
        // expect a trailing slash
        if ('\0' != *(slash + 1))
            return nullptr;
        
        *secure = false;
        *slash = '\0';
        return resolveIpPort(arg, port); 
    }
    
    if ('/' != *(slash + 1))
    {
        // expecting ://
        return nullptr;
    }
    
    *secure = 's' == *(slash - 2);
    arg = slash + 2;
    return resolveIpPort(arg, port);
}

static constexpr uint64_t SECONDS_IN_MINUTE = 60;
static constexpr uint64_t SECONDS_IN_HOUR = SECONDS_IN_MINUTE * 60;
static constexpr uint64_t SECONDS_IN_DAY = SECONDS_IN_HOUR * 24;
static constexpr uint64_t SECONDS_IN_MONTH = SECONDS_IN_DAY * 30;
static constexpr uint64_t SECONDS_IN_YEAR = SECONDS_IN_DAY * 365;
static constexpr uint64_t SECONDS_IN_WEEK = SECONDS_IN_DAY * 7;

static int appendTo(std::string& str, int seconds, int count,
        const std::string& singular,
        const std::string& plural = "")
{
    long r = static_cast<unsigned>(seconds) / count;
    if (!r)
        return seconds;
    
    if (!str.empty() && str[str.length() -1] != '-')
        str += ", ";
    
    str += std::to_string(r);
    str += ' ';
    if (r < 2)
    {
        str += singular;
    }
    else if (plural.empty())
    {
        str += singular;
        str += 's';
    }
    else
    {
        str += plural;
    }
    
    return static_cast<unsigned>(seconds) % count;
}

void appendTimeagoTo(std::string& str, uint64_t ts)
{
    int64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int64_t diff = now - ts;
    if (diff == 0)
    {
        str += "just moments ago";
        return;
    }
    
    int seconds = diff / 1000;
    if (seconds < 0)
        seconds = -seconds;
    
    // TODO: l10n i18n?
    if (-1 != (seconds = appendTo(str, seconds, SECONDS_IN_YEAR, "year")) && str.empty() &&
            -1 != (seconds = appendTo(str, seconds, SECONDS_IN_MONTH, "month")) && str.empty() &&
            -1 != (seconds = appendTo(str, seconds, SECONDS_IN_WEEK, "week")) && str.empty() &&
            -1 != (seconds = appendTo(str, seconds, SECONDS_IN_DAY, "day")) && str.empty() &&
            -1 != (seconds = appendTo(str, seconds, SECONDS_IN_HOUR, "hour")) && str.empty() &&
            -1 != (seconds = appendTo(str, seconds, SECONDS_IN_MINUTE, "minute")) && str.empty())
    {
        appendTo(str, seconds, 1, "second");
    }
    
    if (diff < 0)
        str += " from now";
    else
        str += " ago";
}

} // util
