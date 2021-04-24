#include "CompressSize.hpp"
#include <stdexcept>

namespace NumSrv {

	uint64_t readSize(const Byte*& data)
	{
		uint8_t len = ((uint8_t)*data) & 0xC0;
		if( len == 0 ) {
			uint8_t num = *reinterpret_cast<const uint8_t*>(data);
			data += 1;
			return num;
		}
		if( len == 0x40 ) {
			uint16_t num = *reinterpret_cast<const uint16_t*>(data);
			num = num & 0xFF3F;
			data += 2;
			return swap_endian(num);
		}
		if( len == 0x80 ) {
			uint32_t num = *reinterpret_cast<const uint32_t*>(data);
			num = num & 0xFFFFFF3F;
			data += 4;
			return swap_endian(num);
		}
		uint64_t num = *reinterpret_cast<const uint64_t*>(data);
		num = num & 0xFFFFFFFFFFFFFF3F;
		data += 8;
		return swap_endian(num);
	}

	void writeSize(Byte*& data, uint64_t size, uint8_t numBytes)
	{
		if( (size <= 0x3F && numBytes==0) || numBytes==1 ) {
			*data = *reinterpret_cast<Byte*>(&size);
			data += 1;
		}
		else if( (size <= 0x3FFF && numBytes==0) || numBytes==2 ) {
			uint16_t num = swap_endian((uint16_t)size);
			num = num | 0x40;
			auto ptr = reinterpret_cast<uint16_t*>(data);
			*ptr = num;
			data += 2;
		}
		else if( (size <= 0x3FFFFFFF && numBytes==0) || numBytes==4 ) {
			uint32_t num = swap_endian((uint32_t)size);
			num = num | 0x80;
			auto ptr = reinterpret_cast<uint32_t*>(data);
			*ptr = num;
			data += 4;
		}
		else if( (size <= 0x3FFFFFFFFFFFFFFF && numBytes==0) || numBytes==8 ) {
			uint64_t num = swap_endian((uint64_t)size);
			num = num | 0xC0;
			auto ptr = reinterpret_cast<uint64_t*>(data);
			*ptr = num;
			data += 8;
		}
		else {
			throw std::runtime_error("too big number");
		}
	}

	Bytes writeSizeString(uint64_t size, uint8_t numBytes)
	{
		if(numBytes == 0) {
			numBytes = numBytesSize(size);
		}
		Bytes buff(numBytes, Byte{'\0'});
		Byte* buff2 = &buff[0];
		writeSize(buff2, size, numBytes);
		return buff;
	}

	uint8_t numBytesSize(size_t size)
	{
		if( size <= 0x3F ) {
			return 1;
		}
		else if( size <= 0x3FFF ) {
			return 2;
		}
		else if( size <= 0x3FFFFFFF ) {
			return 4;
		}
		else if( size <= 0x3FFFFFFFFFFFFFFF ) {
			return 8;
		}
		else {
			throw std::runtime_error("too big number");
		}
	}

}
