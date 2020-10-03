CC=g++
CFLAGS=-w -O2 -c -std=c++11
LDFLAGS=
SOURCES=analyze_wer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXE=analyze_wer

all: $(SOURCES) $(EXE)

build:
	make all

test:
	./$(EXE) samples//ref samples//hyp

clean:
	rm $(OBJECTS)

$(EXE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
