#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "miscTools.h"
#include "varStorage.h"
#include "gjye++.h"


//################ GJYE VARIABLE SORTING ALGORITHM ################//
	bool strCmp::operator() (std::string s1, std::string s2) const {
		if (tools::isInteger(s1) && tools::isInteger(s2)) {return tools::stringToInt(s1) < tools::stringToInt(s2);}
		return s1.compare(s2) < 0;
	}
/// ################################ ///





//################ GJYE VARIABLE STORAGE ################//
// class VariableStorage 
	
	/* protected */
	void VariableStorage::refresh(bool fullRefresh) { // reset the auto index  ... we have to do a O(n) check for tainted every time we remove! phreak! (implement unrolled) 
		if (this->isScalar.size() > 0) {
			std::map<std::string, bool>::reverse_iterator itr = this->isScalar.rbegin();
			if (fullRefresh == false && this->arrayAutoIndex >= 0 && tools::isInteger(itr->first))  // only refresh autoindex (assume it's still an array) -- for inserts only 
				this->arrayAutoIndex = (int) tools::stringToInt(itr->first) + 1;
			else { // for removals (we have to do a full check) 
				if (tools::isInteger(itr->first)) {
					this->arrayAutoIndex = (int) tools::stringToInt(itr->first) + 1;
					for (; itr != this->isScalar.rend(); ++itr) 
						if (tools::isInteger(itr->first) == false) this->arrayAutoIndex = -1;
				} else this->arrayAutoIndex = -1;
			}
		} else {this->arrayAutoIndex = 0;}
	}
	
	
	
	/* public */ 
	VariableStorage::VariableStorage(int aaIdx) {this->startVariableReference = 0;this->arrayAutoIndex = aaIdx;}
	VariableStorage::~VariableStorage() {} // variableNames.~vector<string>();variableData.~vector<string>();
	
	
	std::string VariableStorage::variableReferencer(std::string header) const { // create a unique (generic) variable name
		std::ostringstream outs;
		char varRef[16];
		sprintf(varRef,"%010i",this->startVariableReference);
		outs << varRef;
		std::string body = outs.str();
		header.append(body);
		++(this->startVariableReference);
		return header;
	}
	
	
	bool VariableStorage::addVector(std::string thisName, VariableStorage vector, int insPos) { // use getVector first to jump to the level!   Dumps an entire vector into this namespace 
		if (this->arrayAutoIndex >= 0 && (thisName == "" || tools::isInteger(thisName))) { // arrays 
			if (thisName == "" && insPos < 0) {
				thisName = tools::intToString(this->arrayAutoIndex);
			}
			else if (thisName == "") { // insert at insPos (& shift all elements over one ... overwriting if necessary) 
				std::map<std::string, bool>::reverse_iterator iter;
				for (iter = this->isScalar.rbegin(); iter != this->isScalar.rend(); ++iter) {
					std::string thisIndex = iter->first, nextIndex = tools::intToString(tools::stringToInt(thisIndex) + 1);
					if (this->isScalar[thisIndex] == true) {this->variableNames[nextIndex] = this->variableNames[thisIndex];}
					else {this->vectorNames[nextIndex] = this->vectorNames[thisIndex];}
					this->isScalar[nextIndex] = this->isScalar[thisIndex];
					if (thisIndex == tools::intToString(insPos)) {break;}
				}
				thisName = tools::intToString(insPos);
			} // otherwise insert as thisName (overwriting if necessary) 
			
			this->vectorNames[thisName] = vector;
			this->isScalar[thisName] = false;
			this->refresh();
			return true;
		}
		else if (thisName != "") { // vector/tree
			this->vectorNames[thisName] = vector;
			this->arrayAutoIndex = -1; // taint array
			this->isScalar[thisName] = false;
			return true;
		}
		else {std::cout << "WARNING :: Mishandling: Cannot push onto a tainted array!" <<std::endl;}
		return false;
	}
	
	
	bool VariableStorage::addVariable(std::string thisName, std::string thisData, int insPos) { // use getVector first to jump to the level! 
		if (this->arrayAutoIndex >= 0 && (thisName == "" || tools::isInteger(thisName))) { // arrays 
			if (thisName == "" && insPos < 0) {
				thisName = tools::intToString(this->arrayAutoIndex);
			}
			else if (thisName == "") { // insert at insPos (& shift all elements over one ... overwriting if necessary) 
				std::map<std::string, bool>::reverse_iterator iter;
				for (iter = this->isScalar.rbegin(); iter != this->isScalar.rend(); ++iter) {
					std::string thisIndex = iter->first, nextIndex = tools::intToString(tools::stringToInt(thisIndex) + 1);
					if (this->isScalar[thisIndex] == true) {this->variableNames[nextIndex] = this->variableNames[thisIndex];}
					else {this->vectorNames[nextIndex] = this->vectorNames[thisIndex];}
					this->isScalar[nextIndex] = this->isScalar[thisIndex];
					if (thisIndex == tools::intToString(insPos)) {break;}
				}
				thisName = tools::intToString(insPos);
			} // otherwise insert as thisName (overwriting if necessary) 
			
			this->variableNames[thisName] = thisData;
			this->isScalar[thisName] = true;
			this->refresh();
			return true;
		} // push onto an array
		else if (thisName != "") { // push a scalar or on a vector
			this->variableNames[thisName] = thisData;
			this->isScalar[thisName] = true;
			this->arrayAutoIndex = -1; // taint array
			return true;
		}
		else {std::cout << "WARNING :: Mishandling: Cannot push onto a tainted array!" <<std::endl;}
		return false;
	}
	
	
	VariableStorage * VariableStorage::getVector(std::string thisName) { // give me an object if i give you a base 
		if (this->variableExists(thisName) == false) {std::cout << "ERROR :: DNE: getVector("  << thisName << ")!" <<std::endl;exit(1);} // otherwise die 
		
		if (this->isScalar.find(thisName)->second == false) {
			return &this->vectorNames.find(thisName)->second;
		}
		else {std::cout << "WARNING:: Scalar: getVector("  << thisName << ") is scalar!" <<std::endl;return NULL;} // we can't grab a scalar foo! 
	}
	
	
	VariableStorage * VariableStorage::vecStringToVector(std::string * vecName, bool checkBaseExistence, bool returnNullOnNonExistance) { // convert a vector STRING (eg. VEC[1][2])  into the baseline object (and vecName becomes the highest level -- eg. 2).  uses getVector
		VariableStorage * returnVector = this;
		if (*vecName == "") return returnVector;
		
		int lIndex = vecName->find_first_not_of(validKeyChars);
		if (lIndex == std::string::npos) {lIndex = vecName->length()-1;}
		
		if (vecName->at(lIndex) == '[') { // it's not a simple vector (%vecName) 
			if (returnNullOnNonExistance && returnVector->variableExists(vecName->substr(0, lIndex)) == false) return NULL; // return NULL instead of dying from a getVector call
			returnVector = returnVector->getVector(vecName->substr(0, lIndex)); // jump! 
			vecName->erase(0, lIndex);
			
			int oBrac = 0, nIndex = 0; // number brackets open, current index 
			while (nIndex < vecName->length() && (oBrac > 0 || vecName->at(nIndex) == '[')) {
				if (vecName->at(nIndex) == '[') {++oBrac;++nIndex;}
				else if (vecName->at(nIndex) == ']') {
					--oBrac;
					if (oBrac == 0) {
						if (nIndex == vecName->length()-1) { // this is the highest level (stop, change vecName and don't jump!) 
							*vecName = vecName->substr(1, nIndex-1);
							break;
						} else {
							if (returnNullOnNonExistance && returnVector->variableExists(vecName->substr(1, nIndex-1)) == false) return NULL;
							returnVector = returnVector->getVector(tools::prepareVectorData(new DataStorageStack(this), vecName->substr(1, nIndex-1))); // jump! 
							vecName->erase(0, nIndex+1);
							nIndex = 0;
						}
					} else {++nIndex;}
				} else {++nIndex;}
			}
			
		} else { // if it's a simple vector, make sure it exists! 
			if (checkBaseExistence && this->variableExists(*vecName) == false) {std::cout << "ERROR :: DNE: vecStringToVector("  << *vecName << ")!" <<std::endl;exit(1);} // otherwise die 
			else if (returnNullOnNonExistance && returnVector->variableExists(*vecName) == false) return NULL;
		}
		
		return returnVector;
	}
	
	
	bool VariableStorage::removeVariable(std::string thisName) { // use getVector first to jump to the level! 
		if (this->variableExists(thisName) == false) {return false;} // we can't delete what doesn't exist! 
		else {
			if (this->isScalar[thisName] == true) {this->variableNames.erase(thisName);}
			else { // you also have to bump back all the autoindices for arrays (do we? is it null now?) 
				this->vectorNames.erase(thisName);
			}
			this->isScalar.erase(thisName);
			this->refresh(true); // check to see if it's still tainted, and refresh the autoindex 
			return true;
		}
	}
	
	
	std::string VariableStorage::getData(std::string thisName) { // use getVector first to jump to the level! 
		if (this->variableExists(thisName) == true) {
			if (this->isScalar.find(thisName)->second == true) {return this->variableNames.find(thisName)->second;} // variable 
			else {return tools::intToString(this->vectorNames.find(thisName)->second.isScalar.size());} // vector - return vector size 
		}
		else {std::cout << "ERROR :: DNE: getData("  << thisName << ")!" <<std::endl;exit(1);} // DNE 
	}
	
	
	std::map<std::string, bool, strCmp> * VariableStorage::getVectorNodes() { // use getVector first to  jump to the level! (gives them a pointer to isScalar (so they can find var names within) 
		return &this->isScalar;
	}
	
	
	std::string VariableStorage::dumpData() { // for developmental purposes 
		std::string buffer = "";
		std::map<std::string, bool>::const_iterator iter;
		for (iter = this->isScalar.begin(); iter != this->isScalar.end(); ++iter) {
			std::string arrayPrint = "";if (this->vectorNames[iter->first].arrayAutoIndex >= 0) {arrayPrint = "*";}
			if (this->isScalar[iter->first] == true) {buffer += "" + iter->first + "=" + this->variableNames[iter->first] + ",";}
			else {buffer += "" + iter->first + arrayPrint + " :: [" + this->vectorNames[iter->first].dumpData() + "],";}
		}
		return buffer;
	}
	
	
	int VariableStorage::size() const { // only makes sense for vector nodes 
		return this->isScalar.size();
	}
	
	
	int VariableStorage::type(std::string thisName) {
		if (thisName == "") {return (this->arrayAutoIndex >= 0 ? 1 : 2);} // *this can't be a scalar! 
		
		if (this->variableExists(thisName)) {
			if (this->isScalar.find(thisName)->second == false && this->vectorNames.find(thisName)->second.arrayAutoIndex >= 0) {return 1;} // array
			else if (this->isScalar.find(thisName)->second == false) {return 2;} // vector
			else {return 0;} // scalar
		}
		else {return -1;} // DNE 
	}
	
	
	std::string VariableStorage::typeString(std::string thisName) {
		int retType = this->type(thisName);
		if (retType > 0) return "%";
		else if (retType == 0) return "$";
		else return "-1";
	}
	
	
	bool VariableStorage::variableExists(std::string thisName) {
		if (thisName == "") return false;
		
		if (this->isScalar.count(thisName) == 1) {return true;}
		return false;
	}
/// ################################ ///





//################ GJYE VARIABLE STACK ################//
// class DataStorageStack
	
	/* public */
	DataStorageStack::DataStorageStack() {}
	DataStorageStack::DataStorageStack(VariableStorage * vStore) {
		this->pushStorage(vStore);
	}
	DataStorageStack::DataStorageStack(DataStorageStack * dStack) { // clone 
		this->dataStack = dStack->dataStack;
	}
	DataStorageStack::~DataStorageStack() {}
	void DataStorageStack::clearMemory() {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		for (; iter != this->dataStack.rend(); ++iter) 
			delete *iter;
		this->dataStack.clear();
	}
	void DataStorageStack::pop() {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		delete *iter; // destroy 
		this->dataStack.pop_back(); // remove  
	}
	
	std::string DataStorageStack::variableReferencer(std::string header) {
		return (*this->dataStack.rbegin())->variableReferencer(header); // top of the stack 
	}
	
	
	bool DataStorageStack::addVector(std::string thisName, VariableStorage vector, int insPos, bool instantiate) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		if (instantiate == true) {return (*iter)->addVector(thisName, vector, insPos);} // if we're instantiating, then we want the top of the stack 
		
		for (; iter != this->dataStack.rend(); ++iter) { // otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return (*iter)->addVector(thisName, vector, insPos);}
		}
		
		return (*this->dataStack.rbegin())->addVector(thisName, vector, insPos); // if all else fails, go with the top of the stack 
	}
	
	bool DataStorageStack::addVariable(std::string thisName, std::string thisData, int insPos, bool instantiate) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		if (instantiate == true) {return (*iter)->addVariable(thisName, thisData, insPos);std::cout << "instant" <<std::endl;} // if we're instantiating, then we want the top of the stack 
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return (*iter)->addVariable(thisName, thisData, insPos);}
		}
		
		return (*this->dataStack.rbegin())->addVariable(thisName, thisData, insPos); // if all else fails, go with the top of the stack 
	}
	
	
	
	VariableStorage * DataStorageStack::getVector(std::string thisName) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return (*iter)->getVector(thisName);}
		}
		
		return (*this->dataStack.rbegin())->getVector(thisName); // if all else fails, go with the top of the stack (which fails for us!) 
	}
	
	
	VariableStorage * DataStorageStack::vecStringToVector(std::string * thisName, bool checkBaseExistence, bool returnNullOnNonExistance) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		int lIndex = thisName->find_first_not_of(validKeyChars);
		if (lIndex == std::string::npos) {lIndex = thisName->length()-1;}
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists( thisName->substr(0, lIndex) )) {return (*iter)->vecStringToVector(thisName, checkBaseExistence, returnNullOnNonExistance);}
		}
		
		return (*this->dataStack.rbegin())->vecStringToVector(thisName, checkBaseExistence, returnNullOnNonExistance); // if all else fails, go with the top of the stack (which fails for us!) 
	}
	
	
	
	bool DataStorageStack::removeVariable(std::string thisName) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return (*iter)->removeVariable(thisName);}
		}
		
		return (*this->dataStack.rbegin())->removeVariable(thisName); // if all else fails, go with the top of the stack (which fails for us!) 
	}
	
	
	std::string DataStorageStack::getData(std::string thisName) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return (*iter)->getData(thisName);}
		}
		
		return (*this->dataStack.rbegin())->getData(thisName); // if all else fails, go with the top of the stack (which fails for us!) 
	}
	
	
	std::map<std::string, bool, strCmp> * DataStorageStack::getVectorNodes() {
		return (*this->dataStack.rbegin())->getVectorNodes(); // top of the stack 
	}
	
	
	std::string DataStorageStack::dumpData() {
		return (*this->dataStack.rbegin())->dumpData(); // dump the top of the stack only 
	}
	
	
	int DataStorageStack::size() {
		return (*this->dataStack.rbegin())->size(); // top of the stack 
	}
	
	
	int DataStorageStack::type(std::string thisName) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return (*iter)->type(thisName);}
		}
		
		return -1; // it doesn't exist! 
	}
	
	
	std::string DataStorageStack::typeString(std::string thisName) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return (*iter)->typeString(thisName);}
		}
		
		return "-1"; // it doesn't exist! 
	}
	
	
	bool DataStorageStack::variableExists(std::string thisName) {
		std::vector<VariableStorage *>::reverse_iterator iter = this->dataStack.rbegin();
		
		for (; iter != this->dataStack.rend(); ++iter) {	// otherwise find the first (highest) declaration of the variable 
			if ((*iter)->variableExists(thisName)) {return true;}
		}
		
		return false; // it doesn't exist! 
	}
	
	
	void DataStorageStack::pushStorage(VariableStorage * vStore) { // add a new VStore to *this 
		if (vStore == NULL) this->dataStack.push_back(new VariableStorage(-1));
		else this->dataStack.push_back(vStore);
	}
	
/// ################################ ///





//################ GJYE TRANSIENT CONSTANT STORAGE ################//
// 
// for strings and temporary vectors; inherited from ConstantStorage 
// 
/// ################################ ///
