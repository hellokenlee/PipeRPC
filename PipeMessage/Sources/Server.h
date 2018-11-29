#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include "TransportPipe.h"

class Server {
public:
	//
	Server(std::string name);
	~Server();
	//
	void startup();
	void shutdown();
	//
	void processLoop();
	//
	void addClient(HANDLE pipe);
	void delClient(HANDLE pipe);
protected:
	bool mIsStarted;
	std::string mName;
	TransportPipe mPipe;
	std::map<HANDLE, TransportBase*> mClients;
};