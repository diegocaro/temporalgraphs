include ../Make.helper
CXX_FLAGS=$(MY_CXX_FLAGS) $(MY_CXX_OPT_FLAGS) \
-I$(INC_DIR) \
-I$(LIB_DIR)/../cdswrapper/include/ \
-I$(LIB_DIR)/../external/libcds/include/ \
-I$(LIB_DIR)/../external/sdsl-lite/include/ \
-I$(LIB_DIR)/../external/sdsl-lite/include/ \
$(LIB_DIR)/libcqtree.a \
$(LIB_DIR)/ext_cdswrapper/ext_sdsl-lite/lib/libsdsl.a \
$(LIB_DIR)/ext_cdswrapper/ext_libcds/libcds.a \
$(LIB_DIR)/ext_cdswrapper/libcdswrapper.a 

CCLIB= 
SOURCES=$(wildcard *.cc)
EXECS=$(SOURCES:.cc=.x)

all: $(EXECS)

build-test: $(EXECS)
	        
%.x:%.cc
	$(MY_CXX) $(CXX_FLAGS) -o $@ $< $(CCLIB) 

clean:
	rm -f $(EXECS)
	rm -rf *.dSYM
