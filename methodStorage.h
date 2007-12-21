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

	void toString() const;
	int calculateMatch(std::vector<std::string>, std::vector<std::string>) const;
	
};


class MethodStorage {
protected:
	std::multimap<std::map<std::string, std::string>, MethodDefinition *> methods;
	
public:
	MethodStorage();
	~MethodStorage();
	void clearMemory();
	
	bool methodExists(std::string) const;
	const MethodDefinition * getMethod(std::string);
	bool isSTDL(std::string) const;
	void addMethod(std::string, std::string, std::vector< std::map<std::string, std::string> >, std::vector<std::string>, std::vector<bool>, bool = false, bool = false);

	void toString() const;
	const MethodDefinition * findMatch(std::vector<std::string>, std::vector<std::string>) const;
	bool isPostFixFunc(std::string) const;
	
};


class MethodStack {
protected:
	std::vector<MethodStorage *> methodStack; // method stack 
	
public:
	MethodStack();
	MethodStack(const MethodStack *);
	~MethodStack();
	void clearMemory();
	void pop();
	
	bool methodExists(std::string) const;
	const MethodDefinition * getMethod(std::string);
	bool isSTDL(std::string) const;
	void addMethod(std::string, std::string, std::vector< std::map<std::string, std::string> >, std::vector<std::string>, std::vector<bool>, bool = false, bool = false);
	
	void pushStorage(MethodStorage * = NULL);
	
	void toString() const;
	const MethodDefinition * findMatch(std::vector<std::string>, std::vector<std::string>) const;
	bool isPostFixFunc(std::string) const;
	
	int stackSize() const;
	
};
/// ################################ ///

#endif
