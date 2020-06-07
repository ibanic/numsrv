#include "BinaryReadWrite.hpp"
#include <cstring>

namespace NumSrv {
namespace BinaryReadWrite {

	template<typename T>
	T readScalar(const Byte*& s)
	{
		const T* ptr = reinterpret_cast<const T*>(s);
		T num = ptr[0];
		s += sizeof(T);
		return num;
	}

	template<typename T>
	void writeScalar(Byte*& s, T num)
	{
		std::memcpy(s, reinterpret_cast<const Byte*>(&num), sizeof(T));
		s += sizeof(T);
	}



	void writeString(Byte*& s, const std::string &txt)
	{
		writeUint32(s, txt.size());
		std::memcpy(s, txt.c_str(), txt.size());
		s += txt.size();
	}


	void writeCString(Byte*& s, const std::string &txt)
	{
		std::memcpy(s, txt.c_str(), txt.size()+1);
		s += txt.size() + 1;
	}


	void writeStringList(Byte*& s, const std::vector<std::string> &list)
	{
		writeUint32(s, list.size());
		for( auto& txt : list )
			writeString(s, txt);
	}


	void writeCStringList(Byte*& s, const std::vector<std::string> &list)
	{
		writeUint32(s, list.size());
		for( auto& txt : list )
			writeString(s, txt);
	}


	void writeUint64(Byte*& s, uint64_t num)
	{
		writeScalar<uint64_t>(s, num);
	}


	void writeUint32(Byte*& s, uint32_t num)
	{
		writeScalar<uint32_t>(s, num);
	}
			
			
	void writeUint8(Byte*& s, uint8_t num)
	{
		writeScalar<uint8_t>(s, num);
	}


	void writeDouble(Byte*& s, double num)
	{
		writeScalar<double>(s, num);
	}


	void writeFloat(Byte*& s, float num)
	{
		writeScalar<float>(s, num);
	}


	void writeBool(Byte*& s, bool num)
	{
		writeScalar<bool>(s, num);
	}








	std::string readString(const Byte*& s)
	{
		uint32_t len = readUint32(s);
		std::string str = std::string((const char*)s, len);
		s += len;
		return str;
	}


	std::string readCString(const Byte*& s)
	{
		std::string txt((const char*)s);
		s += txt.size() + 1;
		return txt;
	}


	std::vector<std::string> readStringList(const Byte*& s)
	{
		uint32_t len = readUint32(s);
		std::vector<std::string> list;
		list.reserve(len);
		for( uint32_t i=0; i<len; ++i )
			list.push_back(readString(s));
		return list;
	}


	std::vector<std::string> readCStringList(const Byte*& s)
	{
		uint32_t len = readUint32(s);
		std::vector<std::string> list;
		list.reserve(len);
		for( uint32_t i=0; i<len; ++i )
			list.push_back(readCString(s));
		return list;
	}


	uint64_t readUint64(const Byte*& s)
	{
		return readScalar<uint64_t>(s);
	}


	uint32_t readUint32(const Byte*& s)
	{
		return readScalar<uint32_t>(s);
	}
			
			
	uint8_t readUint8(const Byte*& s)
	{
		return readScalar<uint8_t>(s);
	}


	double readDouble(const Byte*& s)
	{
		return readScalar<double>(s);
	}


	float readFloat(const Byte*& s)
	{
		return readScalar<float>(s);
	}


	bool readBool(const Byte*& s)
	{
		return readScalar<bool>(s);
	}


	size_t sizeString(const std::string& txt) { return sizeUint32()+txt.size(); }
	size_t sizeCString(const std::string& txt) { return txt.size()+1; }
	size_t sizeStringList(const std::vector<std::string>& list) {
		size_t s = sizeUint32();
		for( const auto& txt : list )
			s += sizeString(txt);
		return s;
	}
	size_t sizeCStringList(const std::vector<std::string>& list) {
		size_t s = sizeUint32();
		for( const auto& txt : list )
			s += sizeCString(txt);
		return s;
	}
	size_t sizeUint64() { return sizeof(uint64_t); }
	size_t sizeUint32() { return sizeof(uint32_t); }
	size_t sizeUint8() { return sizeof(uint8_t); }
	size_t sizeDouble() { return sizeof(double); }
	size_t sizeFloat() { return sizeof(float); }
	size_t sizeBool() { return sizeof(bool); }

}
}