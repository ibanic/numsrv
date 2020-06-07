import json


def cppToDict(dict2):
	dict3 = {}
	for pair in dict2:
		dict3[pair.key()] = pair.data()
	return dict3



def cppToList(lst2):
	lst = []
	for itm in lst2:
		lst.append(itm)
	return lst



def dimItemShort(db, dimKey, itmId):
	try:
		obj = db.dim(dimKey).item(itmId)
	except:
		return False
	return {
		'id': itmId,
		'key': obj.key,
		'nameShort':cppToDict(obj.nameShort),
		'nameLong':cppToDict(obj.nameLong),
	}



def dimItemLong(db, dimKey, itmId):
	try:
		obj = db.dim(dimKey).item(itmId)
	except:
		return False
	return {
		'id': itmId,
		'key': obj.key,
		'nameShort':cppToDict(obj.nameShort),
		'nameLong':cppToDict(obj.nameLong),
		'parent': dimItemShort(db, dimKey, obj.parent) if itmId != 0 else None,
		'desc': cppToDict(obj.desc),
		'childs': list(obj.childs),
	}



def cubeMeta(db, cubeKey):
	try:
		obj = db.cube(cubeKey).meta()
	except:
		return False
	dims = []
	for dd in obj.dims:
		obj2 = db.dim(dd.key).dim()
		dims.append({'key':dd.key, 'name':cppToDict(obj2.name), 'type':str(dd.type)})
	dt = {
		'key':	obj.key,
		'name':	cppToDict(obj.name),
		'desc':	cppToDict(obj.desc),
		'unit':	obj.unit,
		'integers':	obj.integers,
		'onlyPositive':	obj.onlyPositive,
		'other':	json.loads(obj.other) if len(obj.other) > 0 else None,
		'dims':	dims,
	}
	return dt



def dimShort(db, dimKey):
	try:
		obj = db.dim(dimKey)
	except:
		return False
	obj = obj.dim()
	return {
		'key': dimKey,
		'name': cppToDict(obj.name),
	}
	
	
def dimLong(db, dimKey):
	try:
		obj = db.dim(dimKey)
	except:
		return False
	obj = obj.dim()
	return {
		'key': dimKey,
		'name': cppToDict(obj.name),
		'desc': cppToDict(obj.desc),
		'root': dimItemLong(db, dimKey,0),
	}



def cubeCell(db, cubeKey, ids):
	try:
		cube = db.cube(cubeKey)
		cell = cube.cell(ids)
	except:
		return False
	return {
		'value': cell.num,
		#'desc': cell.desc,
	}
