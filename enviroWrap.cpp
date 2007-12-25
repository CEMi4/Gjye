#include <sstream>
#include <stdlib.h>
#include <iostream>

#include "enviroWrap.h"

//################ ENVIRONMENT WRAPPER ################//
// class EnviroWrap 
	
	/* public */ 
	EnviroWrap::EnviroWrap(const DataStorageStack * dss, const MethodStack * ms) {
		if (dss == NULL) {
			DataStorageStack * tmp = new DataStorageStack;
			this-> dataStructure = *tmp;
			delete tmp;
		}
		else this-> dataStructure = *dss;
		
		if (ms == NULL) {
			MethodStack * tmp = new MethodStack;
			this->methodStructure = *tmp;
			delete tmp;
		}
		else this->methodStructure = *ms;
	}
	EnviroWrap::EnviroWrap(const EnviroWrap * eStack) { // clone 
		this->dataStructure = eStack->dataStructure;
		this->methodStructure = eStack->methodStructure;
	}
	EnviroWrap::~EnviroWrap() {}
	void EnviroWrap::clearMemoryStacks() {
		this-> dataStructure.clearMemory();
		this-> methodStructure.clearMemory();
	}
	
/// ################################ ///
