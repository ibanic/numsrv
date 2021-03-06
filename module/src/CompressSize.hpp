//
//  CompressSize.hpp
//  Ignac Banic
//
//  Created by Ignac Banic on 11/01/20.
//  Copyright © 2020 Ignac Banic. All rights reserved.
//

#pragma once


#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <limits.h>
#include <string>
#include "Types.hpp"


namespace NumSrv {
	
	
	// https://stackoverflow.com/a/4956493
	template <typename T>
	T swap_endian(T u)
	{
		static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
		
		union
		{
			T u;
			unsigned char u8[sizeof(T)];
		} source, dest;
		
		source.u = u;
		
		for (size_t k = 0; k < sizeof(T); k++)
			dest.u8[k] = source.u8[sizeof(T) - k - 1];
		
		return dest.u;
	}
	
	uint64_t readSize(const Byte*& data);

	void writeSize(Byte*& data, uint64_t size, uint8_t numBytes=0);
	Bytes writeSizeString(uint64_t size, uint8_t numBytes=0);

	uint8_t numBytesSize(size_t size);

}
