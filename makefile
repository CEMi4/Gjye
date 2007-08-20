# makefile for Gjye++ 
# the existence of windres implicitly assumes we're compiling on windows 


OBJS = blockWrappers.o createTokenStruct.o execTokenStruct.o miscTools.o objMethods.o tokenGroups.o varStorage.o methodStorage.o enviroWrap.o
CXX = g++ -Os
CDEBUG = -g
CXXFLAGS = $(CDEBUG) -Wall -Wno-sign-compare -pedantic-errors


# windres test #
WRTEST = $(shell windres --version)

ifeq (,$(findstring not, $(WRTEST)))
	RES = gjye_private.res
	OUT = gjye.exe
else
	RES = 
	OUT = gjye
endif
##


.PHONY: all
all: $(OUT)

$(OUT): gjye++.cpp gjye++.h $(OBJS) $(RES)
	-rm $(OUT)
	$(CXX) $(CXXFLAGS) -o $(OUT) gjye++.cpp $(OBJS) $(RES)

blockWrappers.o: blockWrappers.cpp blockWrappers.h
	$(CXX) $(CXXFLAGS) -c blockWrappers.cpp

createTokenStruct.o: createTokenStruct.cpp createTokenStruct.h
	$(CXX) $(CXXFLAGS) -c createTokenStruct.cpp

execTokenStruct.o: execTokenStruct.cpp execTokenStruct.h
	$(CXX) $(CXXFLAGS) -c execTokenStruct.cpp

miscTools.o: miscTools.cpp miscTools.h
	$(CXX) $(CXXFLAGS) -c miscTools.cpp

objMethods.o: objMethods.cpp objMethods.h
	$(CXX) $(CXXFLAGS) -c objMethods.cpp

tokenGroups.o: tokenGroups.cpp tokenGroups.h
	$(CXX) $(CXXFLAGS) -c tokenGroups.cpp

varStorage.o: varStorage.cpp varStorage.h
	$(CXX) $(CXXFLAGS) -c varStorage.cpp

methodStorage.o: methodStorage.cpp methodStorage.h
	$(CXX) $(CXXFLAGS) -c methodStorage.cpp

enviroWrap.o: enviroWrap.cpp enviroWrap.h
	$(CXX) $(CXXFLAGS) -c enviroWrap.cpp


# windows icon 
ifdef RES
$(RES): gjye_private.rc 
	windres -i gjye_private.rc --input-format=rc -o $(RES) -O coff 
endif


.PHONY: clean
clean:
	-rm $(OBJS) $(RES) $(OUT)



# END OF MAKE FILE 
