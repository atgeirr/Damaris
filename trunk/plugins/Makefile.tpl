CPP = g++
CC  = gcc

INCLUDE = XXXX

TARGET = XXXX

plugin: $(TARGET).cpp
	$(CPP) -fPIC -c $(TARGET).cpp $(INCLUDE)
	$(CC) -shared -Wl,-soname,$(TARGET).so -o $(TARGET).so $(TARGET).o

clean:
	rm -rf $(TARGET).o $(TARGET).so
