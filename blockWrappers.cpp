#include <sstream>
#include <stdlib.h>
#include <iostream>

#include "blockWrappers.h"
#include "gjye++.h"
#include "miscTools.h"
#include "createTokenStruct.h"
#include "execTokenStruct.h"
#include "objMethods.h"


//################ BLOCK WRAPPER ################//
// class BlockWrap 

	/* public */ 
	BlockWrap::BlockWrap(std::string code, const EnviroWrap * dStack) {
		this->codeInput = code;
		
		if (dStack == NULL) {this->environment = new EnviroWrap();}
		else {this->environment = new EnviroWrap(dStack);}
		
		this->environment->dataStructure.pushStorage(); // new blocks get new top level DS's
		this->environment->methodStructure.pushStorage(); // new blocks get new TLM stacks 
	}
	BlockWrap::~BlockWrap() {
		if (this->environment->dataStructure.stackSize() > 0) {this->environment->dataStructure.pop();}
		if (this->environment->methodStructure.stackSize() > 0) {this->environment->methodStructure.pop();}
		delete environment;
	}
	void BlockWrap::importDataStack(EnviroWrap * storage) {
		this->environment = storage;
	}
	void BlockWrap::clearMemoryStack() {
		this->environment->dataStructure.clearMemory();
	}
	
	void BlockWrap::tokenizeBlocks(TokenGroup * tGroup) { // interface 
		create::blockHandler(tGroup, &this->codeInput);
	}
	
	
	std::string BlockWrap::executeCode() {
		TokenGroup * sovtGroup = new TokenGroup, 
			* tGroup = NULL;
		
		this->tokenizeBlocks(sovtGroup); // tokenize blocks (now so we don't have token overlap in blocks later) 
		
		create::prepareTokenInput(this->environment, sovtGroup, &this->codeInput);
		
		std::string fullUserInput = this->codeInput, 
			singleInput = "";
		
		int eIndex;
		while (fullUserInput != "") { // exec each command given 
			eIndex = -1;
			do {
				eIndex = fullUserInput.find('.', eIndex+1);
			} while (eIndex > -1 && eIndex+1 < fullUserInput.length() && isdigit(fullUserInput.at(eIndex+1)) > 0 && fullUserInput.at(eIndex+1) != '«');
			if (eIndex == std::string::npos) {eIndex = fullUserInput.length()-1;}
			
			singleInput = fullUserInput.substr(0,eIndex+1); // including the period if there was one 
			fullUserInput.replace(0,eIndex+1,"");
			
			if (singleInput.at(singleInput.length()-1) == '.') {
				singleInput = singleInput.substr(0,singleInput.length()-1); // get rid of the period 
			}
			
			if (singleInput != "") {
				tGroup = new TokenGroup(sovtGroup); // (shallow) copy 
				create::createTokenStruct(singleInput,tGroup);
				if (SHOW_DEBUGGING) {std::cout << "\n\ncatalyst -> " << tGroup->catalyst <<std::endl;}
				
				exec::runTokenStruct(this->environment, tGroup);
				
				if (SHOW_DEBUGGING) {
					std::cout << "\n\n=======================================\n\n" 
						+ this->environment->dataStructure.dumpData() 
						<< "\n\n==============================================================================\n\n" <<std::endl;
				}
				
				singleInput = "";
				
				// blindly update If states 
				sovtGroup->openIfBlock = tGroup->openIfBlock;
				sovtGroup->insideIfBlock = tGroup->insideIfBlock;
				
				if (tGroup != NULL) {delete tGroup;}
			}
		}
		
		delete sovtGroup;
		
		return "";
	}
	
/// ################################ ///





//################ METHOD WRAPPER ################//
// class MethodWrap : public BlockWrap 
	
	/* public */
	MethodWrap::MethodWrap(std::string code, const EnviroWrap * dStack) : BlockWrap(code, dStack) {
		//this->isSTDL = false;
		//dataStructure = new DataStorageStack(-1);
	}
	
/// ################################ ///





//################ CLASS WRAPPER ################//
// class ClassWrap : public MethodWrap 
	
	/* public */
	ClassWrap::ClassWrap(std::string code, const EnviroWrap * dStack) : MethodWrap(code, dStack) {
		this->methods = new MethodStack();
		this->methods->pushStorage();
	}
	ClassWrap::~ClassWrap() {
		delete this->methods;
	}
	void ClassWrap::clearMemoryStack() {
		this->methods->clearMemory();
	}
	
/// ################################ ///
