#include "CubeWriter.hpp"
#include "Db.hpp"

namespace NumSrv
{
	void calcValues2(const Cell& realValue, std::vector<Cell>& realChilds, const Cell& proxyValue, const std::vector<Cell>& proxyChilds, ConsolidationType type)
	{
		assert(realChilds.size() == proxyChilds.size());
		uint32_t nc = realChilds.size();
		for( uint32_t i=0; i<nc; ++i ) {
			if( realChilds[i].q.isComputed() && realChilds[i].q.value() < 1 ) {
				double f = proxyChilds[i].num / proxyValue.num;
				Cell c;
				c.num = realValue.num * f;
				c.q = Quality::makePredicted();
				realChilds[i] = c;
			}
		}
	}


	uint32_t convertId(CubeWriter& realCube, uint32_t realNthDim, uint32_t realItemId, const CubeReader& proxyCube, uint32_t proxyNthDim)
	{
		auto dimKey1 = realCube.meta().dims[realNthDim].key;
		auto dimKey2 = proxyCube.meta().dims[proxyNthDim].key;
		if(dimKey1 == dimKey2 && realCube.db() == proxyCube.db()) {
			return realItemId;
		}

		// find item key
		//auto& dim1 = realCube.db()->dim(dimKey1);
		auto dim1 = realCube.dimCache[realNthDim];
		auto it1 = dim1->item(realItemId);  // cant use cache because we need key, which is not cached

		//
		auto& dim2 = proxyCube.db()->dim(dimKey2);
		auto idOpt = dim2.itemIdByKey(it1.key);
		if(idOpt)
			return *idOpt;
		throw std::runtime_error("normalize with cube, convertId() cant find same dim item in proxy dim");
	}


	void visit2(CubeWriter& realCube, const std::vector<uint32_t>& realCoord, const CubeReader& proxyCube, const std::vector<uint32_t>& proxyCoord, const std::map<uint32_t, uint32_t>& mapping, uint32_t from)
	{
		auto ndim = realCube.meta().dims.size();

		for(uint32_t i=from; i<ndim; ++i) {
			// is there mapping?
			auto ptr = mapping.find(i);
			if( ptr == mapping.end() ) {
				continue;
			}
			auto i2 = ptr->second;

			//auto dimKey = realCube.meta().dims[i].key;
			//auto& dim = realCube.db()->dim(dimKey);
			auto dim = realCube.dimCache[i];
			const auto& dimItm = dim->itemCache(realCoord[i]);

			if(dimItm.childs.empty()) {
				continue;
			}

			// load real
			auto& realCell = realCube.cell(realCoord);
			auto realCoord2 = realCoord;
			std::vector<Cell> realChilds(dimItm.childs.size());
			for(uint32_t j=0; j<dimItm.childs.size(); ++j) {
				realCoord2[i] = dimItm.childs[j];
				realChilds[j] = realCube.cell(realCoord2);
			}

			// load proxy
			auto proxyCoord2 = proxyCoord;
			for( const auto& mp : mapping ) {
				proxyCoord2[mp.second] = convertId(realCube, mp.first, realCoord[mp.first], proxyCube, mp.second);
			}
			auto proxyCell = proxyCube.cell(proxyCoord2);
			std::vector<Cell> proxyChilds(dimItm.childs.size());
			for(uint32_t j=0; j<dimItm.childs.size(); ++j) {
				proxyCoord2[i2] = convertId(realCube, i, dimItm.childs[j], proxyCube, i2);
				proxyChilds[j] = proxyCube.cell(proxyCoord2);
			}

			calcValues2(realCell, realChilds, proxyCell, proxyChilds, realCube.meta().dims[i].type);
			
			// copy back
			for(uint32_t j=0; j<dimItm.childs.size(); ++j) {
				realCoord2[i] = dimItm.childs[j];
				auto idx = realCube.calcIdx(realCoord2);
				realCube.cells()[idx] = realChilds[j];
			}

			// do down
			for(uint32_t j=0; j<dimItm.childs.size(); ++j) {
				realCoord2[i] = dimItm.childs[j];
				visit2(realCube, realCoord2, proxyCube, proxyCoord, mapping, i);
			}
		}
	}

	void CubeWriter::normalizeWithCube(std::vector<uint32_t> coord, const std::string& cubeAnotherKey, std::vector<uint32_t> coordAnother, const std::map<uint32_t, uint32_t>& mapping)
	{
		assert(coord.size() <= meta().dims.size());
		while(coord.size() < meta().dims.size()) {
			coord.push_back(0);
		}

		auto& cube2 = db_->cube(cubeAnotherKey);
		assert(coordAnother.size() <= cube2.meta().dims.size());
		while(coordAnother.size() < cube2.meta().dims.size()) {
			coordAnother.push_back(0);
		}

		for( const auto& pair : mapping ) {
			assert(pair.first < meta().dims.size());
			assert(pair.second < cube2.meta().dims.size());
			assert(meta().dims[pair.first].type == cube2.meta().dims[pair.second].type);
		}
		if( mapping.empty() ) {
			throw std::runtime_error("normalizeWithCube() mapping is empty");
		}
		
		// cache other dims
		for( const auto& d : cube2.meta().dims ) {
			auto& dim = cube2.db()->dim(d.key);
			dim.cacheStart();
		}

		//
		visit2(*this, coord, cube2, coordAnother, mapping, 0);
	}
}
