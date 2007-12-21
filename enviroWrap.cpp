#include <sstream>
#include <stdlib.h>
#include <iostream>

#include "enviroWrap.h"

//################ ENVIRONMENT WRAPPER ################//
// class EnviroWrap 
	
	/* public */ 
	EnviroWrap::EnviroWrap(const DataStorageStack * dss, const MethodStack * ms) {
		if (dss == NULL) this-> dataStructure = *(new DataStorageStack);
		else this-> dataStructure = *dss;
		
		if (ms == NULL) this->methodStructure = *(new MethodStack);
		else this->methodStructure = *ms;
	}
	EnviroWrap::EnviroWrap(const EnviroWrap * eStack) { // clone 
		this->dataStructure = *(new DataStorageStack(eStack->dataStructure));
		this->methodStructure = *(new MethodStack(eStack->methodStructure));
	}
	EnviroWrap::~EnviroWrap() {}
	void EnviroWrap::clearMemoryStacks() {
		this-> dataStructure.clearMemory();
		this-> methodStructure.clearMemory();
	}
	
/// ################################ ///
