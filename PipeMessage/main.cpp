
#include "Sources/Server.h"
#include <iostream>

using namespace std;

#define PIPE_NAME "\\\\.\\pipe\\mynamedpipe"


int main() {
	char x = 'o';
	while (x != 's' && x != 'c') {
		printf("Select to start Client[c]/Server[s]: ");
		x = getchar();
	}
	if (x == 's') {
		Server s(PIPE_NAME);
		s.startup();
		s.processLoop();
		s.shutdown();
	}
	if (x == 'c') {
		TransportPipe client;
		client.connect(PIPE_NAME);
		int size = 0;
		while (true) {
			printf("Enter message size to send: ");
			cin >> size;
			client.send(string(size, 'x'));
			auto t = clock();
			string reply = "";
			while (reply.size() == 0) {
				client.process();
				reply = client.recv();
			}
			printf("Reply: %s, RTT: %lf s\n", string(reply.begin(), reply.end()).c_str(), (double)(clock() - t) / (double)CLOCKS_PER_SEC);
		}
	}
	return 0;
}