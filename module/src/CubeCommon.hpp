//
//  CubeWriter.hpp
//  NumSrv
//
//  Created by Ignac Banic on 09/05/20.
//  Copyright © 2020 Ignac Banic. All rights reserved.
//

#pragma once

#include <string>
#include <string>
#include <vector>
#include <map>
#include "Types.hpp"
#include <cassert>

namespace NumSrv {

// 16 bits
// two most significant are for options:
//   0 0    regular quality
//   0 1    prediction
//   1 1    fixed

	class Quality
	{
	private:
		uint16_t value_;
		Quality(uint16_t v) : value_(v) {}

	public:
		Quality() : value_(0) {}

		static Quality makeComputed(float v) {
			assert(v <= 1 && v >= 0);
			return {static_cast<uint16_t>(v*0x3FFF)};
		}
		static Quality makeFixed() {
			return {0xC000};
		}
		static Quality makePredicted() {
			float v = 0;
			assert(v <= 1 && v >= 0);
			uint16_t n = static_cast<uint16_t>(v*0x3FFF);
			n += 0x4000;
			return {n};
		}

		float value() const {
			assert(isComputed());
			uint16_t n = value_ & 0x3FFF;
			return (float)n / 0x3FFF;
		}
		float valueOrOne() const {
			if(!isComputed())
				return 1;
			uint16_t n = value_ & 0x3FFF;
			return (float)n / 0x3FFF;
			//return std::min(value_, (float)1.0);
		}
		bool isZero() const { return value_ == 0; }
		bool isComputed() const {
			uint16_t n = value_ & 0xC000;
			return n == 0;
		}
		bool isPredicted() const {
			uint16_t n = value_ & 0xC000;
			return n == 0x4000;
		}
		bool isFixed() const {
			uint16_t n = value_ & 0xC000;
			return n == 0xC000;
		}
		bool isOneOrMore() const {
			auto n = valueOrOne();
			return n >= 1;
		}
		//bool isComputed() const { return value_ <= 1; }
		//bool isPredicted() const { return value_ > 1 && value_ < 2; }
		//bool isFixed() const { return value_ == 2; }
		//bool isOneOrMore() const { return value_ >= 1; }

		bool operator ==(const Quality& b) const { return value_ == b.value_; }
		bool operator !=(const Quality& b) const { return value_ != b.value_; }
		bool operator <(const Quality& b) const { return value_ < b.value_; }
		bool operator >(const Quality& b) const { return value_ > b.value_; }
		bool operator <=(const Quality& b) const { return value_ <= b.value_; }
		bool operator >=(const Quality& b) const { return value_ >= b.value_; }
	};


	struct Cell
	{
		double num;
		//std::string desc;
		Quality q;
		Cell() = default;
		Cell(double num2, Quality q2=Quality()) : num(num2), q(q2) {}
	};


	enum class ConsolidationType
	{
		sum = 0,
		average = 1,
		last = 2
	};


	struct CubeDim
	{
		std::string key;
		ConsolidationType type;
		
		CubeDim() : type(ConsolidationType::sum) { }
		CubeDim(const std::string& key2) : key(key2), type(ConsolidationType::sum) { }
		CubeDim(const std::string& key2, ConsolidationType type2) : key(key2), type(type2) { }

		bool operator==(const CubeDim& it2) const
		{
			return key == it2.key && type == it2.type;
		}
	};


	struct Cube
	{
		std::string key;
		std::map<std::string, std::string> name;
		std::map<std::string, std::string> desc;
		std::string unit;
		bool integers = false;
		bool onlyPositive = false;
		std::string other;
		std::vector<CubeDim> dims;
	};


	namespace CubeCommon {
		Cube readMeta(const Byte*& dt);
		size_t sizeMeta(const Cube& meta);
		void writeMeta(Byte*& dt, const Cube& meta);

		Cell readCell(const Byte*& data, const Byte* start);
		size_t sizeCellBasic();
		size_t sizeCellAdditional(const Cell& cell);
		void writeCell(const Cell& cell, Byte*& dataCell, Byte* dataStart, Byte*& dataAdditional);

		uint64_t calcIdx(const std::vector<uint64_t>& multipliers, const std::vector<uint32_t>& coord);
		std::vector<uint32_t> calcCoord(const std::vector<uint64_t>& multipliers, uint64_t idx);
	}


}
