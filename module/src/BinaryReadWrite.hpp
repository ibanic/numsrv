//
//  BinaryReadWrite.hpp
//  NumSrvLib
//
//  Created by Ignac Banic on 02/09/17.
//  Copyright © 2017 Ignac Banic. All rights reserved.
//

#pragma once

#include "Types.hpp"
#include <string>
#include <map>
#include <vector>

namespace NumSrv {
	namespace BinaryReadWrite {

		void writeString(Byte*& s, const std::string& txt);
		void writeCString(Byte*& s, const std::string& txt);
		void writeStringList(Byte*& s, const std::vector<std::string>& list);
		void writeCStringList(Byte*& s, const std::vector<std::string>& list);
		void writeUint64(Byte*& s, uint64_t num);
		void writeUint32(Byte*& s, uint32_t num);
		void writeUint8(Byte*& s, uint8_t num);
		void writeDouble(Byte*& s, double num);
		void writeFloat(Byte*& s, float num);
		void writeBool(Byte*& s, bool c);

		std::string readString(const Byte*& s);
		std::string readCString(const Byte*& s);
		std::vector<std::string> readStringList(const Byte*& s);
		std::vector<std::string> readCStringList(const Byte*& s);
		uint64_t readUint64(const Byte*& s);
		uint32_t readUint32(const Byte*& s);
		uint8_t readUint8(const Byte*& s);
		double readDouble(const Byte*& s);
		float readFloat(const Byte*& s);
		bool readBool(const Byte*& s);

		size_t sizeString(const std::string& txt);
		size_t sizeCString(const std::string& txt);
		size_t sizeStringList(const std::vector<std::string>& list);
		size_t sizeCStringList(const std::vector<std::string>& list);
		size_t sizeUint64();
		size_t sizeUint32();
		size_t sizeUint8();
		size_t sizeDouble();
		size_t sizeFloat();
		size_t sizeBool();

	}
}