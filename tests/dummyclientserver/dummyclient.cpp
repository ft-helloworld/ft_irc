/* This is a dummy TCP socket client application.
Send numbers to the server in the sequence: 0,4,8,12,...
Receive data from the server and verify that numbers are received in the expected sequence 0,1,2,3,...

    |------|                       |------|
    |      | === 0,4,8,12,... == > |      |
    |client|                       |server|
    |      | < == 0,1,2,3,... ==== |      |
    |------|                       |------|
(dummyclient.cc)               (dummyserver.cc)

The client sends data at a lower rate than the server does (similar to most real client-server scenarios).
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <vector>
#include <string>

#include "clientserver.h"
#include "stats.h"

int main (int argc, char* argv[]) {
	fd_set readfds;
	struct timeval tv;

	// use server IP:port from cmd line args if supplied, otherwise use default values
	const char* serverIP = "127.0.0.1";
	const char* serverPort = "6667";
	const std::string botId(argv[1]);

	printf("Connecting to server %s:%s...\n", serverIP, serverPort);

	conn_t* serverconn = connect_tcp(serverIP, serverPort);
	if (!serverconn)
		return -1;

	// connection stats
	connxnstats_t* stats = stats_initialize();

	//"data" to send, and to receive and verify
	unsigned char recvBuf[2000];

	std::vector<std::string> init_packets;
	init_packets.push_back("PASS password");
	init_packets.push_back("NICK tbot" + botId);
	init_packets.push_back("USER tbot" + botId + " 0 * :bot test");
	init_packets.push_back("JOIN #42,#42Seoul,#fourtytwo,#안녕,#42서울");

	std::vector<std::string> dummy_packets;
	dummy_packets.push_back("PRIVMSG #42 :Chat-");
	dummy_packets.push_back("PRIVMSG #42Seoul :Chat-");
	dummy_packets.push_back("PRIVMSG #fourtytwo :Chat-");
	dummy_packets.push_back("PRIVMSG #안녕 :Chat-");
	dummy_packets.push_back("PRIVMSG #42서울 :Chat-");
	dummy_packets.push_back("PRIVMSG #42,#42Seoul,#fourtytwo,#안녕,#42서울 :Chat-");

	bool packetState = false;
	int packetCounter = 0;
	int x = 0;

	while (1) {
		tv.tv_sec = 0;
		tv.tv_usec = 10000; // 10ms
		FD_ZERO(&readfds);
		FD_SET(serverconn->sockfd, &readfds);

		//TODO: abstract the select logic out of the main application
		if (select(serverconn->sockfd + 1, &readfds, NULL, NULL, &tv) == -1) {
			perror("select failed\n");
			disconnect_tcp(serverconn);
			break;
		}

		// recv data if available
		if (FD_ISSET(serverconn->sockfd, &readfds)) {

			// recv data
			int retVal = recvData(serverconn, (char*)recvBuf, (int)sizeof(recvBuf));
			if (retVal <= 0)
				break;

			// report stats
			stats_reportBytesRecd(stats, retVal); //stats reporting
		}

		// send data
		if (!packetState)
		{
			for (const auto& p : init_packets)
			{
				const std::string pp = p + "\r\n";
				int retVal = sendData(serverconn, pp.data(), (int)pp.length());
				if (retVal <= 0)
					break;
				stats_reportBytesSent(stats, retVal); //stats reporting
			}
			packetState = true;
		}
		else
		{
			if (++x % 5 != 0)
				continue;
			for (const auto& p : dummy_packets)
			{
				const std::string pp = p + std::to_string(++packetCounter) + "\r\n";
				int retVal = sendData(serverconn, pp.data(), (int)pp.length());
				if (retVal <= 0)
					break;
				stats_reportBytesSent(stats, retVal); //stats reporting
			}
		}
	}

	// print final stats before exiting
	stats_finalize(stats);

	return 0;
}
