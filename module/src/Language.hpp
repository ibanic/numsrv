//
//  Language.hpp
//  NumSrvLib
//
//  Created by Ignac Banic on 07/08/19.
//  Copyright © 2019 Ignac Banic. All rights reserved.
//

#ifndef Language_hpp
#define Language_hpp

#include <string>
#include <vector>
#include <map>
#include "Types.hpp"


namespace NumSrv
{
	uint8_t getLanguageCode(const std::string& lang);
	const std::string& getLanguageFromCode(uint8_t c);


	size_t sizeMapLang(const std::vector<std::pair<uint8_t, std::string> >& map);
	void writeMapLang(Byte*& s, const std::vector<std::pair<uint8_t, std::string> >& map);
	std::vector<std::pair<uint8_t, std::string> > readMapLang(const Byte*& s);
	std::vector<std::pair<uint8_t, std::string> > mapLangToNum(const std::map<std::string, std::string>& map);
	std::map<std::string, std::string> mapLangFromNum(const std::vector<std::pair<uint8_t, std::string> >& map);

}

#endif /* Language_hpp */