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

//#include <string>
#include <cstring>
#include <unistd.h>

#include "network.h"
#include "packet.h"
#include "srp.h"

int main(int argc, char** argv) {
	int success;
	if (argc <=4)
	{
		printf("Too few arguments! Expected 4, got %d\nUsage: %s <IP/hostname> <port> <username> <password>\n", argc-1, argv[0]);
		exit(1);
	}

	uint16_t port;
	success = sscanf(argv[2], "%hd", &port);

	if (success != 1) {
		printf("Invalid port: %s\n", argv[2]);
		exit(1);
	}

	success = CreateClient(argv[1], argv[2]);

	if (success != 0)
	{
		return 1;
	}

	NetworkPacket pkt;

	Create_Packet(pkt, 0, true);

	pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) 0;

	Send(pkt);

	uint32_t prot_id;
	uint16_t peer_id;
	uint8_t channel;

	while (m_peer_id == PEER_ID_NONEXISTENT)
	{
		prot_id = 0;
		peer_id = 0;
		channel = 0;

		while (prot_id != PROTOCOL_ID or peer_id != PEER_ID_SERVER)
		{
			pkt.reset();

			success = Recv(pkt);

			if (success == 1) // timeout, reconnect
			{
				printf("Connection timed out, retrying...\n");
				sleep(1);
				pkt.reset();
				Create_Packet(pkt, 0, true);
				pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) 0;
				Send(pkt);
			}
			else if (success > 1)
			{
				printf("Error!");
				return 1;
			}

			pkt >> prot_id >> peer_id >> channel;
		} // This shouldnt get executed more than once, but just in case...

		uint8_t packet_type;
		pkt >> packet_type;

		if (packet_type == TYPE_RELIABLE)
		{
			uint16_t seqn;
			pkt >> seqn;
			NetworkPacket resp_pkt;
			Create_Packet(resp_pkt, 0, false);
			resp_pkt << (uint8_t)TYPE_CONTROL << (uint8_t)CONTROLTYPE_ACK << seqn;
			Send(resp_pkt);

			pkt >> packet_type;
		}

		switch (packet_type)
		{
			case TYPE_CONTROL:
				uint8_t control_type;
				pkt >> control_type;

				switch(control_type)
				{
					case CONTROLTYPE_ACK:
						break;
					case CONTROLTYPE_SET_PEER_ID:
						pkt >> m_peer_id;
						printf("Peer ID: 0x%04hX\n", m_peer_id);
						break;
					case CONTROLTYPE_PING:
						break;
					case CONTROLTYPE_DISCO:
						return 1;
					default:
						break;
				}
				break;
			case TYPE_ORIGINAL:
				break;
			case TYPE_SPLIT:
				break;
			default:
				break;
		}
	} // All we care about right now is getting a peer id

	pkt.reset();

	Create_Packet(pkt, 0, false);
	pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) TOSERVER_INIT << (uint8_t) SER_VER << (uint16_t) NETPROTO_COMPRESSION_MODE << (uint16_t) PROTOCOL_VERSION << (uint16_t) MIN_PROTOCOL_VERSION << argv[3] /*Username*/;

	Send(pkt);

	bool next = false;

	while (!next)
	{
		prot_id = 0;
		peer_id = 0;
		channel = 0;

		while (prot_id != PROTOCOL_ID or peer_id != PEER_ID_SERVER)
		{
			pkt.reset();

			success = Recv(pkt);

			if (success == 1) { // timeout
				printf("Connection timed out.\n");
				return 1;
			} else if (success > 1) {
				printf("Error!");
				return 1;
			}

			pkt >> prot_id >> peer_id >> channel;
		} // This shouldnt get executed more than once, but just in case...

		uint8_t packet_type;
		pkt >> packet_type;

		if (packet_type == TYPE_RELIABLE)
		{
			uint16_t seqn;
			pkt >> seqn;
			NetworkPacket resp_pkt;
			Create_Packet(resp_pkt, 0, false);
			resp_pkt << (uint8_t)TYPE_CONTROL << (uint8_t)CONTROLTYPE_ACK << seqn;
			Send(resp_pkt);

			pkt >> packet_type;
		}

		switch (packet_type)
		{
			case TYPE_CONTROL:
				uint8_t control_type;
				pkt >> control_type;

				switch(control_type)
				{
					case CONTROLTYPE_ACK:
						break;
					case CONTROLTYPE_SET_PEER_ID:
						pkt >> m_peer_id;
						printf("Peer ID: 0x%04hX\n", m_peer_id);
						break;
					case CONTROLTYPE_PING: // sometimes it doesnt answer, if they ping us we'll resend, will redo it properly eventually
						pkt.reset();

						Create_Packet(pkt, 0, false);
						pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) TOSERVER_INIT << (uint8_t) SER_VER << (uint16_t) NETPROTO_COMPRESSION_MODE << (uint16_t) PROTOCOL_VERSION << (uint16_t) MIN_PROTOCOL_VERSION << argv[3] /*Username*/;

						Send(pkt);
						break;
					case CONTROLTYPE_DISCO:
						printf("Server disconnected.\n");
						return 1;
					default:
						break;
				}
				break;
			case TYPE_ORIGINAL:
				uint16_t command;
				pkt >> command;

				switch(command)
				{
					case TOCLIENT_HELLO:
						uint8_t server_ser_ver;
						uint16_t dep_net_comp_mode, dep_prot_ver;
						uint32_t auth_methods;
						char auth_user[64]; // 64 is more than the max, but I havent found the exact number yet
						pkt >> server_ser_ver >> dep_net_comp_mode >> dep_prot_ver >> auth_methods >> auth_user;

						printf("auth_user: %s\n", auth_user);

						if (!(auth_methods & AUTH_MECHANISM_SRP) || server_ser_ver < MIN_SER_VER || dep_net_comp_mode != NETPROTO_COMPRESSION_MODE || dep_prot_ver < MIN_PROTOCOL_VERSION)
						{
							Disconnect();
							printf("Error: Server or auth methods not supported!\nServer version: 0x%02hhX\nAuth methods: 0x%08X\n", server_ser_ver, auth_methods);
							return 2;
						}
						next = true;
						break;
					default:
						break;
				}
				break;
			case TYPE_SPLIT:
				break;
			default:
				printf("Error: Unknown Packet Type: %hhd\n", packet_type);
				break;
		}
	}

	pkt.reset();

	char uname_lower[64];
	memset(uname_lower, 0, 64);
	for (int i = 0; argv[3][i]; i++) {
		uname_lower[i] = tolower(argv[3][i]);
	}

	printf("uname_lower: %s\n", uname_lower);

	struct SRPUser *m_auth_data;

	m_auth_data = srp_user_new(SRP_SHA256, SRP_NG_2048, argv[3], uname_lower, (const unsigned char *)argv[4], strlen(argv[4]), NULL, NULL);

	char *bytes_A = 0;
	size_t len_A = 0;

	SRP_Result res = srp_user_start_authentication((struct SRPUser *) m_auth_data, NULL, NULL, 0, (unsigned char **) &bytes_A, &len_A);

	Create_Packet(pkt, 0, false);
	pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) TOSERVER_SRP_BYTES_A;
	pkt.add_char(bytes_A, len_A);
	pkt << (uint8_t) 1;

	Send(pkt);

	uint16_t len_s;
	char bytes_s[16];

	uint16_t len_B;
	char bytes_B[256];

	next = false;
	while (!next)
	{
		prot_id = 0;
		peer_id = 0;
		channel = 0;

		while (prot_id != PROTOCOL_ID or peer_id != PEER_ID_SERVER)
		{
			pkt.reset();

			success = Recv(pkt);

			if (success > 0) // timeout
			{
				printf("Error while receiving, likely timeout\n");
				return 1;
			}

			pkt >> prot_id >> peer_id >> channel;
		} // This shouldnt get executed more than once, but just in case...

		uint8_t packet_type;
		pkt >> packet_type;

		if (packet_type == TYPE_RELIABLE)
		{
			uint16_t seqn;
			pkt >> seqn;
			NetworkPacket resp_pkt;
			Create_Packet(resp_pkt, 0, false);
			resp_pkt << (uint8_t)TYPE_CONTROL << (uint8_t)CONTROLTYPE_ACK << seqn;
			Send(resp_pkt);

			pkt >> packet_type;
		}

		switch (packet_type)
		{
			case TYPE_CONTROL:
				uint8_t control_type;
				pkt >> control_type;

				switch(control_type)
				{
					case CONTROLTYPE_ACK:
						break;
					case CONTROLTYPE_SET_PEER_ID:
						pkt >> m_peer_id;
						printf("Peer ID: 0x%04hX\n", m_peer_id);
						break;
					case CONTROLTYPE_PING: // sometimes it doesnt answer, if they ping us we'll resend, will redo it properly eventually
						pkt.reset();

						Create_Packet(pkt, 0, false);

						pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) TOSERVER_SRP_BYTES_A;
						pkt.add_char(bytes_A, len_A);
						pkt << (uint8_t) 1;

						Send(pkt);
						break;
					case CONTROLTYPE_DISCO:
						printf("Server disconnected.\n");
						return 1;
					default:
						break;
				}
				break;
			case TYPE_ORIGINAL:
				uint16_t command;
				pkt >> command;

				switch(command)
				{
					case TOCLIENT_SRP_BYTES_S_B:
						pkt.get_char(bytes_s, len_s);
						pkt.get_char(bytes_B, len_B);
						next = true;
						break;
					default:
						break;
				}
				break;
			case TYPE_SPLIT:
				break;
			default:
				printf("Error: Unknown Packet Type: %hhd\n", packet_type);
				break;
		}
	}

	char* bytes_M = 0;
	size_t len_M = 0;

	srp_user_process_challenge(m_auth_data, (const unsigned char*) bytes_s, len_s, (const unsigned char*) bytes_B, len_B, (unsigned char**) &bytes_M, &len_M);

	pkt.reset();
	Create_Packet(pkt, 0, false);

	pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) TOSERVER_SRP_BYTES_M;
	pkt.add_char(bytes_M, len_M);

	Send(pkt);

	next = false;
	while (!next)
	{
		prot_id = 0;
		peer_id = 0;
		channel = 0;

		while (prot_id != PROTOCOL_ID or peer_id != PEER_ID_SERVER)
		{
			pkt.reset();

			success = Recv(pkt);

			if (success > 0) // timeout
			{
				printf("Error while receiving, likely timeout\n");
				return 1;
			}

			pkt >> prot_id >> peer_id >> channel;
		} // This shouldnt get executed more than once, but just in case...

		uint8_t packet_type;
		pkt >> packet_type;

		if (packet_type == TYPE_RELIABLE)
		{
			uint16_t seqn;
			pkt >> seqn;
			NetworkPacket resp_pkt;
			Create_Packet(resp_pkt, 0, false);
			resp_pkt << (uint8_t)TYPE_CONTROL << (uint8_t)CONTROLTYPE_ACK << seqn;
			Send(resp_pkt);

			pkt >> packet_type;
		}

		switch (packet_type)
		{
			case TYPE_CONTROL:
				uint8_t control_type;
				pkt >> control_type;

				switch(control_type)
				{
					case CONTROLTYPE_ACK:
						break;
					case CONTROLTYPE_SET_PEER_ID:
						pkt >> m_peer_id;
						printf("Peer ID: 0x%04hX\n", m_peer_id);
						break;
					case CONTROLTYPE_PING:
						break;
					case CONTROLTYPE_DISCO:
						return 1;
					default:
						break;
				}
				break;
			case TYPE_ORIGINAL:
				uint16_t command;
				pkt >> command;
				switch(command)
				{
					case TOCLIENT_ACCESS_DENIED:
						printf("Access denied.\n");
						return 2;
					case TOCLIENT_AUTH_ACCEPT:
						printf("Access granted.\n");
						next = true;
					default:
						break;
				}
				break;
			case TYPE_SPLIT:
				break;
			default:
				break;
		}
	}

	pkt.reset();
	Create_Packet(pkt, 0, false);
	pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) TOSERVER_INIT2;
	Send(pkt);

	pkt.reset();
	Create_Packet(pkt, 0, false);
	pkt << (uint8_t) TYPE_ORIGINAL << (uint16_t) TOSERVER_CLIENT_READY << (uint8_t) 0x05 << (uint8_t) 0x05 << (uint8_t) 0x00 << (uint8_t) 0x00 << (char*) "5.5.0-Test_User";
	Send(pkt);

	while (true)
	{
		prot_id = 0;
		peer_id = 0;
		channel = 0;

		while (prot_id != PROTOCOL_ID or peer_id != PEER_ID_SERVER)
		{
			pkt.reset();

			success = Recv(pkt);

			if (success > 0) // timeout
			{
				printf("Error while receiving, likely timeout\n");
				return 1;
			}

			pkt >> prot_id >> peer_id >> channel;
		} // This shouldnt get executed more than once, but just in case...

		uint8_t packet_type;
		pkt >> packet_type;

		if (packet_type == TYPE_RELIABLE)
		{
			uint16_t seqn;
			pkt >> seqn;
			NetworkPacket resp_pkt;
			Create_Packet(resp_pkt, 0, false);
			resp_pkt << (uint8_t)TYPE_CONTROL << (uint8_t)CONTROLTYPE_ACK << seqn;
			Send(resp_pkt);

			pkt >> packet_type;
		}

		switch (packet_type)
		{
			case TYPE_CONTROL:
				uint8_t control_type;
				pkt >> control_type;

				switch(control_type)
				{
					case CONTROLTYPE_ACK:
						break;
					case CONTROLTYPE_SET_PEER_ID:
						pkt >> m_peer_id;
						printf("Peer ID: 0x%04hX\n", m_peer_id);
						break;
					case CONTROLTYPE_PING:
						break;
					case CONTROLTYPE_DISCO:
						return 1;
					default:
						break;
				}
				break;
			case TYPE_ORIGINAL:
				break;
			case TYPE_SPLIT: // Now I've got to deal with these...
				uint16_t split_seqnum, chunk_count, chunk_num;
				pkt >> split_seqnum >> chunk_count >> chunk_num;
		}
	}

	return 0; // should never reach here, its a while true loop
};
