DEPEN_LIBCDS_INC=../libcds/include/
DEPEN_LIBCDS=../libcds/libcds.a

#CC=g++

DEBUG_CFLAGS = -c -Wall -I$(DEPEN_LIBCDS_INC) -O0 -I../wtmatrix/ -std=c++0x  -g -DDEBUG 

EXPERIMENTS = -DEXPERIMENTS

UNAME := $(shell uname)
LDFLAGSRT = 
ifeq ($(UNAME), Linux)
LDFLAGSRT = -lrt
endif

CFLAGS=-c -Wall -I$(DEPEN_LIBCDS_INC) -O3 -I../wtmatrix/ -std=c++0x
LDFLAGS=-Wall $(DEPEN_LIBCDS) -lm ../wtmatrix/wavelet_matrix.o $(LDFLAGSRT)

EXT=

SOURCES=tgs.cpp arraysort.cpp mywtmatrix.cpp
OBJECTS=$(SOURCES:.cpp=.o)

MAINSRC=use.cpp create.cpp benchmark.cpp getsize.cpp exp-query.cpp
MAINOBJ=$(MAINSRC:.cpp=.o)

EXECUTABLE=create use benchmark getsize exp-query

#all: CFLAGS += -O9
all: $(SOURCES) $(MAINSRC) $(MAINOBJ) $(OBJECTS) $(EXECUTABLE) 

debug: CFLAGS = $(DEBUG_CFLAGS)
debug: clean all

experiments: CFLAGS += $(EXPERIMENTS)
experiments: clean all

#$(EXECUTABLE): $(MAINOBJ) $(OBJECTS)
#	$(CC) $(LDFLAGS) $(OBJECTS) $(DEPENOBJS) -o $@

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean: cleanobj cleanexe

cleanobj:
	rm -f $(OBJECTS) $(MAINOBJ)

cleanexe:
	rm -f $(EXECUTABLE)

create: create.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

use: use.o tgs.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

benchmark: benchmark.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

getsize: getsize.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

exp-query: exp-query.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)