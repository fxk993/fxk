#pragma once
#include<boost/asio.hpp>
#include<boost/beast.hpp>
#include<boost/beast/http.hpp>
#include<iostream>
#include<memory>
#include<functional>

class GateServer:public std::enable_shared_from_this<GateServer>
{
public:
	GateServer(boost::asio::io_context& ioc,unsigned short port);
	void Start();
public:
	~GateServer();
private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::io_context& _ioc;
	boost::asio::ip::tcp::socket _socket;
};
