#include "Server.h"

using namespace std;

string dispose(string data) {
	static string reply = "[ACK]";
	printf("[Info] Receive %d length data.\n", data.size());
	return reply;
}

Server::Server(std::string name) {
	mName = name;
}

Server::~Server() {
	for (auto pair : mClients) {
		pair.second->close();
		delete pair.second;
	}
}

void Server::startup() {
	mIsStarted = true;
	mPipe.listen(mName);
}

void Server::shutdown() {
	// Close all clients
	mClients.clear();
	// Restore members
	mIsStarted = false;
}

void Server::processLoop() {
	//
	if (!mIsStarted) {
		return;
	}
	//
	string message;
	vector<HANDLE> invalids(0);
	//
	while (true) {
		// Add new clients
		HANDLE c = mPipe.accept();
		if (c != INVALID_HANDLE_VALUE) {
			addClient(c);
		}
		// Update clients
		for (auto it = mClients.begin(); it != mClients.end(); ++it) {
			// Update
			it->second->process();
			// Read message
			message = it->second->recv();
			// Process message
			if (message.size() != 0) {
				message = dispose(message);
			}
			// Write message
			if (message.size() != 0) {
				it->second->send(message);
			}
			// Check if close
			if (it->second->state() == NET_STATE_DISCONNECTED) {
				printf("[Info] One client dissconncted.\n");
				it->second->close();
				invalids.push_back(it->first);
			}
			//
			Sleep(1);
		}
		// Delete invalid clients
		for (auto h : invalids) {
			delClient(h);
		}
		invalids.clear();
	}
}

void Server::addClient(HANDLE pipe) {
	TransportPipe* pNew = new TransportPipe;
	pNew->assign(pipe);
	mClients.insert(make_pair(pipe, pNew));
}

void Server::delClient(HANDLE pipe) {
	auto it = mClients.find(pipe);
	if (it != mClients.end()) {
		delete mClients.at(pipe);
		mClients.erase(pipe);
	}
}

