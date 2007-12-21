#ifndef BLOCKWRAPPERS_H
#define BLOCKWRAPPERS_H

extern bool SHOW_DEBUGGING;

#include "enviroWrap.h"
#include "methodStorage.h"
#include "varStorage.h"
#include "tokenGroups.h"

#include <map>
#include <vector>


//################ PROTOTYPES ################//
class BlockWrap {
protected: 
	std::string codeInput;
	EnviroWrap * environment; // declare (locally) global variable storage object
	
public: 
	BlockWrap(std::string = "", const EnviroWrap * = NULL);
	virtual ~BlockWrap();
	void importDataStack(EnviroWrap *);
	virtual void clearMemoryStack();
	
	virtual void tokenizeBlocks(TokenGroup *); // only tokenize when it's a class 
	std::string executeCode();
	
};



class MethodWrap : public BlockWrap {
protected: 
	
public:
	MethodWrap(std::string = "", const EnviroWrap * = NULL);
	
};



class ClassWrap : public MethodWrap {
protected: 
	MethodStack * methods;
	std::vector<std::string> friends; // who has access to my shizz 
	
public:
	ClassWrap(std::string = "", const EnviroWrap * = NULL);
	virtual ~ClassWrap();
	void clearMemoryStack();
	
};
/// ################################ ///

#endif
