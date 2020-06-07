//
//  Types.hpp
//  NumSrv
//
//  Created by Ignac Banic on 07/05/20.
//  Copyright © 2020 Ignac Banic. All rights reserved.
//

#pragma once

#include <string>
#include <string_view>

namespace NumSrv
{
	typedef unsigned char Byte;
	typedef std::basic_string<Byte> Bytes;
	typedef std::basic_string_view<Byte> BytesView;
}