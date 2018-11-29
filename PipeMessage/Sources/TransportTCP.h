#pragma once

#include "TransportBase.h"

class TransportTCP : public TransportBase{
	//
	virtual HANDLE accept();
	virtual void listen(const std::string& address);
	//
	virtual void close();
	virtual void assign(HANDLE handle);
	virtual void connect(const std::string& address);
	//
	virtual void send(const std::string& msg);
	virtual std::string recv();
	//
	virtual void process();
	virtual NetState state();
};