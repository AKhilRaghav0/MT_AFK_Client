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

#include <string>
#include <arpa/inet.h>

#if __BIG_ENDIAN__
# define htonll (x) (x)
# define ntohll (x) (x)
#else
# define htonll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((uint32_t)((uint64_t)(x) >> 32)))
# define ntohll(x) (((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((uint32_t)((uint64_t)(x) >> 32)))
#endif

class NetworkPacket
{

public:
	NetworkPacket();
	NetworkPacket(void* buffer, uint32_t len_buffer);
	~NetworkPacket();

	uint32_t get_remaining() const { return m_datasize - m_read_offset; }
	uint32_t get_size() const { return m_datasize; }
	uint32_t get_offset() const { return m_read_offset; }
	char* get_data() const { return m_data; }
	bool get_error() const { return m_error; }
	bool set_data(char* data, uint32_t len);

	NetworkPacket &operator<<(uint64_t src);
	NetworkPacket &operator>>(uint64_t &dst);

	NetworkPacket &operator<<(uint32_t src);
	NetworkPacket &operator>>(uint32_t &dst);

	NetworkPacket &operator<<(uint16_t src);
	NetworkPacket &operator>>(uint16_t &dst);

	NetworkPacket &operator<<(uint8_t src);
	NetworkPacket &operator>>(uint8_t &dst);

	NetworkPacket &operator<<(char* src);
	NetworkPacket &operator>>(char* dst);

	bool add_char(char* src, uint16_t len);
	bool get_char(char* dst, uint16_t &len);

	bool reset();

private:
	uint32_t m_datasize = 0;
	char* m_data = (char*)malloc(0);
	uint32_t m_read_offset = 0;
	bool m_error = false;
};
