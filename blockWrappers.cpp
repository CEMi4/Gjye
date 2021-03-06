#include <sstream>
#include <stdlib.h>
#include <iostream>

#include "blockWrappers.h"
#include "gjye++.h"
#include "miscTools.h"
#include "createTokenStruct.h"
#include "execTokenStruct.h"


//################ BLOCK WRAPPER ################//
// class BlockWrap 

	/* public */ 
	BlockWrap::BlockWrap(std::string code, const EnviroWrap * dStack) {
		this->codeInput = code;
		
		if (dStack == NULL) {this->environment = new EnviroWrap();}
		else {
			this->environment = new EnviroWrap(dStack);
		}
		
		this->environment->dataStructure.pushStorage(); // new blocks get new top level DS's
		this->environment->methodStructure.pushStorage(); // new blocks get new TLM stacks 
		
		this->sovtGroup = new TokenGroup;
		create::blockHandler(this->sovtGroup, &this->codeInput);
		
		int start = 0;
		while (this->codeInput.find("�", start) != std::string::npos) {
			start = this->codeInput.find("�", start);
			this->codeInput.replace(start, 1, "�.");
			++start;
		}
		
		create::prepareTokenInput(this->environment, this->sovtGroup, &this->codeInput);
	}
	BlockWrap::~BlockWrap() {
		if (this->environment->dataStructure.stackSize() > 0) {this->environment->dataStructure.pop();}
		if (this->environment->methodStructure.stackSize() > 0) {this->environment->methodStructure.pop();}
		if (this->environment != NULL) delete this->environment;
		if (this->sovtGroup != NULL) delete this->sovtGroup;
	}
	void BlockWrap::importDataStack(EnviroWrap * storage) {
		this->environment = storage;
	}
	void BlockWrap::clearMemoryStack() {
		this->environment->dataStructure.clearMemory();
	}
	
	
	
	std::string BlockWrap::executeCode() {
		std::string fullUserInput = this->codeInput, 
			singleInput = "", returnValue="";
		TokenGroup * tGroup = new TokenGroup(this->sovtGroup);
		
		unsigned int eIndex;
		while (fullUserInput != "") { // exec each command given 
			eIndex = std::string::npos;
			do {
				eIndex = fullUserInput.find('.', eIndex+1);
			} while (eIndex != std::string::npos && eIndex+1 < fullUserInput.length() && isdigit(fullUserInput.at(eIndex+1)) > 0 && fullUserInput.at(eIndex+1) != '�');
			if (eIndex == std::string::npos) {eIndex = fullUserInput.length()-1;}
			
			unsigned int firstChar = 0;
			while ( firstChar < fullUserInput.length() && fullUserInput.at(firstChar) == ' ')
				++firstChar;
			
			singleInput = fullUserInput.substr(firstChar,eIndex+1-firstChar); // including the period if there was one 
			fullUserInput.replace(0,eIndex+1,"");
			
			if (singleInput.length() == 0) 
				continue;
			
			if (singleInput.at(singleInput.length()-1) == '.') {
				singleInput = singleInput.substr(0,singleInput.length()-1); // get rid of the period 
			}
			
			if (singleInput != "") {
				create::createTokenStruct(singleInput, tGroup);
				if (SHOW_DEBUGGING) {std::cout << "\n\ncatalyst -> " << tGroup->catalyst <<std::endl;}
				
				returnValue = exec::runTokenStruct(this->environment, tGroup);
				
				if (SHOW_DEBUGGING) {
					std::cout << "\n\n=======================================\n\n" 
						+ this->environment->dataStructure.dumpData() 
						<< "\n\n==============================================================================\n\n" <<std::endl;
				}
				
				singleInput = "";
			}
		}
		
		returnValue = tools::prepareVectorData( &this->environment->dataStructure, returnValue );
		
		if (tGroup != NULL) delete tGroup;
		
		return returnValue;
	}
	
/// ################################ ///





//################ CLASS WRAPPER ################//
// class ClassWrap : public MethodWrap 
	
	/* public */
	ClassWrap::ClassWrap(std::string className, std::string code, const EnviroWrap * dStack) : BlockWrap(code, dStack) {
		this->methods = new MethodStack();
		this->methods->pushStorage();
		if (className == "") 
			this->classType = "Object"; // general 
		else this->classType = className;
	}
	ClassWrap::~ClassWrap() {
		delete this->methods;
	}
	void ClassWrap::clearMemoryStack() {
		this->methods->clearMemory();
	}
	
/// ################################ ///
