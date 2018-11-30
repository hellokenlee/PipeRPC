#include "Server.h"
#include "TransportPipe.h"

using namespace std;

string dispose(string data) {
	static string reply = "[ACK]";
	printf("[Info] Receive %d length data.\n", data.size());
	return reply;
}

Server::Server(std::string name) {
	mName = name;
	mServer = new TransportPipe();
}

Server::~Server() {
	if (mServer != nullptr) {
		delete mServer;
	}
}

void Server::startup() {
	mIsStarted = true;
	mServer->listen(mName);
}

void Server::shutdown() {
	// Close all clients
	mClients.clear();
	// Restore flag
	mIsStarted = false;
}

void Server::processLoop() {
	//
	if (!mIsStarted) {
		return;
	}
	//
	string message;
	vector<int> invalids(0);
	//
	while (true) {
		// Add new clients
		addClient(mServer->accept());
		// Update clients
		for (unsigned int id = 0; id < mClients.size(); ++id) {
			shared_ptr<TransportBase> client = mClients[id];
			if (client != nullptr) {
				// Update
				client->process();
				// Read message
				message = client->recv();
				// Process message
				if (message.size() != 0) {
					message = dispose(message);
				}
				// Write message
				if (message.size() != 0) {
					client->send(message);
				}
				// Check if close
				if (client->state() == NET_STATE_DISCONNECTED) {
					printf("[Info] One client(id = %d) dissconncted.\n", id);
					client->close();
					invalids.push_back(id);
				}
			}
		}
		// Delete invalid clients
		for (auto id : invalids) {
			delClient(id);
		}
		invalids.clear();
		//
		Sleep(1);
	}
}

int Server::addClient(shared_ptr<TransportBase> client) {
	int id = generateClientID();
	mClients[id] = client;
	return id;
}

void Server::delClient(unsigned int id) {
	//
	if (id < mClients.size()) {
		mClients[id] = nullptr;
	}
}

unsigned int Server::generateClientID() {
	for (unsigned int i = 0; i < mClients.size(); ++i) {
		if (mClients[i] == nullptr) {
			return i;
		}
	}
	mClients.push_back(nullptr);
	return mClients.size() - 1;
}

