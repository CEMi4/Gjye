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
	
	EnviroWrap(const DataStorageStack * dss = NULL, const MethodStack * ms = NULL);
	EnviroWrap(const EnviroWrap * eStack);
	~EnviroWrap();
	void clearMemoryStacks();
	
};
/// ################################ ///

#endif
