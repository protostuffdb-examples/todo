#pragma once

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>

#include "UrlRequest.h"

namespace rpc {

const char* extractJson(std::string& body)
{
    // remove suffix: ]
    body[body.size() - 1] = '\0';
    
    // remove prefix: +[0,
    return body.c_str() + 4;
}

const char* extractMsg(std::string& body)
{
    // TODO
    return body.c_str();
}

bool fetchInitialTodos(UrlRequest& req, flatbuffers::Parser& parser, std::string& errmsg)
{
    req.uri("/todo/user/Todo/list")
        .method("POST")
        .addHeader("Content-Type: application/json")
        .body(R"({"1":true,"2":31})");
    
    auto res = std::move(req.perform());
    auto body = res.body();
    
    if (200 != res.statusCode())
    {
        errmsg.assign("Request failed.");
        return false;
    }
    
    if ('+' != body[0])
    {
        errmsg.assign(body.c_str() + 1);
        return false;
    }
    
    if (!parser.SetRootType("Todo_PList") || !parser.ParseJson(extractJson(body), true))
    {
        errmsg.assign("Malformed message.");
        return false;
    }
    
    return true;
}

} // rpc
