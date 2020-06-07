//
//  CubeWriter.hpp
//  NumSrv
//
//  Created by Ignac Banic on 09/05/20.
//  Copyright © 2020 Ignac Banic. All rights reserved.
//

#pragma once

#include "CubeCommon.hpp"
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

namespace NumSrv {

	class Db;
	class DimReader;

	class CubeWriter {
	private:
		Db* db_;
		Cube meta_;
		std::vector<uint64_t> multipliers_;
		std::vector<Cell> cells_;
		
		

	public:
		CubeWriter(Db* db, const Cube& meta);
		CubeWriter(const CubeWriter&) = delete;
		CubeWriter& operator=(const CubeWriter&) = delete;
		CubeWriter(CubeWriter&&) = default;
		CubeWriter& operator=(CubeWriter&&) = default;
		
		
		std::vector<DimReader*> dimCache;

				
		const Cube& meta() const { return meta_; }
		uint64_t numCells() const { return multipliers_.back(); }
		std::vector<uint32_t> calcIds(const std::vector<std::string>& keys) const;
		Db* db() const { return db_; }

		const Cell& cell(const std::vector<uint32_t>& ids) const;
		const Cell& cell(uint64_t idx) const { return cells_.at(idx); }
		std::vector<Cell>& cells() { return cells_; }

		void setCellNumber(const std::vector<uint32_t>& ids, double n);
		void setCellNumber(uint64_t idx, double n);

		void normalize();
		void predict(std::vector<uint32_t> coord, uint32_t nthDimTime, uint32_t levelForPrediction);
		void normalizeWithCube(std::vector<uint32_t> coord, const std::string& cubeAnotherKey, std::vector<uint32_t> coordAnother, const std::map<uint32_t, uint32_t>& mapping);
		
		void save() const;

		uint64_t calcIdx(const std::vector<uint32_t>& coord) const;
		std::vector<uint32_t> calcCoord(uint64_t idx) const;
	};
}
