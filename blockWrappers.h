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



class ClassWrap : public BlockWrap {
protected: 
	MethodStack * methods;
	std::string classType; // the name of this class 
	std::vector<std::string> friends; // who has access to my shizz 
	
public:
	ClassWrap(std::string = "", std::string = "", const EnviroWrap * = NULL);
	virtual ~ClassWrap();
	void clearMemoryStack();
	
};
/// ################################ ///

#endif
