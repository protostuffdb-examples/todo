#pragma once

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>

#include <brynet/net/http/HttpService.h>
#include <brynet/net/http/HttpFormat.h>

namespace rpc {

const char* const MALFORMED_MESSAGE = "Malformed message.";

const char* extractJson(std::string& body)
{
    // remove suffix: ]
    body[body.size() - 1] = '\0';
    
    // remove prefix: +[0,
    return body.data() + 4;
}

const char* extractMsg(std::string& body)
{
    size_t found = body.find('"');
    if (found == std::string::npos || '"' == body[found + 1])
        return MALFORMED_MESSAGE;
    
    // remove suffix: "}]
    body[body.size() - 3] = '\0';
    
    return body.data() + found + 1;
}

/*
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
*/

struct Base
{
    const std::string host;
    const int port;
    
    flatbuffers::Parser parser;
    std::string errmsg;
    
protected:
    brynet::net::WrapTcpService service;
    int fd{ SOCKET_ERROR };
    
    Base(const char* host, int port) : host(host), port(port)
    {
        
    }
    
    virtual void onLoop(const brynet::net::EventLoop::PTR& loop) = 0;
    std::function<void (const brynet::net::EventLoop::PTR& loop)> $onLoop =
            std::bind(&Base::onLoop, this, std::placeholders::_1);
    
    virtual void onHttpData(const brynet::net::HTTPParser& httpParser, const brynet::net::HttpSession::PTR& session) = 0;
    std::function<void (const brynet::net::HTTPParser& httpParser, const brynet::net::HttpSession::PTR& session)> $onHttpData =
            std::bind(&Base::onHttpData, this, std::placeholders::_1, std::placeholders::_2);
    
    virtual void onHttpClose(const brynet::net::HttpSession::PTR& httpSession) = 0;
    std::function<void (const brynet::net::HttpSession::PTR& httpSession)> $onHttpClose =
            std::bind(&Base::onHttpClose, this, std::placeholders::_1);
    
    virtual void onHttpOpen(const brynet::net::HttpSession::PTR& httpSession) = 0;
    
private:
    void setupHttp(const brynet::net::HttpSession::PTR& httpSession)
    {
        httpSession->setCloseCallback($onHttpClose);
        httpSession->setHttpCallback($onHttpData);
        onHttpOpen(httpSession);
    }
    std::function<void (const brynet::net::HttpSession::PTR& httpSession)> $setupHttp =
            std::bind(&Base::setupHttp, this, std::placeholders::_1);
    
    void onTcpSession(const brynet::net::TCPSession::PTR& tcpSession)
    {
        brynet::net::HttpService::setup(tcpSession, $setupHttp);
    }
    std::function<void (const brynet::net::TCPSession::PTR& tcpSession)> $onTcpSession =
            std::bind(&Base::onTcpSession, this, std::placeholders::_1);
    
protected:
    /**
     * Returns true if already connected or if the connection was successful. 
     */
    bool connect()
    {
        bool ret = SOCKET_ERROR != fd;
        
        if (!ret && SOCKET_ERROR != (fd = brynet::net::base::Connect(false, host, port)))
        {
            auto socket = brynet::net::TcpSocket::Create(fd, false);
            service.addSession(std::move(socket), $onTcpSession, false, nullptr, 1024 * 1024, false);
            ret = true;
        }
        
        return ret;
    }
    std::function<void ()> $connect = std::bind(&Base::connect, this);
    
public:
    bool isConnected()
    {
        return SOCKET_ERROR != fd;
    }
};

} // rpc
