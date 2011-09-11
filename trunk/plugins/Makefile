CPP = g++
CC  = gcc

INCLUDE = -I/home/matthieu/deploy/include -I..

TARGET = persistence
LIBDIR = -L$(HOME)/deploy/lib
LIB = -lhdf5 -lz

plugin: $(TARGET).cpp
	$(CPP) -fPIC -c $(TARGET).cpp $(INCLUDE)
	$(CC) -shared -Wl,-soname,$(TARGET).so -o $(TARGET).so $(TARGET).o $(LIBDIR) $(LIB)

clean:
	rm -rf $(TARGET).o $(TARGET).so
