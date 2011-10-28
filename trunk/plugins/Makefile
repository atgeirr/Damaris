CPP = g++
CC  = gcc

INCLUDE = -I/home/mdorier/deploy/include -I..

TARGET = image

plugin: $(TARGET).cpp
	$(CPP) -fPIC -c $(TARGET).cpp $(INCLUDE)
	$(CC) -shared -Wl,-soname,$(TARGET).so -o $(TARGET).so $(TARGET).o -lpng

clean:
	rm -rf $(TARGET).o $(TARGET).so
