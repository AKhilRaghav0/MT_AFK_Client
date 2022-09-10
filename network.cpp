/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

HIGHLY MODIFIED FROM THE ORIGINAL CODE
*/

#include <string>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network.h"

uint16_t m_peer_id;

int sockfd = 0;

struct sockaddr_in server;

uint16_t seqnum_reliable[3];
uint16_t seqnum_split;

int Resolve(char* address, char* port, in_addr_t &ip) {
	int success;
	struct addrinfo hints, *servinfo, *p;

	bzero(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	success = getaddrinfo(address, port , &hints, &servinfo);

	if (success != 0)
	{
		printf("Resolve hostname: failed.\n");
		return 1;
	} else {
		printf("Resolve hostname: success.\n");
	}

	for (p=servinfo; p != NULL; p = p->ai_next)
	{
		if ( ((struct sockaddr_in*) p->ai_addr)->sin_addr.s_addr != INADDR_ANY)
		{
			ip = ((struct sockaddr_in*) p->ai_addr)->sin_addr.s_addr;
			break;
		}
	}

	return 0;
};

int CreateClient(char* address, char* port) {
	int success, i;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		printf("Error while creating socket!\n");
		return 1;
	}

	struct timeval timeout;
	timeout.tv_sec = 30;
	timeout.tv_usec = 0;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

	bzero(&server, sizeof(server));

	in_addr_t ip;

	success = inet_pton(AF_INET, address, &ip);

	printf("Address: %s\n", address);

	if (success != 1)
	{
		printf("Direct conversion: failed.\n");
		success = Resolve(address, port, ip);
	} else {
		printf("Direct conversion: success.\n");
		success = 0;
	}

	if (success != 0)
	{
		printf("Unable to proccess address: %s\n", address);
		return 2;
	}

	uint16_t iport;
	sscanf(port, "%hd", &iport);

	server.sin_addr.s_addr = ip;
	server.sin_family = AF_INET;
	server.sin_port=htons(iport);

	connect(sockfd, (sockaddr*)&server, sizeof(server));

	m_peer_id = PEER_ID_NONEXISTENT;

	for (i=0;i<CHANNEL_COUNT;i++){
		seqnum_reliable[i] = SEQNUM_INITIAL;
	}
	seqnum_split = SEQNUM_INITIAL;

	return 0;
};

int Send(NetworkPacket &pkt) {
	int len_sent = send(sockfd, pkt.get_data(), pkt.get_size(), 0);
	if (len_sent >= 1)
	{
		return 0;
	} else {
		return 1;
	}
};

int Recv(NetworkPacket &pkt) {
	int len;
	char buf[512];

	len = recv(sockfd, buf, 512, 0);

	while (len == 0)
		len = recv(sockfd, buf, 512, 0);
	if (len < 0) {
		printf("%04X\n", errno);
		return 1;
	}

	bool success = pkt.set_data(buf, len);

	if (!success)
		return 2;

	return 0;
};

int Create_Packet(NetworkPacket &pkt, uint8_t channel, bool reliable) {
	if (channel>=CHANNEL_COUNT)
		return 1;

	pkt << (uint32_t)PROTOCOL_ID << m_peer_id << channel;

	if (!reliable)
		return 0;

	pkt << (uint8_t)TYPE_RELIABLE << seqnum_reliable[channel];
	seqnum_reliable[channel]++;

	return 0;
};

int Disconnect() {
	NetworkPacket pkt;
	Create_Packet(pkt, 0, false);
	pkt << (uint8_t) TYPE_CONTROL << (uint8_t) CONTROLTYPE_DISCO;
	Send(pkt);
	close(sockfd);
	return 0;
};
