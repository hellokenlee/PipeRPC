#pragma once

#include <Windows.h>
#include <string>

#include "NetState.h"

class TransportBase {
	//
	virtual HANDLE accept() = 0;
	virtual void listen(const std::string& name) = 0;
	//
	virtual void close() = 0;
	virtual void assign(HANDLE handle) = 0;
	virtual void connect(const std::string& name) = 0;
	//
	virtual void send(const std::string& msg) = 0;
	virtual std::string recv() = 0;
	//
	virtual void process() = 0;
	virtual NetState state() = 0;
};