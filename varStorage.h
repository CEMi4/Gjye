#ifndef VARSTORAGE_H
#define VARSTORAGE_H

#include <map>
#include <vector>


//################ PROTOTYPES ################//
struct strCmp {
	bool operator() (std::string, std::string) const;
};



class InternalDataType {
protected: 
	std::string stringValue; // value's representation as a string -- 0
	// note:  1 and 2 are arrays and vectors 
	int intValue; // as an int -- 3
	double dblValue; // as a double -- 4 
	// someday ... complex number -- 5
	// someday ... ClassWrap -- 6
	
	int validType; // which type is currently valid (best effort) 
	int references; // how many pointers are pointint to you? 
	
	int bestType( const std::string ) const;
	
public: 
	InternalDataType(std::string = ""); // default constructor definitely not recommended, but whatever 
	~InternalDataType();
	
	
	void modify( double, bool = true ); // update it as an integer or double 
	
	void modify(std::string); // update the value/object 
	
	
	int getType() const;
	
	int getInt() const;
	double getDbl() const;
	std::string getString() const;
	// and the like ... 
};



class VariableStorage {
protected: 
	std::map<std::string, bool, strCmp> isScalar; // type
	std::map<std::string, std::string> variableNames; // scalar
	std::map<std::string, VariableStorage> vectorNames; // vector
	
	mutable int startVariableReference; // 1000000000 -- hopefully find a better way than this later 
	int arrayAutoIndex;
	
	// all of these methods will be called in the CORRECT level already (bc of getVector) 
	
	void refresh(bool = false);
	
public: 
	VariableStorage(int = 0);
	~VariableStorage();
	
	std::string variableReferencer(std::string) const;
	
	bool addVector(std::string = "", VariableStorage = NULL, int = -1);
	bool addVariable(std::string = "", std::string = "", int = -1);
	
	VariableStorage * getVector(std::string);
	VariableStorage * vecStringToVector(std::string *, bool = true, bool = false);
	
	bool removeVariable(std::string);
	
	std::string getData(std::string);
	std::map<std::string, bool, strCmp> * getVectorNodes();
	std::string dumpData();
	
	int size() const;
	int type(std::string = "");
	std::string typeString(std::string = "");
	bool variableExists(std::string);
};



class DataStorageStack {
protected: 
	std::vector<VariableStorage *> dataStack; // data stack 
	
public: 
	DataStorageStack();
	DataStorageStack(VariableStorage *);
	DataStorageStack(DataStorageStack *);
	~DataStorageStack();
	void clearMemory();
	void pop(); // pop and destroy the top of the stack 
	
	std::string variableReferencer(std::string);
	
	bool addVector(std::string = "", VariableStorage = NULL, int = -1, bool = false);
	bool addVariable(std::string = "", std::string = "", int = -1, bool = false);
	
	
	VariableStorage * getVector(std::string);
	VariableStorage * vecStringToVector(std::string *, bool = true, bool = false);
	
	bool removeVariable(std::string);
	
	std::string getData(std::string);
	std::map<std::string, bool, strCmp> * getVectorNodes();
	std::string dumpData();
	
	int size();
	int type(std::string = "");
	std::string typeString(std::string = "");
	bool variableExists(std::string);
	
	void pushStorage(VariableStorage * = NULL);
	
	int stackSize() {return this->dataStack.size();}
};
/// ################################ ///

#endif
