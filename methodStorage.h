#ifndef METHODSTORAGE_H
#define METHODSTORAGE_H

#include <vector>
#include <map>


//################ PROTOTYPES ################//
class MethodDefinition {
protected:
	
public:
	std::string code, returnType;
	std::vector< std::map<std::string, std::string> > vocabulary;
	std::vector<bool> isOptional;
	std::vector<std::string> paramType;
	bool isSTDL, isPostFixFunc;
	
	MethodDefinition(std::string, std::string, std::vector< std::map<std::string, std::string> >, std::vector<std::string>, std::vector<bool>, bool = false, bool = false);
	~MethodDefinition();

	void toString();
	int calculateMatch(std::vector<std::string>, std::vector<std::string>);
	
};


class MethodStorage {
protected:
	std::multimap<std::map<std::string, std::string>, MethodDefinition *> methods;
	
public:
	MethodStorage();
	~MethodStorage();
	void clearMemory();
	
	bool methodExists(std::string);
	MethodDefinition * getMethod(std::string);
	bool isSTDL(std::string);
	void addMethod(std::string, std::string, std::vector< std::map<std::string, std::string> >, std::vector<std::string>, std::vector<bool>, bool = false, bool = false);

	void toString();
	MethodDefinition * findMatch(std::vector<std::string>, std::vector<std::string>);
	bool isPostFixFunc(std::string);
	
};


class MethodStack {
protected:
	std::vector<MethodStorage *> methodStack; // method stack 
	
public:
	MethodStack();
	MethodStack(MethodStack *);
	~MethodStack();
	void clearMemory();
	void pop();
	
	bool methodExists(std::string);
	MethodDefinition * getMethod(std::string);
	bool isSTDL(std::string);
	void addMethod(std::string, std::string, std::vector< std::map<std::string, std::string> >, std::vector<std::string>, std::vector<bool>, bool = false, bool = false);
	
	void pushStorage(MethodStorage * = NULL);
	
	void toString();
	MethodDefinition * findMatch(std::vector<std::string>, std::vector<std::string>);
	bool isPostFixFunc(std::string);
	
	int stackSize() {return this->methodStack.size();}
	
};
/// ################################ ///

#endif
