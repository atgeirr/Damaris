import sys
import damaris
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
print "Drawing from Python, iteration "+str(damaris.iteration)
var = damaris.open("images/julia")
image = np.empty((320,240,3), dtype=np.uint8)

for c in var.select({"iteration":damaris.iteration}):
	d = c.data
	x0 = c.lower_bounds[0]
	x1 = c.upper_bounds[0]
	y0 = c.lower_bounds[1]
	y1 = c.upper_bounds[1]
	for i in range(x0,x1+1):
		for j in range(y0,y1+1):
			image[i-x0,j-y0,0] = d[i-x0,j-y0]

image[:,:,1] = image[:,:,0]
image[:,:,2] = image[:,:,0]

plot = plt.imshow(image)
plot.write_png("result%d_%d.png" % (c.iteration,c.source))
plt.clf()
