#include "Dim.hpp"
#include "BinaryReadWrite.hpp"
#include "Language.hpp"
#include <iostream>
#include <ctime>
#include <unordered_set>
#include "Db.hpp"
#include <fstream>
#include <thread>
#include <future>

#include "../../../Search/FindMany.hpp"
#include "../../../Search/Comparators.hpp"
#include "../../../Search/Db.hpp"
#include "Common.hpp"


namespace NumSrv
{
	fs::path genDimDataPath(const fs::path& dataPath, const std::string& dimKey)
	{
		fs::path p(dataPath);
		p /= "dim-data-" + dimKey + ".bin";
		return p;
	}


	fs::path genDimNamesPath(const fs::path& dataPath, const std::string& dimKey)
	{
		fs::path p(dataPath);
		p /= "dim-names-" + dimKey + ".bin";
		return p;
	}


	fs::path genDimKeysPath(const fs::path& dataPath, const std::string& dimKey)
	{
		fs::path p(dataPath);
		p /= "dim-keys-" + dimKey + ".bin";
		return p;
	}




	bool checkKey(const std::string& key) {
		if( key.size() < 2 )
			return false;
		if( key.size() > 125 )
			return false;
		for( size_t i=0; i<key.size(); ++i ) {
			char c = key[i];
			if( c >= 'a' && c <= 'z' )
				continue;
			if( c >= '0' && c <= '9' )
				continue;
			if( c == '-' || c == '_' )
				continue;
			return false;
		}
		return true;
	}

	void sortDimItems(std::vector<DimItem>& items)
	{
		// parentItemId -> childIndex
		writeStatus("mapping childs ...");
		uint32_t totalIdx = -1;
		std::map<uint32_t, std::vector<uint32_t> > childs;
		for( uint32_t i=0; i<items.size(); ++i ) {
			if( items[i].id == 0 ) {
				if( totalIdx != (uint32_t)-1 )
					throw std::runtime_error("Multiple items with id:0");
				totalIdx = i;
			}
			else {
				auto p = items[i].parent;
				auto res = childs.insert({p, {i}});
				if( !res.second ) {
					res.first->second.push_back(i);
				}
			}
			items[i].childs.clear();
		}
		if( totalIdx == (uint32_t)-1 )
			throw std::runtime_error("Item with id:0 doesnt exist");

		std::vector<DimItem> tmp;
		tmp.reserve(items.size());
		tmp.push_back(items[totalIdx]);

		std::map<uint32_t, uint32_t> mapOldNewId;

		writeStatus("rebuilding ids ...");
		uint32_t i = 0;
		while( i < tmp.size() ) {
			// add all childs
			auto oldId = tmp[i].id;
			tmp[i].id = i;
			mapOldNewId[oldId] = i;
			auto ptr = childs.find(oldId);
			if( ptr != childs.end() ) {
				// sort childs by name
				auto& arr = ptr->second;
				std::sort(
					arr.begin(),
					arr.end(), 
					[&items](uint32_t idx1, uint32_t idx2) -> bool
					{
						const auto& it1 = items[idx1];
						const auto& it2 = items[idx2];
						auto ptr1 = it1.nameShort.find("en");
						auto ptr2 = it2.nameShort.find("en");
						auto n1 = ptr1!=it1.nameShort.end() ? ptr1->second : it1.key;
						auto n2 = ptr2!=it2.nameShort.end() ? ptr2->second : it2.key;
						auto r = n1.compare(n2);
						if( r < 0 )
							return true;
						if( r > 0 )
							return false;
						return idx1 < idx2;
					}
				);
				// add
				for( auto idx : arr ) {
					items[idx].parent = i;
					tmp[i].childs.push_back(tmp.size());
					tmp.push_back(items[idx]);
				}
			}
			i++;
		}
		items = tmp;
	}


	void itemsSearchThread(Search::Db<Search::FileStore<DimItem>>::BulkWriter& writer, const std::vector<DimItem>& items, size_t begin, size_t end)
	{
		for( size_t i=begin; i<end; ++i ) {
			auto& doc = items[i];
			writer.add(doc);
		}
	}

	void writeDimAndItems(Db* db, const Dim& dim, std::vector<DimItem>& items)
	{
		const auto& dataPath = db->dataPath();
		sortDimItems(items);

		// check id, key
		writeStatus("checking key ...");
		std::unordered_set<std::string> keys;
		for( uint32_t i=0; i<items.size(); ++i ) {
			if(!checkKey(items[i].key))
				throw std::runtime_error("Bad key at item id: "+std::to_string(i)+" key:"+items[i].key);
			auto pair = keys.insert(items[i].key);
			if( !pair.second )
				throw std::runtime_error("Duplicate key at item id: "+std::to_string(i)+" key: "+items[i].key);
		}

		// numItems, itemLocations
		writeStatus("write dim and items ...");
		auto sz = BinaryReadWrite::sizeUint32() + items.size() * BinaryReadWrite::sizeUint32();
		// dim
		sz += sizeDim(dim);
		// items
		for( const auto& it : items )
			sz += sizeDimItem(it);

		auto path2 = genDimDataPath(dataPath, dim.key);
		// https://stackoverflow.com/a/7896096
		{
			std::ofstream ofs(path2.string(), std::ios::binary | std::ios::out);
			ofs.seekp(sz-1);
			ofs.write("", 1);
			ofs.close();
		}

		// open file
		boost::iostreams::mapped_file file(path2.string());
		Byte* s = (Byte*)file.data();

		// write num items, locations
		BinaryReadWrite::writeUint32(s, items.size());
		auto sItmLoc = s;
		s += items.size() * BinaryReadWrite::sizeUint32();
		
		// dim meta data
		writeDim(s, dim);

		// items
		for( const auto& it : items ) {
			auto offset = s - (Byte*)file.data();
			writeDimItem(s, it);
			BinaryReadWrite::writeUint32(sItmLoc, offset);
		}

		// keys
		writeStatus("write keys ...");
		auto path4 = genDimKeysPath(dataPath, dim.key);
		Search::KeyValueFile keys2(path4);
		for( const auto& it : items ) {
			Search::Bytes id2(BinaryReadWrite::sizeUint32(), '\0');
			Byte* ptr = &id2[0];
			BinaryReadWrite::writeUint32(ptr, it.id);
			keys2.set(it.key, id2);
		}

		// search
		writeStatus("write search ...");
		
		size_t total = 0;
		size_t numTokens = 0;
		for( const auto& it : items ) {
			auto s = sizeDimItem(it);
			total += s;
			for(auto& x : it.allTexts()) {
				numTokens += x.size();
			}
		}
		std::cout << "total size: " << std::to_string(total) << std::endl;
		std::cout << "total tokens: " << std::to_string(numTokens) << std::endl;


		// search
		auto path3 = genDimNamesPath(dataPath, dim.key);
		Search::FileStore<DimItem>::removeFiles(path3);
		typedef Search::Db<Search::FileStore<DimItem>> TSearchDb;
		Search::FileStore<DimItem> store(path3);
		TSearchDb db2(store);

		if( items.size() < 10'000 ) {
			for( const auto& it : items ) {
				db2.add(it);
			}
		}
		else {
			auto numThreads = std::thread::hardware_concurrency();
			if(numThreads==0) {numThreads = 1;}
			std::vector<size_t> starts;
			size_t perThread = items.size() / numThreads;
			for(size_t i=0; i<numThreads; ++i) {
				starts.push_back(i*perThread);
			}
			starts.push_back(items.size());
			auto writers = db2.bulkWriters(numThreads);
			std::vector<std::future<void>> arr(numThreads);
			for( size_t i=0; i<numThreads; ++i ) {
				arr[i] = std::async(std::launch::async, itemsSearchThread, std::ref(writers[i]), std::cref(items), starts[i], starts[i+1]);
			}
			for(auto& ft : arr) {
				ft.wait();
			}
			writeStatus("bulk add ...");
			db2.bulkAdd(writers);
		}


		writeStatus("finished");
	}

	
	size_t sizeDimItem(const DimItem& itm)
	{
		size_t s =
			BinaryReadWrite::sizeCString(itm.key) +  // key
			sizeMapLang(mapLangToNum(itm.nameShort)) +  // nameShort
			sizeMapLang(mapLangToNum(itm.nameLong)) +  // nameLong
			sizeMapLang(mapLangToNum(itm.desc)) +  // desc
			BinaryReadWrite::sizeUint32() +  // parent
			BinaryReadWrite::sizeUint32() + itm.childs.size() * BinaryReadWrite::sizeUint32() +  // numChilds, childs
			0
		;
		return s;
	}


	void writeDimItem(Byte*& s, const DimItem& itm)
	{
		BinaryReadWrite::writeCString(s, itm.key);
		writeMapLang(s, mapLangToNum(itm.nameShort));
		writeMapLang(s, mapLangToNum(itm.nameLong));
		writeMapLang(s, mapLangToNum(itm.desc));
		BinaryReadWrite::writeUint32(s, itm.parent);
		
		BinaryReadWrite::writeUint32(s, itm.childs.size());
		for( auto itmId2 : itm.childs )
			BinaryReadWrite::writeUint32(s, itmId2);
	}


	DimItem readDimItem(const Byte*& s, uint32_t id)
	{
		DimItem itm;
		itm.id = id;
		itm.key = BinaryReadWrite::readCString(s);
		itm.nameShort = mapLangFromNum(readMapLang(s));
		itm.nameLong = mapLangFromNum(readMapLang(s));
		itm.desc = mapLangFromNum(readMapLang(s));
		itm.parent = BinaryReadWrite::readUint32(s);
		
		auto numChilds = BinaryReadWrite::readUint32(s);
		itm.childs.resize(numChilds);
		for( uint32_t i=0; i<numChilds; ++i )
			itm.childs[i] = BinaryReadWrite::readUint32(s);

		return itm;
	}


	size_t sizeDim(const Dim& dim)
	{
		return
			BinaryReadWrite::sizeCString(dim.key) +
			sizeMapLang(mapLangToNum(dim.name)) +
			sizeMapLang(mapLangToNum(dim.desc))
		;
	}


	void writeDim(Byte*& s, const Dim& dim)
	{
		BinaryReadWrite::writeCString(s, dim.key);
		writeMapLang(s, mapLangToNum(dim.name));
		writeMapLang(s, mapLangToNum(dim.desc));
	}


	Dim readDim(const Byte*& s)
	{
		Dim dim;
		dim.key = BinaryReadWrite::readCString(s);
		dim.name = mapLangFromNum(readMapLang(s));
		dim.desc = mapLangFromNum(readMapLang(s));
		return dim;
	}


	DimReader::DimReader(Db* db, const std::string& dimKey) :
		db_(db),
		file_(genDimDataPath(db->dataPath(),dimKey)),
		keys_(genDimKeysPath(db->dataPath(), dimKey)),
		store_(genDimNamesPath(db->dataPath(), dimKey))
	{
	}


	Dim DimReader::dim() const
	{
		const Byte* s = (const Byte*)file_.data();
		auto num = BinaryReadWrite::readUint32(s);
		s += num * BinaryReadWrite::sizeUint32();
		return readDim(s);
	}


	DimItem DimReader::item(uint32_t id) const
	{
		const Byte* dt = (const Byte*)file_.data();
		dt += BinaryReadWrite::sizeUint32() * (1+id);
		auto offset = BinaryReadWrite::readUint32(dt);
		dt = (Byte*)file_.data() + offset;
		return readDimItem(dt, id);
	}


	uint32_t DimReader::numItems() const
	{
		if(cacheEnabled()) {
			return cache_.size();
		}
		auto dt = (const Byte*)file_.data();
		return BinaryReadWrite::readUint32(dt);
	}


	std::vector<uint32_t> DimReader::allParents(uint32_t itemId) const
	{
		std::vector<uint32_t> arr;
		while(itemId != 0)
		{
			if( cacheEnabled() ) {
				const auto& it = itemCache(itemId);
				itemId = it.parent;
			}
			else {
				auto it = item(itemId);
				itemId = it.parent;
			}
			arr.push_back(itemId);
		}
		return arr;
	}


	std::optional<uint32_t> DimReader::itemIdByKey(const std::string& itmKey) const
	{
		auto view = keys_.get(itmKey);
		if(view.size() == 0)
			return std::nullopt;
		auto ptr = view.data();
		return BinaryReadWrite::readUint32(ptr);
	}




	template <class TRes>
	class CompLevel {
	public:
		const DimReader& reader_;
		CompLevel(const DimReader& reader) : reader_(reader) {}
		void init(const std::vector<TRes>& all, const Search::SearchSettings<typename TRes::TDoc>& sett) { }
		void clean() { }
		int compare(const TRes& d1, const TRes& d2) const {
			uint32_t l1 = reader_.allParents(d1.id).size();
			uint32_t l2 = reader_.allParents(d2.id).size();
			if(l1 < l2)
				return -1;
			if(l1 > l2)
				return 1;
			return 0;
		}
	};

	template <class TRes>
	class CompIsKey {
	public:
		const Search::SearchSettings<typename TRes::TDoc>* sett_;
		void init(const std::vector<TRes>& all, const Search::SearchSettings<typename TRes::TDoc>& sett) {
			sett_ = &sett;
		}
		void clean() { }
		int compare(const TRes& d1, const TRes& d2) const {
			auto& k1 = d1.doc.key;
			auto& k2 = d2.doc.key;
			int l1 = 0;
			int l2 = 0;
			if(l1==0 && k1 == sett_->query) { l1 = 2; }
			if(l2==0 && k2 == sett_->query) { l2 = 2; }
			if(l1==0 && k1.rfind(sett_->query, 0) == 0) { l1 = 1; }
			if(l2==0 && k2.rfind(sett_->query, 0) == 0) { l2 = 1; }
			if(l1 > l2)
				return -1;
			if(l1 < l2)
				return 1;
			return 0;
		}
	};



	std::vector<uint32_t> DimReader::search(const std::string& query, const std::string& lang)
	{
		typedef Search::Db<Search::FileStore<DimItem>> TSearchDb;
		TSearchDb db(store_);

		typedef Search::Result<DimItem> TRes;
		Search::CompIsWhole<TRes> cmp1;
		Search::CompWordsTogether<TRes> cmp2;
		CompLevel<TRes> cmpLevel(*this);
		CompIsKey<TRes> cmpKey;
		Search::CompPriorityTextsCallback<TRes> cmpLang([&lang](const DimItem& doc, std::vector<uint8_t>& priorities){
			size_t idx = 0;
			idx++;  // key
			for(const auto& pair : doc.nameShort) {
				if(pair.first == lang)
					priorities[idx] = 1;
				idx++;
			}
			for(const auto& pair : doc.nameLong) {
				if(pair.first == lang)
					priorities[idx] = 1;
				idx++;
			}
			for(const auto& pair : doc.desc) {
				if(pair.first == lang)
					priorities[idx] = 1;
				idx++;
			}
		});

		Search::SearchSettings<DimItem> sett;
		sett.query = query;
		auto results1 = Search::findMany<TSearchDb> ({&db}, sett, cmp1, cmp2, cmpKey, cmpLang, cmpLevel);

		std::vector<uint32_t> arr;
		for(const auto& res : results1 ) {
			arr.push_back(res.id);
		}
		return arr;
	}


	std::vector<uint32_t> DimReader::itemsOnLevel(uint32_t level) const
	{
		std::vector<uint32_t> ids({0});
		while(level != 0) {
			std::vector<uint32_t> ids2;
			for( auto id : ids ) {
				if(cacheEnabled()) {
					const auto& it = itemCache(id);
					ids2.insert(ids2.end(), it.childs.begin(), it.childs.end());
				}
				else {
					auto it = item(id);
					ids2.insert(ids2.end(), it.childs.begin(), it.childs.end());
				}
			}
			ids = ids2;
			level--;
		}
		return ids;
	}


	void DimReader::cacheStart()
	{
		if( !cache_.empty() ) {
			return;
		}
		auto num = numItems();
		cache_.resize(num);
		for(uint32_t i=0; i<num; ++i) {
			auto it = item(i);
			cache_[i].parent = it.parent;
			cache_[i].childs = it.childs;
		}
	}


	void DimReader::cacheStop()
	{
		cache_ = std::vector<DimItemCache>();
	}


	const DimItemCache& DimReader::itemCache(uint32_t id) const
	{
		if(!cacheEnabled()) {
			throw std::runtime_error("DimReader::itemCache() cache is not enabled");
		}
		return cache_.at(id);
	}
}
