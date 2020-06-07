#from _NumSrvHelper import Dim, DimItem, writeDimAndItems
#import _NumSrvHelper

#from _NumSrvHelper import *
from ._NumSrvHelper import *
#import ._NumSrvHelper
import os.path
import os
import time
#import transliterate

def t():
	return time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())



def lastDataDir(d):
	#return '/Users/ibanic/projects/NumSrv13/data-compiled/1'

	dirs = [os.path.join(d, o) for o in os.listdir(d) 
				if os.path.isdir(os.path.join(d,o))]
	dirs2 = [os.path.basename(path) for path in dirs]
	maxNth = None
	maxNum = 0
	for i in range(len(dirs)):
		try:
			n = int(dirs2[i])
		except ValueError:
			continue
		if n >= maxNum:
			maxNum = n
			maxNth = i

	if maxNth == None:
		raise Exception('Cant find latest data path')
	return dirs[maxNth]