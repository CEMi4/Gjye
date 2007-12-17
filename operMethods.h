#ifndef OPERMETHODS_H
#define OPERMETHODS_H

extern bool SHOW_DEBUGGING;

#include "varStorage.h"
#include "objMethods.h"


//################ PROTOTYPES ################//
class Addition_Obj : public FuncObj {
private: 
	
public: 
	Addition_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Assignation_Obj : public FuncObj {
private: 
	
public: 
	Assignation_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Compares_Obj : public FuncObj {
private: 
	
public: 
	Compares_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Division_Obj : public FuncObj {
private: 
	
public: 
	Division_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Modulus_Obj : public FuncObj {
private: 
	
public: 
	Modulus_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Multiplication_Obj : public FuncObj {
private: 
	
public: 
	Multiplication_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Subtraction_Obj : public FuncObj {
private: 
	
public: 
	Subtraction_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Unaries_Obj : public FuncObj {
private: 
	
public: 
	Unaries_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};
// ################################ ///

#endif
