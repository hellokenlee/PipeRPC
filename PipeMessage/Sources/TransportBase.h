#pragma once

#include <Windows.h>
#include <string>
#include <memory>
#include "NetState.h"

class TransportBase {
public:
	TransportBase() : mState(NET_STATE_STOP) {};
	virtual ~TransportBase() {};
	// 
	virtual std::shared_ptr<TransportBase> accept() = 0;
	virtual void listen(const std::string& name) = 0;
	//
	virtual void close() = 0;
	virtual void connect(const std::string& name) = 0;
	//
	virtual void send(const std::string& msg) = 0;
	virtual std::string recv() = 0;
	//
	virtual void process() = 0;
	virtual inline NetState state() { return mState; };
protected:
	NetState mState;
};