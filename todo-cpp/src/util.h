#pragma once

#include <cstring>
#include <string>

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

} // util
