DEPEN_LIBCDS_INC=../libcds/include/
DEPEN_LIBCDS=../libcds/libcds.a

DEPEN_WT64=../wtkdmatrix/
DEPEN_WT64_OBJS=wavelet_kd_matrix.o symbols.o

#CC=g++
DEBUGFLAGS =-c -Wall -I$(DEPEN_LIBCDS_INC) -I$(DEPEN_WT64) -O3 -g -DDEBUG -std=c++0x

EXPERIMENTS = -DEXPERIMENTS

UNAME := $(shell uname)
LDFLAGSRT = 
ifeq ($(UNAME), Linux)
LDFLAGSRT = -lrt
endif

CFLAGS=-c -Wall -I$(DEPEN_LIBCDS_INC) -I$(DEPEN_WT64) -O3 -std=c++0x
LDFLAGS=-Wall $(DEPEN_LIBCDS) -lm $(LDFLAGSRT)

EXT=

SOURCES=tgl.cpp arraysort.cpp mywtkdmatrix.cpp
OBJECTS=$(SOURCES:.cpp=.o)

DEPENOBJS+=$(addprefix $(DEPEN_WT64), $(DEPEN_WT64_OBJS))

MAINSRC=create.cpp use.cpp benchmark.cpp exp-query.cpp benchmark2.cpp
MAINOBJ=$(MAINSRC:.cpp=.o)

EXECUTABLE=create use benchmark exp-query benchmark2

#all: CFLAGS = -O9
all: $(SOURCES) $(MAINSRC) $(MAINOBJ) $(OBJECTS) $(EXECUTABLE) 

debug: CFLAGS = $(DEBUGFLAGS)
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

use: use.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

benchmark: benchmark.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)

benchmark2: benchmark2.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)


exp-query: exp-query.o
	$(CXX) $(OBJECTS) $(DEPENOBJS) $< -o $@$(EXT) $(LDFLAGS)
