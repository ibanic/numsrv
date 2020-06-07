#include "Common.hpp"
#include <iostream>
#include <ctime>

namespace NumSrv {
	void writeStatus(const char* txt) {
		time_t now;
	    time(&now);
	    char buf[sizeof "2011-10-08 07:07:09"];
	    strftime(buf, sizeof buf, "%F %T", gmtime(&now));
	    // this will work too, if your compiler doesn't support %F or %T:
	    //strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));
	    std::cout << buf << " " << txt << std::endl;
	}


	/*void printCoord(CubeWriter& cube, const std::vector<uint32_t>& arr)
	{
		for( size_t i=0; i<arr.size(); ++i ) {
			const auto& dim = cube.dimCache[i];
			auto itm = dim->item(arr[i]);
			std::cout << itm.key << "\t";
		}
		std::cout << std::endl;
	}*/
}