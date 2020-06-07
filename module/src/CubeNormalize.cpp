#include "CubeWriter.hpp"
#include "Db.hpp"
#include <cmath>
#include "Common.hpp"

namespace NumSrv
{
	Cell calcValuesBottomTop(CubeWriter& cube, std::vector<Cell>& tab, std::vector<uint32_t> coord, uint32_t nth, const std::vector<uint32_t>& childs)
	{
		assert(!childs.empty());
		auto consType = cube.meta().dims[nth].type;
		if(consType == ConsolidationType::sum) {
			float q = 0;
			uint32_t nReal = 0;
			double val = 0;
			for(uint32_t i=0; i<childs.size(); ++i) {
				coord[nth] = childs[i];
				auto idx = cube.calcIdx(coord);
				if(!tab[idx].q.isZero()) {
					q += tab[idx].q.valueOrOne();
					nReal++;
					val += tab[idx].num;
				}
			}
			if( nReal == childs.size() ) {
				return { val, Quality::makeComputed(q / nReal)};
			}
			if( nReal == 0 ) {
				return {0};
			}
			return {val/nReal*childs.size(), Quality::makeComputed(q/childs.size())};
		}
		else if(consType == ConsolidationType::average) {
			float q = 0;
			uint32_t nReal = 0;
			double val = 0;
			for(uint32_t i=0; i<childs.size(); ++i) {
				coord[nth] = childs[i];
				auto idx = cube.calcIdx(coord);
				if(!tab[idx].q.isZero()) {
					q += tab[idx].q.valueOrOne();
					nReal++;
					val += tab[idx].num;
				}
			}
			return {val/nReal, Quality::makeComputed(q/childs.size())};
		}
		else if(consType == ConsolidationType::last) {
			size_t i = childs.size() - 1;
			while(true) {
				coord[nth] = childs[i];
				auto idx = cube.calcIdx(coord);
				if( !tab[idx].q.isZero() ) {
					auto qq = childs.size() - i;
					float q = (tab[idx].q.valueOrOne()+2) / (float)(qq+2);
					return { tab[idx].num, Quality::makeComputed(q) };
				}
				if(i==0)
					break;
				i--;
			}
			return {0};
		}
		else {
			throw std::runtime_error("calcValuesBottomTop() unknown consolidation type");
		}
	}


	void normalizeBottomTop(CubeWriter& cube, std::vector<Cell>& tab, std::vector<bool>& visited, std::vector<uint32_t> coord)
	{
		auto idx = cube.calcIdx(coord);
		if(visited[idx])
			return;
		visited[idx] = true;

		double n = 0;
		Quality q;

		// check if this cell has defined value
		const auto& cell = cube.cell(idx);

		// have to check all cells
		for(uint32_t i=0; i<coord.size(); ++i) {
			//auto dimKey = cube.meta().dims[i].key;
			//auto& dim = cube.db()->dim(dimKey);
			auto dim = cube.dimCache[i];
			const auto& dimItm = dim->itemCache(coord[i]);
			if(dimItm.childs.empty()) {
				continue;
			}
			// visit childs
			//auto coord2 = coord;
			auto origId = coord[i];
			for(uint32_t j=0; j<dimItm.childs.size(); ++j) {
				coord[i] = dimItm.childs[j];
				normalizeBottomTop(cube, tab, visited, coord);
			}
			coord[i] = origId;
			if(cell.q.isComputed()) {
				// calc value from childs
				auto res = calcValuesBottomTop(cube, tab, coord, i, dimItm.childs);
				if( res.q > q ) {
					q = res.q;
					n = res.num;
				}
			}
		}

		if(cell.q.isComputed()) {
			tab[idx] = {n, q};
		}
	}
























	void makeFit(std::vector<Cell>& vals, double n)
	{
		uint32_t nc = vals.size();
		assert(nc != 0);
		if(nc == 1) {
			vals[0].num = n;
			return;
		}

		// make sum of numbers in val equal to n
		double sum_n = 0;
		float sum_q = 0;
		for(uint32_t i=0; i<nc; ++i) {
			sum_n += vals[i].num;
			auto q2 = 1 - vals[i].q.valueOrOne();
			sum_q += q2;
		}

		double change = n - sum_n;
		assert(std::abs(change) > 0.0001);

		if( std::abs(sum_n) > 0.001 && std::abs(change/sum_n) < 2 ) {
			if( sum_q > 0.001 ) {
				// by a factor and quality
				for( uint32_t i=0; i<nc; ++i ) {
					// fact
					double f1 = vals[i].num / sum_n;
					// quality
					auto q2 = 1 - vals[i].q.valueOrOne();
					double f2 = q2 / sum_q;
					// combine
					double f = (f1+f2)/2;
					vals[i].num += change * f;
				}
				return;
			}
			else {
				// only by a factor
				double fact = n / sum_n;
				for( uint32_t i=0; i<nc; ++i ) {
					vals[i].num *= fact;
				}
				return;
			}
		}

		// by fixed amount based on quality
		if( std::abs(change/sum_n) < 2 && sum_q > 0.001 ) {
			for( uint32_t i=0; i<nc; ++i ) {
				auto q2 = 1 - vals[i].q.valueOrOne();
				vals[i].num += change * q2 / sum_q;
			}
			return;
		}

		// set all equaly
		double n2 = n / nc;
		for( uint32_t i=0; i<nc; ++i ) {
			vals[i].num = n2;
		}
		return;
	}

	void calcValuesTopBottom(CubeWriter& cube, std::vector<Cell>& vals, ConsolidationType consType, double n)
	{
		uint32_t nc = vals.size();
		if( consType == ConsolidationType::sum ) {
			// try to chnage only those with q=0
			double real_sum = 0;
			uint32_t real_count = 0;
			double one_sum = 0;
			uint32_t one_count = 0;
			for(uint32_t i=0; i<nc; ++i) {
				if( !vals[i].q.isZero() ) {
					real_count++;
					real_sum += vals[i].num;
					if( vals[i].q.isOneOrMore() ) {
						one_count++;
						one_sum += vals[i].num;
					}
				}
			}

			// is equal?
			if( std::abs(n-real_sum) < 0.0001 ) {
				return;
			}

			// is there any q=0 to change
			if( nc != real_count ) {
				auto countToChange = nc - real_count;
				std::vector<Cell> vals2(countToChange, {0});
				double desired_amount = n - real_sum;
				makeFit(vals2, desired_amount);
				uint32_t nextIdx = 0;
				for(uint32_t i=0; i<nc; ++i) {
					if( vals[i].q.isZero() ) {
						vals[i].num = vals2[nextIdx].num;
						nextIdx++;
					}
				}
				return;
			}

			// is there any q<1 to change
			if( one_count != nc ) {
				std::vector<Cell> vals2;
				vals2.reserve(nc - one_count);
				for(uint32_t i=0; i<nc; ++i) {
					if( vals[i].q.isComputed() && !vals[i].q.isOneOrMore() ) {
						vals2.push_back(vals[i]);
					}
				}
				double desired_amount = n - one_sum;
				makeFit(vals2, desired_amount);
				uint32_t nextIdx = 0;
				for(uint32_t i=0; i<nc; ++i) {
					if( vals[i].q.isComputed() && !vals[i].q.isOneOrMore() ) {
						vals[i].num = vals2[nextIdx].num;
						nextIdx++;
					}
				}
				return;
			}

			// is there any q < 2 to change
			std::vector<Cell> vals2;
			for(uint32_t i=0; i<nc; ++i) {
				if( vals[i].q.isPredicted() ) {
					vals2.push_back(vals[i]);
				}
			}
			if( !vals2.empty() ) {
				double desired_amount = n - one_sum;
				makeFit(vals2, desired_amount);
				uint32_t nextIdx = 0;
				for(uint32_t i=0; i<nc; ++i) {
					if( vals[i].q.isPredicted() ) {
						vals[i].num = vals2[nextIdx].num;
						nextIdx++;
					}
				}
				return;
			}

			// change all
			makeFit(vals, n);
			return;
		}
		else if( consType == ConsolidationType::average ) {
			return calcValuesTopBottom(cube, vals, ConsolidationType::sum, n*vals.size());
		}
		else if( consType == ConsolidationType::last ) {
			assert(!vals.empty());
			uint32_t i = vals.size() - 1;
			while(true) {
				//if( !vals[i].q.isZero() ) {
				if( vals[i].q.valueOrOne() > 0.01 ) {
					n = vals[i].num;
				}
				else {
					vals[i].num = n;
				}
				if( i == 0 ) {
					break;
				}
				i--;
			}
			return;
		}
		else {
			throw std::runtime_error("calcValuesTopBottom() unknown ConsolidationType");
		}
	}


	void normalizeTopBottom(CubeWriter& cube, std::vector<Cell>& cells, std::vector<uint32_t> coord, uint32_t from, double n)
	{
		auto ndim = cube.meta().dims.size();
		auto idx = cube.calcIdx(coord);
		cells[idx].num = n;
		

		for(uint32_t i=from; i<ndim; ++i) {
			//auto dimKey = cube.meta().dims[i].key;
			//auto& dim = cube.db()->dim(dimKey);
			auto dim = cube.dimCache[i];
			const auto& dimItm = dim->itemCache(coord[i]);

			if(dimItm.childs.empty()) {
				continue;
			}

			//auto coord2 = coord;
			auto origId = coord[i];
			std::vector<Cell> vals(dimItm.childs.size());
			for(uint32_t j=0; j<dimItm.childs.size(); ++j) {
				coord[i] = dimItm.childs[j];
				auto idx2 = cube.calcIdx(coord);
				vals[j] = cells[idx2];
			}
			coord[i] = origId;

			calcValuesTopBottom(cube, vals, cube.meta().dims[i].type, n);
			for( size_t j=0; j<dimItm.childs.size(); ++j ) {
				coord[i] = dimItm.childs[j];
				normalizeTopBottom(cube, cells, coord, i, vals[j].num);
			}
			coord[i] = origId;
		}
	}





void normalizeSiblings(CubeWriter& cube, std::vector<Cell>& cells)
{
	auto n = cube.numCells();
	auto ndim = cube.meta().dims.size();
	for(uint64_t idx=0; idx<n; ++idx) {
		if(cells[idx].q.valueOrOne() > 0.01)
			continue;
		// get coord
		std::vector<uint32_t> coord = cube.calcCoord(idx);
		// calc average n, q
		double numBest = 0;
		float qBest = 0;
		for( uint32_t j=0; j<ndim; ++j ) {
			/*if( cube.meta().dims[j].type != ConsolidationType::last ) {
				continue;
			}*/
			auto dim = cube.dimCache[j];
			const auto& itm1 = dim->itemCache(coord[j]);
			if(coord[j] == itm1.parent) {
				continue;
			}
			const auto& itm2 = dim->itemCache(itm1.parent);
			if( itm2.childs.size() <= 1 ) {
				continue;
			}
			uint32_t numValues = 0;
			double numSum = 0;
			float qSum = 0;
			auto orig = coord[j];
			for( uint32_t k=0; k<itm2.childs.size(); ++k ) {
				if(itm2.childs[k] == coord[j]) {
					continue;
				}
				coord[j] = itm2.childs[k];
				auto idx2 = cube.calcIdx(coord);
				const auto& cell2 = cells[idx2];
				if( cell2.q.valueOrOne() > 0.01 ) {
					numSum += cell2.num;
					qSum += cell2.q.valueOrOne();
					numValues++;
				}
			}
			coord[j] = orig;
			float q = qSum / (itm2.childs.size()-1);
			if(q > qBest) {
				numBest = numSum / numValues;
				qBest = q;
			}
		}
		
		if(qBest != 0) {
			cells[idx].num = numBest;
			cells[idx].q = Quality::makeComputed(0.01);
		}
	}
}



























	void CubeWriter::normalize()
	{
		auto ndim = meta().dims.size();
		std::vector<uint32_t> coordTotal(ndim, 0);
		
		// bottom -> top
		std::vector<bool> visited(numCells(), false);
		writeStatus("normalize bottom -> top ...");
		normalizeBottomTop(*this, cells_, visited, coordTotal);
		
		double n;
		if( !cells_[0].q.isZero() ) {
			n = cells_[0].num;
		}
		else {
			n = 0;
		}
		
		// siblings
		writeStatus("normalize siblings ...");
		normalizeSiblings(*this, cells_);
		
		// top -> bottom
		writeStatus("normalize top -> bottom ...");
		normalizeTopBottom(*this, cells_, coordTotal, 0, n);
		writeStatus("normalize finished ...");
	}
}
