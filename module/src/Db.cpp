#include "Db.hpp"
#include <iostream>

namespace NumSrv
{

	Db::Db(const fs::path& dataPath)
		: dataPath_(dataPath)
	{
		for( const auto& e : fs::directory_iterator(dataPath) ) {
			auto path = e.path();
			if( path.extension() != ".bin" )
				continue;
			auto name = path.filename().string();
			if( name.substr(0,9) == "dim-data-" ) {
				auto n = name.substr(9, name.size()-9-4);
				dimKeys_.push_back(n);
			}
			if( name.substr(0,10) == "cube-data-" ) {
				auto n = name.substr(10, name.size()-10-4);
				cubeKeys_.push_back(n);
			}
		}

		// dims
		for( const auto& key : dimKeys_ )
			dims_.emplace_back(this, key);

		// cubes
		for( const auto& key : cubeKeys_ )
			cubes_.emplace_back(this, key);
	}


	DimReader& Db::dim(const std::string& key) {
		auto ptr = std::find(dimKeys_.begin(), dimKeys_.end(), key);
		if( ptr != dimKeys_.end() )
			return dims_[ptr-dimKeys_.begin()];
		throw std::runtime_error("Dim doesnt exist");
	}


	CubeReader& Db::cube(const std::string& key) {
		auto ptr = std::find(cubeKeys_.begin(), cubeKeys_.end(), key);
		if( ptr != cubeKeys_.end() )
			return cubes_[ptr-cubeKeys_.begin()];
		throw std::runtime_error("Cube doesnt exist");
	}




	void Db::dimCreate(const Dim& dim, std::vector<DimItem>& items)
	{
		auto ptr = std::find(dimKeys_.begin(), dimKeys_.end(), dim.key);
		if( ptr != dimKeys_.end() ) {
			size_t idx = ptr - dimKeys_.begin();
			dims_.erase(dims_.begin()+idx);
			dimKeys_.erase(ptr);
		}
		
		writeDimAndItems(this, dim, items);
		
		dimKeys_.push_back(dim.key);
		dims_.emplace_back(this, dim.key);
	}

	CubeWriter Db::cubeCreate(const Cube& meta)
	{
		return CubeWriter(this, meta);
	}


	void Db::loadCube(const std::string& key)
	{
		auto ptr = std::find(cubeKeys_.begin(), cubeKeys_.end(), key);
		if( ptr != cubeKeys_.end() ) {
			size_t idx = ptr - cubeKeys_.begin();
			cubes_.erase(cubes_.begin()+idx);
			cubeKeys_.erase(ptr);
		}
		
		cubeKeys_.push_back(key);
		cubes_.emplace_back(this, key);
	}


	/*void Db::cubeCreate(const Cube& meta)
	{
		auto ptr = std::find(cubeKeys_.begin(), cubeKeys_.end(), meta.key);
		if( ptr != cubeKeys_.end() ) {
			size_t idx = ptr - cubeKeys_.begin();
			cubes_.erase(cubes_.begin()+idx);
			cubeKeys_.erase(ptr);
		}
		
		CubeReadWrite::create(this, meta);
		
		cubeKeys_.push_back(meta.key);
		cubes_.emplace_back(this, meta.key);
	}*/


}

