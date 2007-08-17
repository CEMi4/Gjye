# makefile for Gjye++ 

gjye : gjye++.cpp gjye++.h blockWrappers.o createTokenStruct.o execTokenStruct.o miscTools.o objMethods.o tokenGroups.o varStorage.o methodStorage.o enviroWrap.o #gjye_private.res
	g++ -O3 -Wall -Wno-sign-compare -pedantic -o gjye gjye++.cpp blockWrappers.o createTokenStruct.o execTokenStruct.o miscTools.o objMethods.o tokenGroups.o varStorage.o methodStorage.o enviroWrap.o #gjye_private.res

blockWrappers.o: blockWrappers.cpp blockWrappers.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c blockWrappers.cpp

createTokenStruct.o: createTokenStruct.cpp createTokenStruct.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c createTokenStruct.cpp

execTokenStruct.o: execTokenStruct.cpp execTokenStruct.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c execTokenStruct.cpp

miscTools.o: miscTools.cpp miscTools.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c miscTools.cpp

objMethods.o: objMethods.cpp objMethods.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c objMethods.cpp

tokenGroups.o: tokenGroups.cpp tokenGroups.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c tokenGroups.cpp

varStorage.o: varStorage.cpp varStorage.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c varStorage.cpp

methodStorage.o: methodStorage.cpp methodStorage.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c methodStorage.cpp

enviroWrap.o: enviroWrap.cpp enviroWrap.h
	g++ -O3 -Wall -Wno-sign-compare -pedantic -c enviroWrap.cpp

# windows icon 
#gjye_private.res: gjye_private.rc 
#	windres -i gjye_private.rc --input-format=rc -o gjye_private.res -O coff 

clean:
	rm *.o # *.res

# END OF MAKE FILE 

