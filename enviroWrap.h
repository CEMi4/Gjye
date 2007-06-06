#ifndef ENVIROWRAP_H
#define ENVIROWRAP_H

#include "methodStorage.h"
#include "varStorage.h"


//################ PROTOTYPES ################//
class EnviroWrap {
protected: 
	
public: 
	DataStorageStack dataStructure; // declare (locally) global variable storage object
	MethodStack methodStructure; 
	
	EnviroWrap(DataStorageStack * dss = NULL, MethodStack * ms = NULL);
	EnviroWrap(EnviroWrap * eStack);
	~EnviroWrap();
	void clearMemoryStacks();
	
};
/// ################################ ///

#endif
