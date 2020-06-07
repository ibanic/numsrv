//
//  Db.hpp
//  NumSrv
//
//  Created by Ignac Banic on 10/08/19.
//  Copyright © 2019 Ignac Banic. All rights reserved.
//

#ifndef Db_hpp
#define Db_hpp

#include <string>
#include <vector>
#include <filesystem>
#include "Dim.hpp"
#include "CubeReader.hpp"
#include "CubeWriter.hpp"

namespace fs = std::filesystem;

namespace NumSrv
{
	class Db {
	private:
		fs::path dataPath_;
		std::vector<std::string> dimKeys_;
		std::vector<DimReader> dims_;
		std::vector<std::string> cubeKeys_;
		std::vector<CubeReader> cubes_;

	public:
		Db(const fs::path& dataPath);
		Db(const Db&) = delete;
		Db& operator=(const Db&) = delete;

		const fs::path& dataPath() const { return dataPath_; }
		const std::vector<std::string>& dimKeys() const { return dimKeys_; }
		const std::vector<std::string>& cubeKeys() const { return cubeKeys_; }

		DimReader& dim(const std::string& key);
		CubeReader& cube(const std::string& key);

		void dimCreate(const Dim& dim, std::vector<DimItem>& items);
		CubeWriter cubeCreate(const Cube& meta);
		void loadCube(const std::string& key);

	};

}


#endif /* Dim_hpp */
