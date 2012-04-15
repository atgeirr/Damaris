import sys
import damaris
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
print "Drawing from Python, iteration "+str(damaris.iteration)
var = damaris.open("images/julia")
for c in var.select({"iteration":damaris.iteration}):
	image = np.empty((640,480,3), dtype=np.uint8)
	d = c.data
	image[:,:,0] = d
	image[:,:,1] = d
	image[:,:,2] = d
	plot = plt.imshow(image)
	plot.write_png("result%d.png" % c.iteration)
	plt.clf()
