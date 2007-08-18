#ifndef OBJMETHODS_H
#define OBJMETHODS_H

extern bool SHOW_DEBUGGING;

#include "varStorage.h"

#include <vector>


//################ PROTOTYPES ################//
class FuncObj {
protected: 
	int levelPointer;
	std::vector<bool> isDefined; // denotes whether verb and complements are defined yet (for building the object) 
	
	std::map<std::string, std::string> vocab; // known valid complements and verbs for this object 
	
	DataStorageStack * dataStructure; 
	
	bool isValidLevel(std::string);
	
public: 
	std::vector< std::map<std::string, std::string> > vocabulary; // valid verb and complements and aliases (in order of occurrence) 
	
	std::vector<bool> isOptional; // which complements are optional (default value given) (including verb) 
	std::vector< std::vector<std::string> > paramType; // what parameter each allows (including verb) 
	std::vector<std::string> returnType; // what the return type is 
	bool isPostFixFunc, isSTDL; // is this a post fix? (really used for the executer only) 
	
	FuncObj(DataStorageStack * = NULL);
	virtual ~FuncObj();
	void importDataStack(DataStorageStack *);
	
	std::string getLevelData(std::string);
	bool setLevelData(std::string, std::string);
	virtual std::string executeCode() = 0;
	
};



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



class Add_Obj : public FuncObj {
private: 
	std::string addToStringVar();
	std::string addToVector(VariableStorage *);
	
public: 
	Add_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class beginsWith_Obj : public FuncObj {
private: 
	std::string beginsWithStringVar(int);
	std::string beginsWithVector(int, VariableStorage *);
	
public: 
	beginsWith_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class CharAt_Obj : public FuncObj {
private: 
	std::string charAtInStringVar();
	
public: 
	CharAt_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Contains_Obj : public FuncObj {
private: 
	std::string containsStringVar(int);
	std::string containsVector(int, VariableStorage *);
	
public: 
	Contains_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Exit_Obj : public FuncObj {
private: 
	
public: 
	Exit_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class IndexOf_Obj : public FuncObj {
private: 
	std::string indexOfInStringVar(int);
	std::string indexOfInVector(int, VariableStorage *);
	
public: 
	IndexOf_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class isDefined_Obj : public FuncObj {
private: 
	
public: 
	isDefined_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Join_Obj : public FuncObj {
private: 
	
public: 
	Join_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Local_Obj : public FuncObj {
private: 
	
public: 
	Local_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class LowerUpperCase_Obj : public FuncObj {
private: 
	std::string lowerUpperStringVar();
	
public: 
	LowerUpperCase_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class LowerUpperCaseFirst_Obj : public FuncObj {
private: 
	std::string luCaseFirstStringVar();
	
public: 
	LowerUpperCaseFirst_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Length_Obj : public FuncObj {
private: 
	
public: 
	Length_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Print_Obj : public FuncObj {
private: 
	
public: 
	Print_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Random_Obj : public FuncObj {
private: 
	std::string randomStringVar();
	
public: 
	Random_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Remove_Obj : public FuncObj {
private: 
	std::string deleteVar();
	std::string deleteVec();
	
public: 
	Remove_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Reverse_Obj : public FuncObj {
private: 
	std::string reverseStringVar();
	
public: 
	Reverse_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Round_Obj : public FuncObj {
private: 
	std::string roundStringVar();
	
public: 
	Round_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Select_Obj : public FuncObj {
private: 
	std::string selectFromStringVar();
	
public: 
	Select_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Split_Obj : public FuncObj {
private: 
	
public: 
	Split_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class ValueOf_Obj : public FuncObj {
private: 
	
public: 
	ValueOf_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};



class Void_Obj : public FuncObj {
private: 
	
public: 
	Void_Obj(DataStorageStack * = NULL);
	std::string executeCode();
	
};
// ################################ ///

#endif
