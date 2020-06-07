//
//  Dim.hpp
//  NumSrv
//
//  Created by Ignac Banic on 24/07/19.
//  Copyright © 2017 Ignac Banic. All rights reserved.
//

#ifndef Dim_hpp
#define Dim_hpp

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
#include "DimItem.hpp"

namespace fs = std::filesystem;

namespace NumSrv
{
	struct Dim
	{
	public:
		std::string key;
		std::map<std::string, std::string> name;
		std::map<std::string, std::string> desc;
	};

	void writeDimAndItems(Db* db, const Dim& dim, std::vector<DimItem>& items);

	size_t sizeDimItem(const DimItem& it);
	void writeDimItem(Byte*& s, const DimItem& it);
	DimItem readDimItem(const Byte*& s);

	size_t sizeDim(const Dim& it);
	void writeDim(Byte*& s, const Dim& it);
	Dim readDim(const Byte*& s);

	//Dim readDimPath(const std::string& path);


	class DimReader
	{
	private:
		Db* db_;
		boost::iostreams::mapped_file file_;
		Search::KeyValueFile keys_;
		Search::FileStore<DimItem> store_;
		std::vector<DimItemCache> cache_;

	public:
		DimReader(Db* db, const std::string& dimName);
		DimReader(const DimReader&) = delete;
		DimReader& operator=(const DimReader&) = delete;
		DimReader(DimReader&&) = default;
		DimReader& operator=(DimReader&&) = default;

		Dim dim() const;
		DimItem item(uint32_t id) const;
		uint32_t numItems() const;
		std::vector<uint32_t> allParents(uint32_t itemId) const;
		std::optional<uint32_t> itemIdByKey(const std::string& itmKey) const;
		std::vector<uint32_t> search(const std::string& query, const std::string& lang);
		std::vector<uint32_t> itemsOnLevel(uint32_t level) const;
		
		bool cacheEnabled() const { return !cache_.empty(); }
		void cacheStart();
		void cacheStop();
		const DimItemCache& itemCache(uint32_t id) const;
		
	};


}


#endif /* Dim_hpp */
