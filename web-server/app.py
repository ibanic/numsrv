# server.py
from flask import Flask, jsonify, request, abort, make_response
from flask_cors import CORS
import NumSrvHelper
import os.path
import urllib.parse
import json
from converters import *

db = None

def loadDimsAndCubes():
	global db
	# get latest data dir
	d = os.path.abspath(__file__)
	d = os.path.dirname(d)
	d = os.path.dirname(d)
	d = os.path.join(d, 'data-compiled')
	dataPath = NumSrvHelper.lastDataDir(d)

	db = NumSrvHelper.Db(dataPath)

loadDimsAndCubes()






app = Flask(__name__, static_folder="../website/dist")  #, template_folder="../static"
CORS(app)


@app.errorhandler(404)
def resource_not_found(e):
	if request.path.startswith('/api/'):
		return jsonify(error=str(e)), 404
	else:
		response = make_response('<!DOCTYPE html><html><head><title>page not found</title></head><body><h1>Error page not found</h1><a href="/">Go home</a></body>', 404)
		response.mimetype = "text/html; charset=utf-8"
		return response


@app.route('/')
def index():
	return app.send_static_file('index.html')


@app.route('/main.js')
def mainJs():
	return app.send_static_file('main.js')





@app.route('/api/dims', methods=['GET'])
def apiDims():
	arr = [dimShort(db, dimKey) for dimKey in db.dimKeys()]
	return jsonify(arr)


@app.route('/api/dims/<dimKey>', methods=['GET'])
def apiDimDetails(dimKey):
	res = dimLong(db, dimKey)
	if res == False:
		return abort(404, description='Not found')
	return jsonify(res)


@app.route('/api/dims/<dimKey>/items/<int:itemId>', methods=['GET'])
def apiDimItemDetails(dimKey, itemId):
	res = dimItemLong(db, dimKey, itemId)
	if res == False:
		return abort(404, description='Not found')
	return jsonify(res)


@app.route('/api/dims/<dimKey>/itemsMany', methods=['POST'])
def apiDimItemManyDetails(dimKey):
	ids = request.get_json()
	arr = [dimItemLong(db, dimKey, itemId) for itemId in ids]
	return jsonify(arr)


@app.route('/api/dims/<dimKey>/search', methods=['POST'])
def apiDimSearch(dimKey):
	rq = request.get_json()
	lang = rq['lang'] if 'lang' in rq else ''
	assert isinstance(rq['query'], str)
	try:
		dim = db.dim(dimKey)
	except:
		return abort(404, description='Not found')

	if 'multipleQueries' in rq and rq['multipleQueries']:
		lst = []
		queries = rq['query'].split(',')
		for q in queries:
			res = dim.search(q, lang)
			lst.append(res[0] if len(res)>0 else None)
	else:
		q = rq['query']
		lst = dim.search(q, lang)

	lst2 = [(dimItemShort(db, dimKey, id2) if id2!=None else None) for id2 in lst]
	return jsonify(lst2)





@app.route('/api/cubes', methods=['GET'])
def apiCubes():
	arr = [cubeMeta(db, n) for n in db.cubeKeys()]
	return jsonify(arr)



@app.route('/api/cubes/<cubeKey>', methods=['GET'])
def apiCubeDetails(cubeKey):
	res = cubeMeta(db, cubeKey)
	if res == False:
		return abort(404, description='Not found')
	return jsonify(res)



@app.route('/api/cubes/<cubeKey>/cellValuesById', methods=['POST'])
def apiCubeCellValuesById(cubeKey):
	ids = request.get_json()
	try:
		cube = db.cube(cubeKey)
	except:
		return abort(404, description='Not found')
	arr = []
	for id2 in ids:
		try:
			c = cube.cell(id2)
		except:
			arr.append(None)
		n = c.num
		arr.append(n)
	return jsonify(arr)



@app.route('/api/cubes/<cubeKey>/cellById/', methods=['GET'])
@app.route('/api/cubes/<cubeKey>/cellById/<path:subpath>', methods=['GET'])
def apiCubeCellById(cubeKey, subpath=None):
	if subpath:
		ids = subpath.split('/')
		ids = [int(n) for n in ids]
	else:
		ids = []
	res = cubeCell(db, cubeKey, ids)
	if res == False:
		return abort(404, description='Not found')
	return jsonify(res)



@app.route('/api/reload/yfg34bdg9bq3t63fwfngvbqhgaghei', methods=['POST'])
def apiReload():
	global dataPath
	res = { 'dataPathOld': dataPath, }
	loadDimsAndCubes()
	res['dataPathNew'] = dataPath
	return jsonify(res)




if __name__ == "__main__":
	app.run()
