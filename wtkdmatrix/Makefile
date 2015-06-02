DEPEN_LIBCDS_INC=../libcds/include/
DEPEN_LIBCDS=../libcds/libcds.a

#CC=g++
CFLAGS=-c -Wall  -I$(DEPEN_LIBCDS_INC) -O3
LDFLAGS=-Wall $(DEPEN_LIBCDS) -lm
DEBUGCFLAGS=-c -Wall  -I$(DEPEN_LIBCDS_INC) -O0 -g -DDEBUG
SOURCES=wavelet_kd_matrix.cpp test_wt.cpp symbols.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=test_wt

all: $(SOURCES) $(EXECUTABLE)

debug: CFLAGS = $(DEBUGCFLAGS)
debug: clean all

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLE)
	rm -f *.o
