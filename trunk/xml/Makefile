XSD = xsd cxx-tree

TARGET = model.xsd
NAMING = --type-naming knr 
SUFFIX = --hxx-suffix .hpp --cxx-suffix .cpp
PROLOG = --prologue-file prologue.txt 
FTYPES = #--file-per-type
WARN   = #--disable-warning D002
VALID  = #--generate-validation

OPT = $(NAMING) $(SUFFIX) $(PROLOG) $(FTYPES) $(WARN) $(VALID)

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.c=.o)

all: xml

xml: model.xsd
	$(XSD) $(OPT) $(TARGET)

%.o: %.cpp
	g++ -o $@ -c $<

test: $(OBJ)
	g++ -o test test.cxx $^ -I. -I$(HOME)/deploy/include -L$(HOME)/deploy/lib -lxerces-c

clean:
	rm -rf *.cpp *.hpp
