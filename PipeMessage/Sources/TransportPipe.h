#pragma once
#include <Windows.h>
#include <string>
#include <queue>
#include <mutex>
#include "NetState.h"
#include "TransportBase.h"

#define PIPE_MAX_BUFFER_SIZE 1024

class TransportPipe : public TransportBase {
public:
	TransportPipe(HANDLE pipe = INVALID_HANDLE_VALUE);
	virtual ~TransportPipe();
	//
	virtual std::shared_ptr<TransportBase> accept();
	virtual void listen(const std::string& name);
	//
	virtual void close();
	virtual void connect(const std::string& name);
	//
	virtual void send(const std::string& msg);
	virtual std::string recv();
	//
	virtual void process();
protected:
	HANDLE mPipe;
	HANDLE mThread;
	std::mutex mLock;
	std::string mPipeName;
	std::queue<HANDLE> mAcptQueue;
	std::queue<std::string> mSendQueue;
	std::queue<std::string> mRecvQueue;
private:
	void checkError();
	static DWORD WINAPI threadLoop(LPVOID pTransport);
};