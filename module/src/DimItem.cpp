#include "DimItem.hpp"
#include "BinaryReadWrite.hpp"
#include "Language.hpp"
#include <iostream>
#include <ctime>
#include <unordered_set>
#include "Db.hpp"
#include <fstream>
#include <thread>
#include <future>

#include <FindMany.hpp>
#include <Comparators.hpp>
#include <Db.hpp>


namespace NumSrv
{
	

	bool DimItem::operator==(const DimItem& it2) const {
			const DimItem& it1 = *this;
			if(
				it1.id != it2.id ||
				it1.key != it2.key ||
				it1.nameShort != it2.nameShort ||
				it1.nameLong != it2.nameLong ||
				it1.desc != it2.desc ||
				it1.childs != it2.childs
			)
				return false;
			return true;
		}



		DimItem::DimItem(TId id2, const Search::BytesView& dt2)
			: id(id2)
		{
			const Byte* ptr = (const Byte*)dt2.data();
			key = BinaryReadWrite::readString(ptr);
			auto nameShort2 = readMapLang(ptr);
			nameShort = mapLangFromNum(nameShort2);
			auto nameLong2 = readMapLang(ptr);
			nameLong = mapLangFromNum(nameLong2);
			auto desc2 = readMapLang(ptr);
			desc = mapLangFromNum(desc2);
			parent = BinaryReadWrite::readUint32(ptr);
			auto numChilds = BinaryReadWrite::readUint32(ptr);
			childs.resize(numChilds);
			for( uint32_t i=0; i<numChilds; ++i ) {
				childs[i] = BinaryReadWrite::readUint32(ptr);
			}
		}
		Search::Bytes DimItem::serialize() const
		{
			auto nameShort2 = mapLangToNum(nameShort);
			auto nameLong2 = mapLangToNum(nameLong);
			auto desc2 = mapLangToNum(desc);

			size_t num =
				BinaryReadWrite::sizeString(key) +
				sizeMapLang(nameShort2) +
				sizeMapLang(nameLong2) +
				sizeMapLang(desc2) +
				BinaryReadWrite::sizeUint32() +
				BinaryReadWrite::sizeUint32() + BinaryReadWrite::sizeUint32() * childs.size()
			;
			Search::Bytes tmp(num, Byte{'\0'});
			auto* ptr = &tmp[0];
			BinaryReadWrite::writeString(ptr, key);
			writeMapLang(ptr, nameShort2);
			writeMapLang(ptr, nameLong2);
			writeMapLang(ptr, desc2);
			BinaryReadWrite::writeUint32(ptr, parent);
			BinaryReadWrite::writeUint32(ptr, childs.size());
			for(auto& id : childs) {
				BinaryReadWrite::writeUint32(ptr, id);
			}
			return tmp;
		}
		DimItem::TIdSerialized DimItem::serializeId(DimItem::TId id)
		{
			TIdSerialized arr;
			uint32_t* ptr = (uint32_t*)&arr[0];
			*ptr = id;
			return arr;
		}
		DimItem::TId DimItem::deserializeId(const DimItem::TIdSerialized& arr)
		{
			uint32_t* ptr = (uint32_t*)&arr[0];
			return *ptr;
		}
		std::vector<std::string> DimItem::allTexts() const
		{
			std::vector<std::string> arr;
			arr.push_back(key);
			for(const auto& pair : nameShort) {
				arr.push_back(pair.second);
			}
			for(const auto& pair : nameLong) {
				arr.push_back(pair.second);
			}
			for(const auto& pair : desc) {
				arr.push_back(pair.second);
			}
			return arr;
		}
}
