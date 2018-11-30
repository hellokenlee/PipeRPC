#include "TransportPipe.h"

using namespace std;

TransportPipe::TransportPipe(HANDLE pipe) : TransportBase(), mPipe(pipe), mThread(INVALID_HANDLE_VALUE) {}

TransportPipe::~TransportPipe() {
	close();
}

void TransportPipe::close() {
	if (mPipe != INVALID_HANDLE_VALUE) {
		CloseHandle(mPipe);
	}
	if (mThread != INVALID_HANDLE_VALUE) {
		TerminateThread(mThread, 0);
		CloseHandle(mThread);
	}
	mState = NET_STATE_STOP;
	mPipe = INVALID_HANDLE_VALUE;
	mThread = INVALID_HANDLE_VALUE;
}

void TransportPipe::listen(const string& name) {
	mPipeName = name;
	DWORD id;
	mThread = CreateThread(
		NULL,
		0,
		threadLoop,
		(LPVOID)this,
		0,
		&id
	);
}
shared_ptr<TransportBase> TransportPipe::accept() {
	lock_guard<mutex> autolock(mLock);
	if (mAcptQueue.empty()) {
		return nullptr;
	}
	// 
	HANDLE client = mAcptQueue.front();
	mAcptQueue.pop();
	//
	TransportPipe* ret = new TransportPipe(client);
	ret->mState = NET_STATE_ESTABLISHED;
	return shared_ptr<TransportBase>(ret);
}

void TransportPipe::connect(const string& name) {
	close();
	mPipe = CreateFile(
		name.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);
	if (mPipe == INVALID_HANDLE_VALUE) {
		printf("[Error] Failed to connect to host (errcode = %d).\n", GetLastError());
		return;
	}
	mState = NET_STATE_ESTABLISHED;
}

void TransportPipe::send(const string& msg) {
	mSendQueue.push(msg);
}

string TransportPipe::recv() {
	if (mRecvQueue.empty()) {
		static string none = "";
		return none;
	}
	string ret = mRecvQueue.front();
	mRecvQueue.pop();
	return ret;
}

void TransportPipe::process() {
	static bool success;
	if (mPipe == INVALID_HANDLE_VALUE) {
		printf("[Error] Invali pipe.\n");
		return;
	}
	// Read message from pipe and store it to message queue.
	static DWORD length;
	length = 0;
	success = PeekNamedPipe(mPipe, NULL, 0, NULL, &length, NULL);
	if (!success) {
		checkError();
	}
	if (length > 0) {
		printf("[Info] Fetch %d bytes data.\n", length);
		DWORD read = 0;
		mRecvQueue.emplace(string(length, '\0'));
		success = ReadFile(
			mPipe,
			&(mRecvQueue.back()[0]),
			length,
			&read,
			nullptr
		);
		if (!success || read != length) {
			checkError();
		}
	}
	// Write messgae to pipe from message queue.
	while(!mSendQueue.empty()) {
		DWORD written = 0;
		success = WriteFile(
			mPipe,
			&(mSendQueue.front()[0]),
			mSendQueue.front().size(),
			&written, 
			nullptr
		);
		if (!success || written != mSendQueue.front().size()) {
			printf("[Error] While writing: failed to write (error code =%d).\n", GetLastError());
		}
		mSendQueue.pop();
	}
}

void TransportPipe::checkError() {
	if (GetLastError() == ERROR_BROKEN_PIPE) {
		printf("[Error] Lose connection (error code = %d).\n", GetLastError());
		mState = NET_STATE_DISCONNECTED;
	} else {
		printf("[Error] Error occured (error code = %d).\n", GetLastError());
	}
}

DWORD WINAPI TransportPipe::threadLoop(LPVOID pParam) {
	TransportPipe* pTransport = (TransportPipe*)pParam;
	//
	BOOL connected = FALSE;
	HANDLE pipe = INVALID_HANDLE_VALUE;
	//
	while (true) {
		// Create a new named-pipe
		pipe = CreateNamedPipe(
			pTransport->mPipeName.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			PIPE_MAX_BUFFER_SIZE,
			PIPE_MAX_BUFFER_SIZE,
			0,
			NULL
		);
		if (pipe == INVALID_HANDLE_VALUE) {
			printf("[Error] AcceptThread: Failed to create new pipe (errcode = %d).\n", GetLastError());
			return -1;
		}
		// Wait for connection...
		printf("[Info] AcceptThread: Waiting for new connection...\n");
		connected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		// Handle new client
		if (connected) {
			printf("[Info] AcceptThread: New client connected.\n");
			lock_guard<mutex> autolock(pTransport->mLock);
			pTransport->mAcptQueue.push(pipe);
			pTransport->mState = NET_STATE_CONNECTING;
		}
		else {
			printf("[Info] AcceptThread: Failed to connect new client.\n");
			CloseHandle(pipe);
		}
	}
}