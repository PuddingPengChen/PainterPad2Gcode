#-*-coding:utf-8-*-
import sys
import wx
import numpy
import meshLoader
import printableObject


class imageLoad(object):
	def __init__(self):
		self._machineSize = numpy.array([100,100,100], numpy.float32)	

	def convertImage(self,filename, height=20.0, width=100.0, blur=0, invert=False, baseHeight=1.0):
		image = wx.Image(filename)
		image.ConvertToGreyscale()
		image.SaveFile("gray.png",wx.BITMAP_TYPE_PNG)
		if image.GetHeight() > 200:
			image.Rescale(image.GetWidth() * 200 / image.GetHeight(), 200, wx.IMAGE_QUALITY_HIGH)
		if image.GetWidth() > 200:
			image.Rescale(200, image.GetHeight() * 200 / image.GetWidth(), wx.IMAGE_QUALITY_HIGH)
		if blur > 0:
			image = image.Blur(blur)
		z = numpy.fromstring(image.GetData(), numpy.uint8)
		z = numpy.array(z[::3], numpy.float32)
		#Only get the R values (as we are grayscale), and convert to float values
        # 如果进行反转
		if invert:
			z = 255 - z

        #生成最小最大值
		pMin, pMax = numpy.min(z), numpy.max(z)
		if pMax == pMin:
			pMax += 1.0

        #根据像素深度来生成对应的物理高度(同时需要加上基本的高度)
		z = ((z - pMin) * height / (pMax - pMin)) + baseHeight

        #图片的基本宽和高
		w, h = image.GetWidth(), image.GetHeight()
        #图片需要缩放的比例(根据外部给出的参数来设定)
		scale = width / (image.GetWidth() - 1)
		n = w * h

        #>>> np.mgrid[0:5,0:5]
        # array([[[0, 0, 0, 0, 0],
        #         [1, 1, 1, 1, 1],
        #         [2, 2, 2, 2, 2],
        #         [3, 3, 3, 3, 3],
        #         [4, 4, 4, 4, 4]],
        #        [[0, 1, 2, 3, 4],
        #         [0, 1, 2, 3, 4],
        #         [0, 1, 2, 3, 4],
        #         [0, 1, 2, 3, 4],
        #         [0, 1, 2, 3, 4]]])

		y, x = numpy.mgrid[0:h,0:w]
		x = numpy.array(x, numpy.float32, copy=False) * scale
		y = numpy.array(y, numpy.float32, copy=False) *-scale
		v0 = numpy.concatenate((x.reshape((n, 1)), y.reshape((n, 1)), z.reshape((n, 1))), 1)

        #得到一个三维数组
		v0 = v0.reshape((h, w, 3))

		v1 = v0[0:-1,0:-1,:]
		v2 = v0[0:-1,1:,:]
		v3 = v0[1:,0:-1,:]
		v4 = v0[1:,1:,:]

		obj = printableObject.printableObject(filename)
		m = obj._addMesh()

		#欧拉公式的点面关系
		m._prepareFaceCount((w-1) * (h-1) * 2 + 2 + (w-1)*4 + (h-1)*4)
		m.vertexes = numpy.array(numpy.concatenate((v1,v3,v2,v2,v3,v4), 2).reshape(((w-1) * (h-1) * 6, 3)), numpy.float32, copy=False)
		m.vertexes = numpy.concatenate((m.vertexes, numpy.zeros(((2+(w-1)*4+(h-1)*4)*3, 3), numpy.float32)))
		m.vertexCount = (w-1) * (h-1) * 6
		x = (w-1)* scale
		y = (h-1)*-scale
		m._addFace(0,0,0, x,0,0, 0,y,0)
		m._addFace(x,y,0, 0,y,0, x,0,0)
		for n in xrange(0, w-1):
			x = n* scale
			i = w*h-w+n
			m._addFace(x+scale,0,0, x,0,0, x,0,z[n])
			m._addFace(x+scale,0,0, x,0,z[n], x+scale,0,z[n+1])
			m._addFace(x+scale,y,0, x,y,z[i], x,y,0)
			m._addFace(x+scale,y,0, x+scale,y,z[i+1], x,y,z[i])

		x = (w-1)*scale
		for n in xrange(0, h-1):
			y = n*-scale
			i = n*w+w-1
			m._addFace(0,y-scale,0, 0,y,z[n*w], 0,y,0)
			m._addFace(0,y-scale,0, 0,y-scale,z[n*w+w], 0,y,z[n*w])
			m._addFace(x,y-scale,0, x,y,0, x,y,z[i])
			m._addFace(x,y-scale,0, x,y,z[i], x,y-scale,z[i+w])
		obj._postProcessAfterLoad()
		return obj


	def modefiled(self,obj, positionOnly=False):
		if not positionOnly and numpy.max(obj.getSize()[0:2]) > numpy.max(self._machineSize[0:2]) * 2.5:
			scale = numpy.max(self._machineSize[0:2]) * 2.5 / numpy.max(obj.getSize()[0:2])
			matrix = [[scale,0,0], [0, scale, 0], [0, 0, scale]]
			obj.applyMatrix(numpy.matrix(matrix, numpy.float64))
		return obj

	def saved(self,filename,objs):
		meshLoader.saveMeshes(filename, objs)

if __name__ == "__main__":
	modelsaver = imageLoad()
	fi = "in.png"
	if(sys.argv[3]=='invert'):
		wi = float(sys.argv[4])
		obj = modelsaver.convertImage(sys.argv[1],1.0,wi,0,True,0.0)
	if(sys.argv[3]=='noinvert'):
		we = float(sys.argv[4])
		obj = modelsaver.convertImage(sys.argv[1],1.0,we,0,False,0.0)
	obje = modelsaver.modefiled(obj)
	modelsaver.saved(sys.argv[2],obje)

