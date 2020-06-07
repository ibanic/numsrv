#include "CubeReader.hpp"
#include "Db.hpp"
#include "BinaryReadWrite.hpp"
#include <algorithm>
#include "Language.hpp"

namespace NumSrv
{
		CubeReader::CubeReader(Db* db, const std::string& cubeKey)
			: db_(db)
		{
			path_ = db->dataPath();
			path_ /= "cube-data-"+cubeKey+".bin";

			file_.open(path_);
			assert(file_.data() != nullptr);

			// meta
			const Byte* ptrMetaOffset = data() + 0;
			auto metaOffset = *reinterpret_cast<const uint64_t*>(ptrMetaOffset);
			const auto* ptrMeta = data() + metaOffset;
			meta_ = CubeCommon::readMeta(ptrMeta);
			
			// multipliers
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
		}


		std::vector<uint32_t> CubeReader::calcIds(const std::vector<std::string>& keys) const
		{
			assert(keys.size() <= meta_.dims.size());

			std::vector<uint32_t> arr(keys.size());
			for( size_t i=0; i<keys.size(); ++i ) {
				auto dimKey = meta_.dims[i].key;
				auto& dim = db_->dim(dimKey);
				auto res = dim.itemIdByKey(keys[i]);
				if( !res ) {
					throw std::runtime_error("Cant find item key: "+keys[i]+" in dim idx:"+std::to_string(i));
				}
				auto id = *res;
				arr[i] = id;
			}
			return arr;
		}


		Cell CubeReader::cell(const std::vector<uint32_t>& ids) const
		{
			auto idx = CubeCommon::calcIdx(multipliers_, ids);
			return cell(idx);
		}


		Cell CubeReader::cell(uint64_t idx) const
		{
			const Byte* ptr = data() + 128 + idx * CubeCommon::sizeCellBasic();
			return CubeCommon::readCell(ptr, data());
		}


		Byte* CubeReader::data() const
		{
			auto ptr = file_.data();
			assert(ptr != nullptr);
			return (Byte*)ptr;
		}
}
