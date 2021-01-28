CC=g++
CFLAGS=-w -O2 -c -std=c++11
LDFLAGS=
SOURCES=compute-wer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXE=compute-wer

all: $(SOURCES) $(EXE)

build:
	make all
	./$(EXE) samples//ref samples//hyp

test:
	./$(EXE) samples//ref samples//hyp

clean:
	rm $(OBJECTS)

$(EXE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
