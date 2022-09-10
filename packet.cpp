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


#include <cstring>

#include "packet.h"

NetworkPacket::NetworkPacket(){
};

NetworkPacket::~NetworkPacket(){
	free(m_data);
};

NetworkPacket& NetworkPacket::operator<<(uint64_t src){
	char* temp = (char*)realloc(m_data, m_datasize+sizeof(uint64_t));
	if (temp == 0)
	{
		m_error = true;
		return *this;
	}

	m_data = temp;
	src = htonll(src);
	memcpy(m_data+m_datasize, &src, sizeof(uint64_t));

	m_datasize += sizeof(uint64_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator>>(uint64_t &dst){
	memcpy(&dst, m_data+m_read_offset, sizeof(uint64_t));
	dst = ntohll(dst);

	m_read_offset += sizeof(uint64_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator<<(uint32_t src){
	char* temp = (char*)realloc(m_data, m_datasize+sizeof(uint32_t));
	if (temp == 0)
	{
		m_error = true;
		return *this;
	}

	m_data = temp;
	src = htonl(src);
	memcpy(m_data+m_datasize, &src, sizeof(uint32_t));

	m_datasize += sizeof(uint32_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator>>(uint32_t &dst){
	memcpy(&dst, m_data+m_read_offset, sizeof(uint32_t));
	dst = ntohl(dst);

	m_read_offset += sizeof(uint32_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator<<(uint16_t src){
	char* temp = (char*)realloc(m_data, m_datasize+sizeof(uint16_t));
	if (temp == 0)
	{
		m_error = true;
		return *this;
	}

	m_data = temp;
	src = htons(src);
	memcpy(m_data+m_datasize, &src, sizeof(uint16_t));

	m_datasize += sizeof(uint16_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator>>(uint16_t &dst){
	memcpy(&dst, m_data+m_read_offset, sizeof(uint16_t));
	dst = ntohs(dst);

	m_read_offset += sizeof(uint16_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator<<(uint8_t src){
	char* temp = (char*)realloc(m_data, m_datasize+sizeof(uint8_t));
	if (temp == 0)
	{
		m_error = true;
		return *this;
	}

	m_data = temp;
	memcpy(m_data+m_datasize, &src, sizeof(uint8_t));

	m_datasize += sizeof(uint8_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator>>(uint8_t &dst){
	memcpy(&dst, m_data+m_read_offset, sizeof(uint8_t));

	m_read_offset += sizeof(uint8_t);

	return *this;
};

NetworkPacket& NetworkPacket::operator<<(char* src){
	uint16_t len = strlen(src);
	char* temp = (char*)realloc(m_data, m_datasize+sizeof(len)+len);
	if (temp == 0)
	{
		m_error = true;
		return *this;
	}

	len = htons(len);
	m_data = temp;
	memcpy(m_data+m_datasize, &len, sizeof(len));
	m_datasize+=sizeof(len);

	len = ntohs(len);
	memcpy(m_data+m_datasize, src, len);
	m_datasize += len;

	return *this;
};

NetworkPacket& NetworkPacket::operator>>(char* dst){
	uint8_t null_char = 0;

	uint16_t len;
	memcpy(&len, m_data+m_read_offset, sizeof(len));
	m_read_offset += sizeof(len);

	len = ntohs(len);

	memcpy(dst, m_data+m_read_offset, len);
	m_read_offset += len;

	memcpy(dst+len, &null_char, sizeof(uint8_t));

	return *this;
};

bool NetworkPacket::add_char(char* src, uint16_t len)
{
	char* temp = (char*)realloc(m_data, m_datasize+sizeof(len)+len);
	if (temp == 0)
	{
		m_error = true;
		return false;
	}

	m_data = temp;
	len = htons(len);
	memcpy(m_data+m_datasize, &len, sizeof(len));
	m_datasize+=sizeof(len);
	len = ntohs(len);
	memcpy(m_data+m_datasize, src, len);
	m_datasize += len;

	return true;
};

bool NetworkPacket::get_char(char* dst, uint16_t &len)
{
	memcpy(&len, m_data+m_read_offset, sizeof(uint16_t));
	len = ntohs(len);
	m_read_offset += sizeof(uint16_t);

	memcpy(dst, m_data+m_read_offset, len);
	m_read_offset += len;

	return true;
};

bool NetworkPacket::reset(){
	free(m_data);

	m_data = (char*)malloc(0);

	m_datasize = 0;
	m_read_offset = 0;
	m_error = false;

	return true;
};

bool NetworkPacket::set_data(char* data, uint32_t len){
	char* temp;
	temp = (char*)malloc(len);
	if (temp == 0)
		return false;

	free(m_data);
	m_data = temp;
	memcpy(m_data, data, len);

	m_datasize = len;
	m_read_offset = 0;
	m_error = false;

	return true;
};
