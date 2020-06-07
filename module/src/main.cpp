#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include "Dim.hpp"
#include "CubeReader.hpp"
#include "Db.hpp"
//#include <iostream>

// /Users/ibanic/projects/NumSrv13/data-source/dim-geo/3-import.py
// /Users/ibanic/projects/NumSrv13/data-source/sex.py

using namespace NumSrv;
using namespace boost::python;
namespace py = boost::python;

template <typename T>
std::vector<T> extractList(const py::list& items2)
{
	size_t num = py::len(items2);
	std::vector<T> items(num);
	for( size_t i=0; i<num; ++i )
		items[i] = py::extract<T>(items2[i]);
	return items;
}

template <typename TKey, typename TValue>
std::map<TKey, TValue> extractDict(const py::dict& py_dict)
{
	py::list keys = py_dict.keys();
	size_t num = py::len(py_dict);
	std::map<TKey, TValue> map;
	for( size_t i=0; i<num; ++i ) {
		auto key = py::extract<TKey>(keys[i]);
		auto value = py::extract<TValue>(py_dict[keys[i]]);
		map[key] = value;
	}
	return map;
}

template<class T>
py::list std_vector_to_py_list(const std::vector<T>& v)
{
    py::object get_iter = py::iterator<std::vector<T> >();
    py::object iter = get_iter(v);
    py::list l(iter);
    return l;
}


std::string dbDataPath(Db& db)
{
	return db.dataPath().string();
}


boost::python::object dimReaderItemIdByKey(DimReader& reader, const std::string& key)
{
	auto opt = reader.itemIdByKey(key);
	if(opt) {
		uint32_t id = *opt;
		return boost::python::object(id);
	}
	return boost::python::object();  // none
}




BOOST_PYTHON_MODULE(_NumSrvHelper)
{
	//def("cleanWithChilds", cleanWithChilds);

	class_<std::vector<uint32_t> >("UintVect")
	.def(vector_indexing_suite<std::vector<uint32_t> >());

	class_<std::vector<std::string> >("StringVect")
	.def(vector_indexing_suite<std::vector<std::string> >());

	class_<std::map<std::string, std::string> >("MapStringString")
	.def(map_indexing_suite<std::map<std::string, std::string>, true >());

	class_<Dim>("Dim")
		.def_readwrite("key", &Dim::key)
		.def_readwrite("name", &Dim::name)
		.def_readwrite("desc", &Dim::desc)
	;

	class_<DimItem>("DimItem")
		.def_readwrite("id", &DimItem::id)
		.def_readwrite("key", &DimItem::key)
		.def_readwrite("nameShort", &DimItem::nameShort)
		.def_readwrite("nameLong", &DimItem::nameLong)
		.def_readwrite("desc", &DimItem::desc)
		.def_readwrite("parent", &DimItem::parent)
		.def_readwrite("childs", &DimItem::childs)
	;

	class_<std::vector<DimItem> >("DimItemVect")
	.def(vector_indexing_suite<std::vector<DimItem> >());

	class_<DimReader, boost::noncopyable>("DimReader", no_init)
		.def("dim", &DimReader::dim)
		.def("item", &DimReader::item)
		.def("numItems", &DimReader::numItems)
		.def("allParents", &DimReader::allParents)
		.def("itemIdByKey", dimReaderItemIdByKey)
		.def("search", &DimReader::search)
		.def("itemsOnLevel", &DimReader::itemsOnLevel)
	;











	class_<Cell>("Cell")
		.def(init<>())
		.def(init<double>())
		.def_readwrite("num", &Cell::num)
		//.def_readwrite("desc", &Cell::desc)
		.def_readwrite("q", &Cell::q)
	;

	enum_<ConsolidationType>("ConsolidationType")
		.value("sum", ConsolidationType::sum)
		.value("average", ConsolidationType::average)
		.value("last", ConsolidationType::last)
	;

	class_<CubeDim>("CubeDim")
		.def(init<std::string>())
		.def(init<std::string, ConsolidationType>())
		.def_readwrite("key", &CubeDim::key)
		.def_readwrite("type", &CubeDim::type)
	;

	class_<std::vector<CubeDim> >("CubeDimVect")
	.def(vector_indexing_suite<std::vector<CubeDim> >());

	class_<Cube>("Cube")
		.def_readwrite("key", &Cube::key)
		.def_readwrite("name", &Cube::name)
		.def_readwrite("desc", &Cube::desc)
		.def_readwrite("unit", &Cube::unit)
		.def_readwrite("integers", &Cube::integers)
		.def_readwrite("onlyPositive", &Cube::onlyPositive)
		.def_readwrite("other", &Cube::other)
		.def_readwrite("dims", &Cube::dims)
	;



	
	// wrap every method that delas with vectors to output native python list
	// cube.setCell(cube.calcIds(['2018','de']), Cell(50000))
	// cube.setCell([1,1], Cell(50000))
	class_<CubeReader, boost::noncopyable>("CubeReader", no_init)
		.def("meta", &CubeReader::meta, return_internal_reference<>())
		.def("numCells", &CubeReader::numCells)
		.def("calcIds", +[](CubeReader& c, const list& items)->py::list{
			auto res = c.calcIds(extractList<std::string>(items));
			return std_vector_to_py_list(res);
		})
		.def("cell", +[](CubeReader& c, const list& items)->Cell{
			return c.cell(extractList<uint32_t>(items));
		})
		.def("db", &CubeReader::db, return_internal_reference<>())
		/*.def("setCell", +[](CubeReadWrite& c, const list& items, const Cell& cell)->void{
			return c.setCell(extractList<uint32_t>(items), cell);
		})
		.def("setMeta", &CubeReadWrite::setMeta)
		.def("normalize", &CubeReadWrite::normalize)*/
	;



	//class_<CubeWriter>("CubeWriter", no_init)
	class_<CubeWriter, boost::noncopyable>("CubeWriter", init<Db*, Cube>())
		.def("meta", &CubeWriter::meta, return_internal_reference<>())
		.def("numCells", &CubeWriter::numCells)
		.def("calcIds", +[](CubeWriter& c, const list& items)->py::list{
			auto res = c.calcIds(extractList<std::string>(items));
			return std_vector_to_py_list(res);
		})
		.def("db", &CubeWriter::db, return_internal_reference<>())

		.def("cell", +[](CubeWriter& c, const list& items)->const Cell&{
			return c.cell(extractList<uint32_t>(items));
		}, return_internal_reference<>())
		.def("setCellNumber", +[](CubeWriter& c, const list& items, double n)->void{
			return c.setCellNumber(extractList<uint32_t>(items), n);
		})
		.def("normalize", &CubeWriter::normalize)
		.def("predict", +[](CubeWriter& c, const list& coord, uint32_t nthDimTime, uint32_t dimLevelToPredict)->void{
			return c.predict(extractList<uint32_t>(coord), nthDimTime, dimLevelToPredict);
		})
		.def("normalizeWithCube", +[](CubeWriter& c, const list& coord2, const std::string& anotherCube, const list& coordAnother2, const dict& mapping2)->void{
			auto coord = extractList<uint32_t>(coord2);
			auto coordAnother = extractList<uint32_t>(coordAnother2);
			auto mapping = extractDict<uint32_t, uint32_t>(mapping2);
			return c.normalizeWithCube(coord, anotherCube, coordAnother, mapping);
		})
		.def("save", &CubeWriter::save)
		
		/*.def("setCell", +[](CubeReadWrite& c, const list& items, const Cell& cell)->void{
			return c.setCell(extractList<uint32_t>(items), cell);
		})
		.def("setMeta", &CubeReadWrite::setMeta)
		.def("normalize", &CubeReadWrite::normalize)*/
	;









	// return_value_policy<reference_existing_object>()
	// return_internal_reference<>()
	class_<Db, boost::noncopyable>("Db", init<std::string>())
		.def("dataPath", dbDataPath)
		.def("dimKeys", &Db::dimKeys, return_internal_reference<>())
		.def("dim", &Db::dim, return_internal_reference<>())
		.def("dimCreate", +[](Db& db, const Dim& dim, const list& items2) {
			auto items = extractList<DimItem>(items2);
			db.dimCreate(dim, items);
		})

		.def("cubeKeys", &Db::cubeKeys, return_internal_reference<>())
		.def("cube", &Db::cube, return_internal_reference<>())
		//.def("cubeCreate", &Db::cubeCreate)
	;
}

