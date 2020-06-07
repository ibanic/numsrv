//
//  CubeReader.hpp
//  NumSrv
//
//  Created by Ignac Banic on 24/07/19.
//  Copyright © 2017 Ignac Banic. All rights reserved.
//

#pragma once

#include "CubeCommon.hpp"
#include <filesystem>
#include <boost/iostreams/device/mapped_file.hpp>


namespace fs = std::filesystem;

namespace NumSrv
{
	class Db;




	class CubeReader
	{
	private:
		Db* db_;
		fs::path path_;
		boost::iostreams::mapped_file file_;
		Cube meta_;
		std::vector<uint64_t> multipliers_;

	public:
		CubeReader(Db* db, const std::string& cubeKey);
		CubeReader(const CubeReader&) = delete;
		CubeReader& operator=(const CubeReader&) = delete;
		CubeReader(CubeReader&&) = default;
		CubeReader& operator=(CubeReader&&) = default;
		
		const Cube& meta() const { return meta_; }
		uint64_t numCells() const { return multipliers_.back(); }
		std::vector<uint32_t> calcIds(const std::vector<std::string>& keys) const;
		Db* db() const { return db_; }

		Cell cell(const std::vector<uint32_t>& ids) const;
		Cell cell(uint64_t idx) const;
		

	private:
		Byte* data() const;
	};
}
