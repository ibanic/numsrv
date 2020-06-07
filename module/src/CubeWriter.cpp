#include "CubeWriter.hpp"
#include "Db.hpp"



// for ram
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

uint64_t availableMemory()
{
	static uint64_t mem = 0;
	if( mem == 0 ) {
		// for determining ram size
		// https://stackoverflow.com/a/2513561
		#ifdef _WIN32
			MEMORYSTATUSEX status;
			status.dwLength = sizeof(status);
			GlobalMemoryStatusEx(&status);
			mem = status.ullTotalPhys;
		#else
			auto pages = sysconf(_SC_PHYS_PAGES);
			auto page_size = sysconf(_SC_PAGE_SIZE);
			mem = pages * page_size;
		#endif
		if( mem == 0 )
			throw std::runtime_error("Cant figure out how much ram is installed");
	}
	return mem;
}



namespace NumSrv {
	CubeWriter::CubeWriter(Db* db, const Cube& meta)
		: db_(db), meta_(meta)
	{
		// cache other dims
		for( const auto& d : meta_.dims ) {
			auto& dim = db_->dim(d.key);
			dimCache.push_back(&dim);
			dim.cacheStart();
		}
		
		auto numDims = meta_.dims.size();
		multipliers_.resize(numDims+1);
		multipliers_[0] = 1;
		for( size_t i=1; i<=numDims; i++ )
		{
			uint64_t prevValue = multipliers_[i-1];
			uint32_t numItems = db_->dim(meta_.dims[i-1].key).numItems();
			uint64_t val = prevValue * numItems;
			multipliers_[i] = val;
		}

		// cells
		auto requiredSpace = numCells() * sizeof(Cell) * 1.2;
		if( requiredSpace > availableMemory() ) {
			throw std::runtime_error("CubeWriter() Not enough RAM. Cube has too many cells "+std::to_string(numCells()));
		}
		cells_.resize(numCells());
	}


	std::vector<uint32_t> CubeWriter::calcIds(const std::vector<std::string>& keys) const
	{
		assert(keys.size() <= meta_.dims.size());

		std::vector<uint32_t> arr(keys.size());
		for( size_t i=0; i<keys.size(); ++i ) {
			//auto dimKey = meta_.dims[i].key;
			//auto& dim = db_->dim(dimKey);
			auto dim = dimCache[i];
			auto res = dim->itemIdByKey(keys[i]);
			if( !res ) {
				throw std::runtime_error("Cant find item key: "+keys[i]+" in dim idx:"+std::to_string(i));
			}
			auto id = *res;
			arr[i] = id;
		}
		return arr;
	}

	uint64_t CubeWriter::calcIdx(const std::vector<uint32_t>& coord) const
	{
		return CubeCommon::calcIdx(multipliers_, coord);
	}

	std::vector<uint32_t> CubeWriter::calcCoord(uint64_t idx) const
	{
		return CubeCommon::calcCoord(multipliers_, idx);
	}


	const Cell& CubeWriter::cell(const std::vector<uint32_t>& ids) const
	{
		auto idx = calcIdx(ids);
		return cell(idx);
	}


	void CubeWriter::setCellNumber(const std::vector<uint32_t>& ids, double n)
	{
		auto idx = calcIdx(ids);
		return setCellNumber(idx, n);
	}


	void CubeWriter::setCellNumber(uint64_t idx, double n)
	{
		if( idx >= numCells() )
			throw std::runtime_error("Cell doesnt exist");
		cells_[idx] = Cell(n, Quality::makeFixed());
	}
	

	void CubeWriter::save() const
	{
		// find file size
		size_t szBasic =
			128 +
			numCells() * CubeCommon::sizeCellBasic() +
			0
		;
		size_t sz = szBasic + CubeCommon::sizeMeta(meta_);
		auto n = numCells();
		for(uint64_t i=0; i<n; ++i) {
			sz += CubeCommon::sizeCellAdditional(cell(i));
		}

		fs::path path(db()->dataPath());
		path /= "cube-data-"+meta_.key+".bin";

		// create file
		{ std::ofstream tmpOf(path); }
		fs::resize_file(path, sz);

		// open file
		boost::iostreams::mapped_file file(path.string());
		Byte* dt = (Byte*)file.data();

		// index
		auto index = reinterpret_cast<uint64_t*>(dt);
		index[0] = szBasic;  // metaOffset

		// write meta
		auto ptrFreeData = dt + szBasic;
		CubeCommon::writeMeta(ptrFreeData, meta_);

		// write cells
		for(uint64_t i=0; i<n; ++i) {
			auto ptrCell = dt + 128 + i*CubeCommon::sizeCellBasic();
			CubeCommon::writeCell(cell(i), ptrCell, dt, ptrFreeData);
		}
		
		file.close();
		db_->loadCube(meta_.key);
	}
}
