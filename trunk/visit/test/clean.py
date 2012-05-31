import damaris
i = damaris.iteration
print "----------------------"
print "Cleaning variable"
print "----------------------"
var = damaris.open("life/cells")
chunks = var.select({'iteration':i})
print chunks
#for c in chunks:
#	var.remove(c)
