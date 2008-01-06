# makefile for Gjye++ 
# the existence of windres implicitly assumes we're compiling on windows 

SHELL = /bin/sh

OBJS = objs/blockWrappers.o objs/createTokenStruct.o objs/execTokenStruct.o objs/miscTools.o objs/objMethods.o \
	objs/operMethods.o objs/tokenGroups.o objs/varStorage.o objs/methodStorage.o objs/enviroWrap.o 
CXX = g++
CDEBUG = -g
CXXFLAGS = -O2 $(CDEBUG) -Wall -pedantic-errors -ansi

# windres test #
WRTEST = $(shell windres -h)

ifeq (rc,$(findstring rc,$(WRTEST)))
	RES = gjye_private.res
	OUT = gjye.exe
else
	RES = 
	OUT = gjye
endif
##




.PHONY: all
all: setup $(OUT)

# make sure the objs folder is there
.PHONY: setup
setup: 
ifdef RES
	if not exist objs mkdir objs
	if exist $(OUT) rm $(OUT)
else
	if test ! -d objs; then mkdir objs; fi
	if test -e $(OUT); then rm $(OUT); fi
endif

$(OUT): gjye++.cpp gjye++.h $(OBJS) $(RES)
	$(CXX) $(CXXFLAGS) -o $(OUT) gjye++.cpp $(OBJS) $(RES)

objs/blockWrappers.o: blockWrappers.cpp blockWrappers.h gjye++.h miscTools.h createTokenStruct.h execTokenStruct.h enviroWrap.h methodStorage.h varStorage.h tokenGroups.h
	$(CXX) $(CXXFLAGS) -c blockWrappers.cpp -o objs/blockWrappers.o

objs/createTokenStruct.o: createTokenStruct.cpp createTokenStruct.h gjye++.h miscTools.h enviroWrap.h varStorage.h tokenGroups.h
	$(CXX) $(CXXFLAGS) -c createTokenStruct.cpp -o objs/createTokenStruct.o

objs/execTokenStruct.o: execTokenStruct.cpp execTokenStruct.h methodStorage.h gjye++.h miscTools.h blockWrappers.h enviroWrap.h varStorage.h tokenGroups.h objMethods.h operMethods.h
	$(CXX) $(CXXFLAGS) -c execTokenStruct.cpp -o objs/execTokenStruct.o

objs/miscTools.o: miscTools.cpp miscTools.h gjye++.h varStorage.h
	$(CXX) $(CXXFLAGS) -c miscTools.cpp -o objs/miscTools.o

objs/objMethods.o: objMethods.cpp objMethods.h miscTools.h varStorage.h
	$(CXX) $(CXXFLAGS) -c objMethods.cpp -o objs/objMethods.o

objs/operMethods.o: operMethods.cpp operMethods.h miscTools.h varStorage.h objMethods.h
	$(CXX) $(CXXFLAGS) -c operMethods.cpp -o objs/operMethods.o

objs/tokenGroups.o: tokenGroups.cpp tokenGroups.h miscTools.h gjye++.h
	$(CXX) $(CXXFLAGS) -c tokenGroups.cpp -o objs/tokenGroups.o

objs/varStorage.o: varStorage.cpp varStorage.h miscTools.h gjye++.h 
	$(CXX) $(CXXFLAGS) -c varStorage.cpp -o objs/varStorage.o

objs/methodStorage.o: methodStorage.cpp methodStorage.h execTokenStruct.h blockWrappers.h 
	$(CXX) $(CXXFLAGS) -c methodStorage.cpp -o objs/methodStorage.o

objs/enviroWrap.o: enviroWrap.cpp enviroWrap.h methodStorage.h varStorage.h
	$(CXX) $(CXXFLAGS) -c enviroWrap.cpp -o objs/enviroWrap.o

# windows icon 
ifdef RES
$(RES): gjye_private.rc 
	windres -i gjye_private.rc --input-format=rc -o $(RES) -O coff 
endif




.PHONY: clean
clean:
	-rm $(OBJS) $(RES) $(OUT)


# END OF MAKE FILE 
