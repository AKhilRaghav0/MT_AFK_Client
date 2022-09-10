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

#pragma once

#include "packet.h"

#define PROTOCOL_ID 0x4f457403
#define PROTOCOL_VERSION 40
#define MIN_PROTOCOL_VERSION 37 // Will attempt to reduce this number eventually
#define NETPROTO_COMPRESSION_MODE 0

#define BASE_HEADER_SIZE 7
#define PEER_ID_NONEXISTENT 0
#define PEER_ID_SERVER 1
#define CHANNEL_COUNT 3

#define TYPE_CONTROL 0
#define TYPE_CONTROL 0
#define CONTROLTYPE_ACK 0
#define CONTROLTYPE_SET_PEER_ID 1
#define CONTROLTYPE_PING 2
#define CONTROLTYPE_DISCO 3

#define TYPE_ORIGINAL 1
#define ORIGINAL_HEADER_SIZE 1

#define TYPE_SPLIT 2

#define TYPE_RELIABLE 3
#define RELIABLE_HEADER_SIZE 3

#define SEQNUM_INITIAL 65500

#define SER_VER 28
#define MIN_SER_VER 0 // to be updated

extern uint16_t m_peer_id;

enum ToClient
{
	TOCLIENT_HELLO = 0x02,
	TOCLIENT_AUTH_ACCEPT = 0x03,
	TOCLIENT_ACCEPT_SUDO_MODE = 0x04,
	TOCLIENT_DENY_SUDO_MODE = 0x05,
	TOCLIENT_ACCESS_DENIED = 0x0A,
	TOCLIENT_INIT_LEGACY = 0x10,
	TOCLIENT_BLOCKDATA = 0x20,
	TOCLIENT_ADDNODE = 0x21,
	TOCLIENT_REMOVENODE = 0x22,
	TOCLIENT_PLAYERPOS = 0x23,
	TOCLIENT_PLAYERINFO = 0x24,
	TOCLIENT_OPT_BLOCK_NOT_FOUND = 0x25,
	TOCLIENT_SECTORMETA = 0x26,
	TOCLIENT_INVENTORY = 0x27,
	TOCLIENT_OBJECTDATA = 0x28,
	TOCLIENT_TIME_OF_DAY = 0x29,
	TOCLIENT_CSM_RESTRICTION_FLAGS = 0x2A,
	TOCLIENT_PLAYER_SPEED = 0x2B,
	TOCLIENT_MEDIA_PUSH = 0x2C,
	TOCLIENT_CHAT_MESSAGE = 0x2F,
	TOCLIENT_CHAT_MESSAGE_OLD = 0x30,
	TOCLIENT_ACTIVE_OBJECT_REMOVE_ADD = 0x31,
	TOCLIENT_ACTIVE_OBJECT_MESSAGES = 0x32,
	TOCLIENT_HP = 0x33,
	TOCLIENT_MOVE_PLAYER = 0x34,
	TOCLIENT_ACCESS_DENIED_LEGACY = 0x35,
	TOCLIENT_FOV = 0x36,
	TOCLIENT_DEATHSCREEN = 0x37,
	TOCLIENT_MEDIA = 0x38,
	TOCLIENT_TOOLDEF = 0x39,
	TOCLIENT_NODEDEF = 0x3A,
	TOCLIENT_CRAFTITEMDEF = 0x3B,
	TOCLIENT_ANNOUNCE_MEDIA = 0x3C,
	TOCLIENT_ITEMDEF = 0x3D,
	TOCLIENT_PLAY_SOUND = 0x3F,
	TOCLIENT_STOP_SOUND = 0x40,
	TOCLIENT_PRIVLEDGES = 0x41,
	TOCLIENT_INVENTORY_FORMSPEC = 0x42,
	TOCLIENT_DETACHED_INVENTORY = 0x43,
	TOCLIENT_SHOW_FORMSPEC = 0x44,
	TOCLIENT_MOVEMENT = 0x45,
	TOCLIENT_SPAWN_PARTICLE = 0x46,
	TOCLIENT_ADD_PARTICLESPAWNER = 0x47,
	TOCLIENT_DELETE_PARTICLESPAWNER_LEGACY = 0x48,
	TOCLIENT_HUDADD = 0x49,
	TOCLIENT_HUDRM = 0x4A,
	TOCLIENT_HUDCHANGE = 0x4B,
	TOCLIENT_HUD_SET_FLAGS = 0x4C,
	TOCLIENT_HUD_SET_PARAM = 0x4D,
	TOCLIENT_BREATH = 0x4E,
	TOCLIENT_SET_SKY = 0x4F,
	TOCLIENT_OVERRIDE_DAY_NIGHT_RATIO = 0x50,
	TOCLIENT_LOCAL_PLAYER_ANIMATIONS = 0x51,
	TOCLIENT_EYE_OFFSET = 0x52,
	TOCLIENT_DELETE_PARTICLESPAWNER = 0x53,
	TOCLIENT_CLOUD_PARAMS = 0x54,
	TOCLIENT_FADE_SOUND = 0x55,
	TOCLIENT_UPDATE_PLAYER_LIST = 0x56,
	TOCLIENT_MODCHANNEL_MSG = 0x57,
	TOCLIENT_MODCHANNEL_SIGNAL = 0x58,
	TOCLIENT_NODEMETA_CHANGED = 0x59,
	TOCLIENT_SET_SUN = 0x5A,
	TOCLIENT_SET_MOON = 0x5B,
	TOCLIENT_SET_STARS = 0x5C,
	TOCLIENT_SRP_BYTES_S_B = 0x60,
	TOCLIENT_FORMSPEC_PREPEND = 0x61,
	TOCLIENT_MINIMAP_MODES = 0x62,
	TOCLIENT_NUM_MSG_TYPES = 0x63,
};

enum ToServer
{
	TOSERVER_INIT = 0x02,
	TOSERVER_INIT_LEGACY = 0x10,
	TOSERVER_INIT2 = 0x11,
	TOSERVER_MODCHANNEL_JOIN = 0x17,
	TOSERVER_MODCHANNEL_LEAVE = 0x18,
	TOSERVER_MODCHANNEL_MSG = 0x19,
	TOSERVER_GETBLOCK = 0x20,
	TOSERVER_ADDNODE = 0x21,
	TOSERVER_REMOVENODE = 0x22,
	TOSERVER_PLAYERPOS = 0x23,
	TOSERVER_GOTBLOCKS = 0x24,
	TOSERVER_DELETEDBLOCKS = 0x25,
	TOSERVER_ADDNODE_FROM_INVENTORY = 0x26,
	TOSERVER_CLICK_OBJECT = 0x27,
	TOSERVER_GROUND_ACTION = 0x28,
	TOSERVER_RELEASE = 0x29,
	TOSERVER_SIGNTEXT = 0x30,
	TOSERVER_INVENTORY_ACTION = 0x31,
	TOSERVER_CHAT_MESSAGE = 0x32,
	TOSERVER_SIGNNODETEXT = 0x33,
	TOSERVER_CLICK_ACTIVEOBJECT = 0x34,
	TOSERVER_DAMAGE = 0x35,
	TOSERVER_PASSWORD_LEGACY = 0x36,
	TOSERVER_PLAYERITEM = 0x37,
	TOSERVER_RESPAWN = 0x38,
	TOSERVER_INTERACT = 0x39,
	TOSERVER_REMOVED_SOUNDS = 0x3A,
	TOSERVER_NODEMETA_FIELDS = 0x3B,
	TOSERVER_INVENTORY_FIELDS = 0x3C,
	TOSERVER_REQUEST_MEDIA = 0x40,
	TOSERVER_RECIEVED_MEDIA = 0x41,
	TOSERVER_BREATH = 0x42,
	TOSERVER_CLIENT_READY = 0x43,
	TOSERVER_FIRST_SRP = 0x50,
	TOSERVER_SRP_BYTES_A = 0x51,
	TOSERVER_SRP_BYTES_M = 0x52,
	TOSERVER_NUM_MSG_TYPES = 0x53,
};

enum AuthMechanism
{
	AUTH_MECHANISM_NONE = 0,
	AUTH_MECHANISM_LEGACY_PASSWORD = 1 << 0,
	AUTH_MECHANISM_SRP = 1 << 1,
	AUTH_MECHANISM_FIRST_SRP = 1 << 2,
};

int CreateClient(char* address, char* port);
int Send(NetworkPacket &pkt);
int Create_Packet(NetworkPacket &pkt, uint8_t channel, bool reliable);
int Recv(NetworkPacket &pkt);
int Disconnect();