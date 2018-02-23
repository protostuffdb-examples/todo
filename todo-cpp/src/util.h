#pragma once

#include <cstring>
#include <string>

namespace util {

const char* const DEFAULT_HOST = "127.0.0.1";
const int DEFAULT_PORT = 5000;

const char* resolveIpPort(char* arg, int* port)
{
    if (arg == nullptr)
    {
        *port = DEFAULT_PORT;
        return DEFAULT_HOST;
    }
    
    char* colon = std::strchr(arg, ':');
    if (colon == nullptr)
    {
        *port = DEFAULT_PORT;
        return arg;
    }
    
    if (':' == *arg)
    {
        *port = std::atoi(arg + 1);
        return DEFAULT_HOST;
    }
    
    *(colon++) = '\0';
    *port = std::atoi(colon);
    
    return arg;
}

} // util
