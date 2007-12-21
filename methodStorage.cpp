#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "methodStorage.h"
#include "execTokenStruct.h"
#include "blockWrappers.h"


//################ GJYE METHOD DB INSTANCE ################//
//class MethodDefinition 
	
	/* public */ 
	MethodDefinition::MethodDefinition(std::string rType, std::string code, std::vector< std::map<std::string, std::string> > comps, std::vector<std::string> pType, std::vector<bool> isOpt, bool isSTDL, bool ipff) {
		this->paramType = pType;
		this->vocabulary  = comps;
		this->code = code;
		this->isOptional = isOpt;
		this->isSTDL = isSTDL;
		this->returnType = rType;
		this->isPostFixFunc = ipff;
	}
	MethodDefinition::~MethodDefinition() {
		//delete obj; // do we really want to do this? 
	}
	
	
	void MethodDefinition::toString() const {
		if (this->isPostFixFunc == true) std::cout << "* ";
		
		for (int i = 0; i < this->vocabulary.size() && vocabulary.at(i).size() > 0; ++i) 
			std::cout << this->vocabulary.at(i).begin()->first << "(" << this->paramType.at(i) << (this->isOptional.at(i) ? "=" : "") << ")  ";
		std::cout << "returns " << this->returnType << std::endl;
	}
	
	
	int MethodDefinition::calculateMatch(std::vector<std::string> vocab, std::vector<std::string> types) const {
		int numberSkipped = 0, vocabLoc = 0, i;
		
		for (i = 0; i < vocab.size() && vocabLoc < this->vocabulary.size(); ++vocabLoc) {
			if (this->vocabulary[vocabLoc].find(vocab[i]) == this->vocabulary[vocabLoc].end() 
				|| (
					this->isOptional[vocabLoc] == false 
					|| (this->isOptional[vocabLoc] == true && types.at(i) != "")
				) 
				&& this->paramType.at(vocabLoc) != types.at(i)) { // not matched 
					if (this->isOptional[vocabLoc] == true) ++numberSkipped;
					else return -1; // NOT A CANDIDATE 
			} else ++i;
		}
		if (i != vocab.size()) return -1; // NOT A CANDIDATE 
		
		return numberSkipped;
	}
	
/// ### ///





//################ GJYE METHOD DB STORAGE ################//
// class MethodStorage 
	
	/* public */
	MethodStorage::MethodStorage() {}
	MethodStorage::~MethodStorage() {}
	void MethodStorage::clearMemory() {
		std::multimap<std::map<std::string, std::string>, MethodDefinition *>::iterator iter = this->methods.begin();
		for (; iter != this->methods.end(); ++iter) 
			delete iter->second;
	}
	
	
	bool MethodStorage::methodExists(std::string mName) const {
		std::multimap<std::map<std::string, std::string>, MethodDefinition *>::const_iterator oIter = this->methods.begin();
		
		for (; oIter != this->methods.end(); ++oIter) {
			if ((oIter->first).count(mName) > 0) return true;
		}
		return false;
	}
	
	
	const MethodDefinition * MethodStorage::getMethod(std::string mName) {
		std::multimap<std::map<std::string, std::string>, MethodDefinition *>::iterator oIter = this->methods.begin();
		
		for (; oIter != this->methods.end(); ++oIter) {
			if ((oIter->first).find(mName) != (oIter->first).end()) return oIter->second;
		}
		return NULL;
	}
	
	
	bool MethodStorage::isSTDL(std::string mName) const {
		std::multimap<std::map<std::string, std::string>, MethodDefinition *>::const_iterator oIter = this->methods.begin();
		
		for (; oIter != this->methods.end(); ++oIter) {
			if ((oIter->first).find(mName) != (oIter->first).end()) return (oIter->second)->isSTDL;
		}
		return false;
	}
	
	
	void MethodStorage::addMethod(std::string rType, std::string code, std::vector< std::map<std::string, std::string> > comps, std::vector<std::string> pType, std::vector<bool> isOpt, bool isSTDL, bool ipff) {
		MethodDefinition * mI = new MethodDefinition(rType, code, comps, pType, isOpt, isSTDL, ipff);
		this->methods.insert(std::make_pair(comps.at(0), mI));
	}
	
	
	void MethodStorage::toString() const {
		std::multimap<std::map<std::string, std::string>, MethodDefinition *>::const_iterator oIter = this->methods.begin();
		for (; oIter != this->methods.end(); ++oIter) {
			(oIter->second)->toString();
		}
	}
	
	
	const MethodDefinition * MethodStorage::findMatch(std::vector<std::string> vocab, std::vector<std::string> types) const {
		std::multimap<std::map<std::string, std::string>, MethodDefinition *>::const_iterator oIter = this->methods.begin();
		std::multimap<int, MethodDefinition *> numberSkipped;
		
		// determine match quality of all contained methods 
		for (; oIter != this->methods.end(); ++oIter) 
			if ((oIter->first).find(vocab.at(0)) != (oIter->first).end()) // only if the verbs match (don't bother otherwise) 
				numberSkipped.insert(std::make_pair((oIter->second)->calculateMatch(vocab, types), oIter->second));
		if (numberSkipped.size() == 0) return NULL; // no matches here 
		
		
		// find the method with the smallest mismatch 
		std::multimap<int, MethodDefinition *>::iterator tIter = numberSkipped.begin();
		int smallest = tIter->first;
		MethodDefinition * tempDef = tIter->second;
		for (; tIter != numberSkipped.end(); ++tIter) {
			if (tIter->first == -1) continue;
			if (smallest == -1 || tIter->first < smallest) {
				tempDef = tIter->second;
				smallest = tIter->first;
			}
		}
		if (smallest == -1) return NULL; // no matches here 
		
		
		// make sure there are no duplicates (this is the only one with this exact match quality) 
		tIter = numberSkipped.begin();
		int numMatches = 0;
		for (; tIter != numberSkipped.end(); ++tIter) 
			if (smallest == tIter->first) 
				++numMatches;
		if (numMatches > 1) {std::cout << "CRITERROR :: Ambiguity: Call made was ambiguous!" <<std::endl;exit(1);} // two with same weight! ambiguous call 
		
		return tempDef;
	}
	
	
	bool MethodStorage::isPostFixFunc(std::string mName) const {
		std::multimap<std::map<std::string, std::string>, MethodDefinition *>::const_iterator oIter = this->methods.begin();
		
		for (; oIter != this->methods.end(); ++oIter) {
			if ((oIter->first).find(mName) != (oIter->first).end()) return (oIter->second)->isPostFixFunc;
		}
		return false;
	}
	
/// ### ///





//################ GJYE METHOD DB STACK ################//
// class MethodStack 
	
	/* public */
	MethodStack::MethodStack() {
		this->methodStack.push_back(new MethodStorage);
	}
	MethodStack::MethodStack(const MethodStack * mStack) { // clone 
		this->methodStack = mStack->methodStack;
	}
	MethodStack::~MethodStack() {}
	void MethodStack::clearMemory() {
		std::vector<MethodStorage *>::reverse_iterator iter = this->methodStack.rbegin();
		for (; iter != this->methodStack.rend(); ++iter) 
			(*iter)->clearMemory();
	}
	void MethodStack::pop() {
		std::vector<MethodStorage *>::reverse_iterator iter = this->methodStack.rbegin();
		(*iter)->clearMemory(); // destroy 
		this->methodStack.pop_back(); // remove  
	}
	
	bool MethodStack::methodExists(std::string mName) const {
		std::vector<MethodStorage *>::const_reverse_iterator iter = this->methodStack.rbegin();
		
		for (; iter != this->methodStack.rend(); ++iter) {	// find the first (highest) declaration of the method 
			if ((*iter)->methodExists(mName)) {return true;}
		}
		
		return false;
	}
	
	
	const MethodDefinition * MethodStack::getMethod(std::string mName) {
		std::vector<MethodStorage *>::reverse_iterator iter = this->methodStack.rbegin();
		
		for (; iter != this->methodStack.rend(); ++iter) { // find the first (highest) declaration of the method 
			if ((*iter)->methodExists(mName)) {return (*iter)->getMethod(mName);}
		}
		
		return NULL;
	}
	
	
	bool MethodStack::isSTDL(std::string mName) const {
		std::vector<MethodStorage *>::const_reverse_iterator iter = this->methodStack.rbegin();
		
		for (; iter != this->methodStack.rend(); ++iter) { // find the first (highest) declaration of the method 
			if ((*iter)->methodExists(mName)) {return (*iter)->isSTDL(mName);}
		}
		
		return false;
	}
	
	
	void MethodStack::addMethod(std::string rType, std::string code, std::vector< std::map<std::string, std::string> > comps, std::vector<std::string> pType, std::vector<bool> isOpt, bool isSTDL, bool ipff) {
		std::vector<MethodStorage *>::reverse_iterator iter = this->methodStack.rbegin();
		(*iter)->addMethod(rType, code, comps, pType, isOpt, isSTDL, ipff); // add it to the top of the stack 
	}
	
	
	void MethodStack::pushStorage(MethodStorage * mStore) { // add a new VStore to *this 
		if (mStore == NULL) this->methodStack.push_back(new MethodStorage());
		else this->methodStack.push_back(mStore);
	}
	
	
	void MethodStack::toString() const {
		std::vector<MethodStorage *>::const_reverse_iterator iter = this->methodStack.rbegin();
		for (; iter != this->methodStack.rend(); ++iter) {
			(*iter)->toString();
		}
	}
	
	
	const MethodDefinition * MethodStack::findMatch(std::vector<std::string> vocab, std::vector<std::string> types) const {
		const MethodDefinition * temp = NULL;
		std::vector<MethodStorage *>::const_reverse_iterator iter = this->methodStack.rbegin();
		for (; iter != this->methodStack.rend(); ++iter) {
			temp = (*iter)->findMatch(vocab, types);
			if (temp != NULL) return temp;
		}
		
		return temp;
	}
	
	bool MethodStack::isPostFixFunc(std::string mName) const {
		std::vector<MethodStorage *>::const_reverse_iterator iter = this->methodStack.rbegin();
		for (; iter != this->methodStack.rend(); ++iter) { // find the first (highest) declaration of the method 
			if ((*iter)->methodExists(mName)) {return (*iter)->isPostFixFunc(mName);}
		}
		
		return false;
	}


	int MethodStack::stackSize() const {return this->methodStack.size();}
	
/// ### ///
