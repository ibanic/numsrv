#include "CubeCommon.hpp"
#include "BinaryReadWrite.hpp"
#include "Language.hpp"

namespace NumSrv {
	namespace CubeCommon {

		Cube readMeta(const Byte*& dt)
		{
			Cube c;
			c.key = BinaryReadWrite::readCString(dt);
			c.name = mapLangFromNum(readMapLang(dt));
			c.desc = mapLangFromNum(readMapLang(dt));
			c.unit = BinaryReadWrite::readCString(dt);
			c.integers = BinaryReadWrite::readBool(dt);
			c.onlyPositive = BinaryReadWrite::readBool(dt);
			c.other = BinaryReadWrite::readCString(dt);
			auto numDims = BinaryReadWrite::readUint8(dt);
			c.dims.resize(numDims);
			for( uint8_t i=0; i<numDims; ++i ) {
				c.dims[i].key = BinaryReadWrite::readCString(dt);
				c.dims[i].type = static_cast<ConsolidationType>(BinaryReadWrite::readUint8(dt));
			}
			return c;
		}

		size_t sizeMeta(const Cube& meta)
		{
			size_t sz =
				BinaryReadWrite::sizeCString(meta.key) +
				sizeMapLang(mapLangToNum(meta.name)) +
				sizeMapLang(mapLangToNum(meta.desc)) +
				BinaryReadWrite::sizeCString(meta.unit) +
				BinaryReadWrite::sizeBool() +
				BinaryReadWrite::sizeCString(meta.other) +
				BinaryReadWrite::sizeUint8()
			;
			for( const auto& cbDim : meta.dims ) {
				sz += BinaryReadWrite::sizeCString(cbDim.key);
				sz += BinaryReadWrite::sizeUint8();
			}
			return sz;
		}

		void writeMeta(Byte*& dt, const Cube& meta)
		{
			BinaryReadWrite::writeCString(dt, meta.key);
			writeMapLang(dt, mapLangToNum(meta.name));
			writeMapLang(dt, mapLangToNum(meta.desc));
			BinaryReadWrite::writeCString(dt, meta.unit);
			BinaryReadWrite::writeBool(dt, meta.integers);
			BinaryReadWrite::writeBool(dt, meta.onlyPositive);
			BinaryReadWrite::writeCString(dt, meta.other);
			BinaryReadWrite::writeUint8(dt, meta.dims.size());
			for( const auto& cbDim : meta.dims ) {
				BinaryReadWrite::writeCString(dt, cbDim.key);
				BinaryReadWrite::writeUint8(dt, static_cast<uint8_t>(cbDim.type));
			}
		}



		Cell readCell(const Byte*& data, const Byte* start)
		{
			Cell c;
			c.num = BinaryReadWrite::readDouble(data);
			/*auto off = BinaryReadWrite::readUint64(data);
			if( off != 0 ) {
				const Byte* ptr = start + off;
				c.desc = BinaryReadWrite::readCString(ptr);
			}*/
			return c;
		}

		size_t sizeCellBasic()
		{
			return BinaryReadWrite::sizeDouble() + BinaryReadWrite::sizeUint64();
		}

		size_t sizeCellAdditional(const Cell& cell)
		{
			/*if( !cell.desc.empty() ) {
				return BinaryReadWrite::sizeCString(cell.desc);
			}*/
			return 0;
		}

		void writeCell(const Cell& cell, Byte*& dataCell, Byte* dataStart, Byte*& dataAdditional)
		{
			BinaryReadWrite::writeDouble(dataCell, cell.num);
			/*if( cell.desc.empty() ) {
				BinaryReadWrite::writeUint64(dataCell, 0);
			}
			else {
				uint64_t off = dataAdditional - dataStart;
				BinaryReadWrite::writeUint64(dataCell, off);
				BinaryReadWrite::writeCString(dataAdditional, cell.desc);
			}*/
		}




		uint64_t calcIdx(const std::vector<uint64_t>& multipliers, const std::vector<uint32_t>& coord)
		{
			auto numDims = multipliers.size() - 1;
			if( coord.size() > numDims )
				throw std::runtime_error("Too many items");

			uint64_t idx = 0;
			for( size_t i=0; i<numDims; ++i )
			{
				uint32_t c;
				if( i < coord.size() )
					c = coord[i];
				else
					c = 0;
				idx += multipliers[i] * c;
			}
			return idx;
		}

	
		std::vector<uint32_t> calcCoord(const std::vector<uint64_t>& multipliers, uint64_t idx)
		{
			auto numDims = multipliers.size() - 1;
			if(numDims == 0)
				return {};
			std::vector<uint32_t> coord(numDims);
			size_t i = numDims-1;
			while(true) {
				auto m = multipliers[i];
				coord[i] = (uint32_t)(idx / m);
				idx -= coord[i] * m;
				if(i == 0)
					break;
				i--;
			}
			return coord;
		}

	}
}
