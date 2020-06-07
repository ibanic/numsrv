//
//  DimItem.hpp
//  NumSrv
//
//  Created by Ignac Banic on 24/07/19.
//  Copyright © 2017 Ignac Banic. All rights reserved.
//

#ifndef DimItem_hpp
#define DimItem_hpp

#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <boost/iostreams/device/mapped_file.hpp>
#include <optional>
#include <map>
#include "../../../Search/KeyValueFile.hpp"
#include "../../../Search/Types.hpp"
#include "../../../Search/FileStore.hpp"
#include "Types.hpp"

namespace fs = std::filesystem;

namespace NumSrv
{
	class Db;

	struct DimItem
	{
	public:
		typedef uint32_t TId;
		typedef std::array<Search::Byte, sizeof(TId)> TIdSerialized;
		
		uint32_t id;
		std::string key;
		std::map<std::string, std::string> nameShort;
		std::map<std::string, std::string> nameLong;
		std::map<std::string, std::string> desc;
		uint32_t parent;
		std::vector<uint32_t> childs;

		bool operator==(const DimItem& it2) const;
		bool operator!=(const DimItem& it2) const
		{
			return !operator==(it2);
		}
		
		DimItem() = default;
		
		DimItem(TId id, const std::basic_string_view<Search::Byte>& dt2);
		Search::Bytes serialize() const;
		static TIdSerialized serializeId(TId id);
		static TId deserializeId(const TIdSerialized& arr);
		std::vector<std::string> allTexts() const;
		uint32_t docId() const { return id; }
	};


	struct DimItemCache
	{
	public:
		DimItem::TId parent;
		std::vector<DimItem::TId> childs;
	};


}


#endif /* DimItem_hpp */
