#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#include "TransportBase.h"

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
	int addClient(std::shared_ptr<TransportBase> client);
	void delClient(unsigned int id);
protected:
	bool mIsStarted;
	std::string mName;
	TransportBase* mServer;
	std::vector<std::shared_ptr<TransportBase>> mClients;
	unsigned int generateClientID();
};