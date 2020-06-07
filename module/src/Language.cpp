#include "Language.hpp"
#include <array>
#include "BinaryReadWrite.hpp"

namespace NumSrv
{
	uint8_t getLanguageCode(const std::string& lang)
	{
		if(lang=="") return 0;
		if(lang=="aa") return 1;
		if(lang=="ab") return 2;
		if(lang=="af") return 3;
		if(lang=="ak") return 4;
		if(lang=="sq") return 5;
		if(lang=="am") return 6;
		if(lang=="ar") return 7;
		if(lang=="an") return 8;
		if(lang=="hy") return 9;
		if(lang=="as") return 10;
		if(lang=="av") return 11;
		if(lang=="ae") return 12;
		if(lang=="ay") return 13;
		if(lang=="az") return 14;
		if(lang=="ba") return 15;
		if(lang=="bm") return 16;
		if(lang=="eu") return 17;
		if(lang=="be") return 18;
		if(lang=="bn") return 19;
		if(lang=="bh") return 20;
		if(lang=="bi") return 21;
		if(lang=="bo") return 22;
		if(lang=="bs") return 23;
		if(lang=="br") return 24;
		if(lang=="bg") return 25;
		if(lang=="my") return 26;
		if(lang=="ca") return 27;
		if(lang=="cs") return 28;
		if(lang=="ch") return 29;
		if(lang=="ce") return 30;
		if(lang=="zh") return 31;
		if(lang=="cu") return 32;
		if(lang=="cv") return 33;
		if(lang=="kw") return 34;
		if(lang=="co") return 35;
		if(lang=="cr") return 36;
		if(lang=="cy") return 37;
		if(lang=="cs") return 38;
		if(lang=="da") return 39;
		if(lang=="de") return 40;
		if(lang=="dv") return 41;
		if(lang=="nl") return 42;
		if(lang=="dz") return 43;
		if(lang=="el") return 44;
		if(lang=="en") return 45;
		if(lang=="eo") return 46;
		if(lang=="et") return 47;
		if(lang=="eu") return 48;
		if(lang=="ee") return 49;
		if(lang=="fo") return 50;
		if(lang=="fa") return 51;
		if(lang=="fj") return 52;
		if(lang=="fi") return 53;
		if(lang=="fr") return 54;
		if(lang=="fr") return 55;
		if(lang=="fy") return 56;
		if(lang=="ff") return 57;
		if(lang=="ka") return 58;
		if(lang=="de") return 59;
		if(lang=="gd") return 60;
		if(lang=="ga") return 61;
		if(lang=="gl") return 62;
		if(lang=="gv") return 63;
		if(lang=="el") return 64;
		if(lang=="gn") return 65;
		if(lang=="gu") return 66;
		if(lang=="ht") return 67;
		if(lang=="ha") return 68;
		if(lang=="he") return 69;
		if(lang=="hz") return 70;
		if(lang=="hi") return 71;
		if(lang=="ho") return 72;
		if(lang=="hr") return 73;
		if(lang=="hu") return 74;
		if(lang=="hy") return 75;
		if(lang=="ig") return 76;
		if(lang=="is") return 77;
		if(lang=="io") return 78;
		if(lang=="ii") return 79;
		if(lang=="iu") return 80;
		if(lang=="ie") return 81;
		if(lang=="ia") return 82;
		if(lang=="id") return 83;
		if(lang=="ik") return 84;
		if(lang=="is") return 85;
		if(lang=="it") return 86;
		if(lang=="jv") return 87;
		if(lang=="ja") return 88;
		if(lang=="kl") return 89;
		if(lang=="kn") return 90;
		if(lang=="ks") return 91;
		if(lang=="ka") return 92;
		if(lang=="kr") return 93;
		if(lang=="kk") return 94;
		if(lang=="km") return 95;
		if(lang=="ki") return 96;
		if(lang=="rw") return 97;
		if(lang=="ky") return 98;
		if(lang=="kv") return 99;
		if(lang=="kg") return 100;
		if(lang=="ko") return 101;
		if(lang=="kj") return 102;
		if(lang=="ku") return 103;
		if(lang=="lo") return 104;
		if(lang=="la") return 105;
		if(lang=="lv") return 106;
		if(lang=="li") return 107;
		if(lang=="ln") return 108;
		if(lang=="lt") return 109;
		if(lang=="lb") return 110;
		if(lang=="lu") return 111;
		if(lang=="lg") return 112;
		if(lang=="mk") return 113;
		if(lang=="mh") return 114;
		if(lang=="ml") return 115;
		if(lang=="mi") return 116;
		if(lang=="mr") return 117;
		if(lang=="ms") return 118;
		if(lang=="mk") return 119;
		if(lang=="mg") return 120;
		if(lang=="mt") return 121;
		if(lang=="mn") return 122;
		if(lang=="mi") return 123;
		if(lang=="ms") return 124;
		if(lang=="my") return 125;
		if(lang=="na") return 126;
		if(lang=="nv") return 127;
		if(lang=="nr") return 128;
		if(lang=="nd") return 129;
		if(lang=="ng") return 130;
		if(lang=="ne") return 131;
		if(lang=="nl") return 132;
		if(lang=="nn") return 133;
		if(lang=="nb") return 134;
		if(lang=="no") return 135;
		if(lang=="ny") return 136;
		if(lang=="oc") return 137;
		if(lang=="oj") return 138;
		if(lang=="or") return 139;
		if(lang=="om") return 140;
		if(lang=="os") return 141;
		if(lang=="pa") return 142;
		if(lang=="fa") return 143;
		if(lang=="pi") return 144;
		if(lang=="pl") return 145;
		if(lang=="pt") return 146;
		if(lang=="ps") return 147;
		if(lang=="qu") return 148;
		if(lang=="rm") return 149;
		if(lang=="ro") return 150;
		if(lang=="ro") return 151;
		if(lang=="rn") return 152;
		if(lang=="ru") return 153;
		if(lang=="sg") return 154;
		if(lang=="sa") return 155;
		if(lang=="si") return 156;
		if(lang=="sk") return 157;
		if(lang=="sk") return 158;
		if(lang=="sl") return 159;
		if(lang=="se") return 160;
		if(lang=="sm") return 161;
		if(lang=="sn") return 162;
		if(lang=="sd") return 163;
		if(lang=="so") return 164;
		if(lang=="st") return 165;
		if(lang=="es") return 166;
		if(lang=="sq") return 167;
		if(lang=="sc") return 168;
		if(lang=="sr") return 169;
		if(lang=="ss") return 170;
		if(lang=="su") return 171;
		if(lang=="sw") return 172;
		if(lang=="sv") return 173;
		if(lang=="ty") return 174;
		if(lang=="ta") return 175;
		if(lang=="tt") return 176;
		if(lang=="te") return 177;
		if(lang=="tg") return 178;
		if(lang=="tl") return 179;
		if(lang=="th") return 180;
		if(lang=="bo") return 181;
		if(lang=="ti") return 182;
		if(lang=="to") return 183;
		if(lang=="tn") return 184;
		if(lang=="ts") return 185;
		if(lang=="tk") return 186;
		if(lang=="tr") return 187;
		if(lang=="tw") return 188;
		if(lang=="ug") return 189;
		if(lang=="uk") return 190;
		if(lang=="ur") return 191;
		if(lang=="uz") return 192;
		if(lang=="ve") return 193;
		if(lang=="vi") return 194;
		if(lang=="vo") return 195;
		if(lang=="cy") return 196;
		if(lang=="wa") return 197;
		if(lang=="wo") return 198;
		if(lang=="xh") return 199;
		if(lang=="yi") return 200;
		if(lang=="yo") return 201;
		if(lang=="za") return 202;
		if(lang=="zh") return 203;
		if(lang=="zu") return 204;
		throw std::runtime_error("Unknown language");
	}


	std::array<std::string, 205> lang = { {
		"","aa","ab","af","ak","sq","am","ar","an","hy","as","av","ae","ay","az","ba","bm","eu","be","bn","bh","bi","bo","bs","br","bg","my","ca","cs","ch","ce","zh","cu","cv","kw","co","cr","cy","cs","da","de","dv","nl","dz","el","en","eo","et","eu","ee","fo","fa","fj","fi","fr","fr","fy","ff","ka","de","gd","ga","gl","gv","el","gn","gu","ht","ha","he","hz","hi","ho","hr","hu","hy","ig","is","io","ii","iu","ie","ia","id","ik","is","it","jv","ja","kl","kn","ks","ka","kr","kk","km","ki","rw","ky","kv","kg","ko","kj","ku","lo","la","lv","li","ln","lt","lb","lu","lg","mk","mh","ml","mi","mr","ms","mk","mg","mt","mn","mi","ms","my","na","nv","nr","nd","ng","ne","nl","nn","nb","no","ny","oc","oj","or","om","os","pa","fa","pi","pl","pt","ps","qu","rm","ro","ro","rn","ru","sg","sa","si","sk","sk","sl","se","sm","sn","sd","so","st","es","sq","sc","sr","ss","su","sw","sv","ty","ta","tt","te","tg","tl","th","bo","ti","to","tn","ts","tk","tr","tw","ug","uk","ur","uz","ve","vi","vo","cy","wa","wo","xh","yi","yo","za","zh","zu"
	} };

	const std::string& getLanguageFromCode(uint8_t c)
	{
		if(c < lang.size())
			return lang[c];
		throw std::invalid_argument("Language code doesnt exist");
	}








	size_t sizeMapLang(const std::vector<std::pair<uint8_t, std::string> >& map) {
		size_t sz = 0;
		sz += BinaryReadWrite::sizeUint8();
		for( const auto& pair : map ) {
			sz += BinaryReadWrite::sizeUint8();
			sz += BinaryReadWrite::sizeCString(pair.second);
		}
		return sz;
	}

	void writeMapLang(Byte*& s, const std::vector<std::pair<uint8_t, std::string> >& map) {
		BinaryReadWrite::writeUint8(s, map.size());
		for( const auto& pair : map ) {
			BinaryReadWrite::writeUint8(s, pair.first);
			BinaryReadWrite::writeCString(s, pair.second);
		}
	}

	std::vector<std::pair<uint8_t, std::string> > readMapLang(const Byte*& s) {
		auto num = BinaryReadWrite::readUint8(s);
		std::vector<std::pair<uint8_t, std::string> > map(num);
		for( uint8_t i=0; i<num; ++i ) {
			map[i].first = BinaryReadWrite::readUint8(s);
			map[i].second = BinaryReadWrite::readCString(s);
		}
		return map;
	}

	std::vector<std::pair<uint8_t, std::string> > mapLangToNum(const std::map<std::string, std::string>& map) {
		std::vector<std::pair<uint8_t, std::string> > map2(map.size());
		size_t i = 0;
		for( const auto& pair : map ) {
			map2[i].first = getLanguageCode(pair.first);
			map2[i].second = pair.second;
			i++;
		}
		return map2;
	}

	std::map<std::string, std::string> mapLangFromNum(const std::vector<std::pair<uint8_t, std::string> >& map) {
		std::map<std::string, std::string> map2;
		for( const auto& pair : map ) {
			map2.insert({getLanguageFromCode(pair.first), pair.second});
		}
		return map2;
	}
}
