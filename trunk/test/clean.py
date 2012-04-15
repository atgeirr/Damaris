import damaris
i = damaris.iteration - 1
print "----------------------"
print "Cleaning iteration "+str(i)
print "----------------------"
var = damaris.open("images/julia")
for c in var.select({"iteration":i}):
	var.remove(c)

