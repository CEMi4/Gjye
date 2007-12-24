#ifndef VARSTORAGE_H
#define VARSTORAGE_H

extern bool SHOW_DEBUGGING;

#include <map>
#include <vector>


//################ PROTOTYPES ################//
struct strCmp {
	bool operator() (std::string, std::string) const;
};



class InternalDataType {
protected: 
	void * dataValue; // a void pointer to whatever it is ... 
	
	int validType; // which type is currently valid (best effort) 
	/* 
	* value's representation as a string -- 0
	* 1 and 2 are arrays and vectors, respectively 
	* as an int -- 4
	* as a double -- 8
	* someday ... complex number -- 16
	* someday ... ClassWrap -- 32
	*/
	
	int references; // how many pointers are pointing to you? 
	
public: 
	InternalDataType( void *, int );
	InternalDataType(const InternalDataType *);
	~InternalDataType();
	
	
	void setData( void *, int ); // update the object 
	
	void upRef();
	void downRef();
	void nullRef();
	
	int getRefs() const;
	int getType() const;
	void * getValue() const;
};



class VariableStorage {
protected: 
	std::map<std::string, InternalDataType *, strCmp> dataNames; // must be a pointer for mult. references 
	
	int startVariableReference; // 1000000000 -- hopefully find a better way than this later 
	int arrayAutoIndex;
	
	// all of these methods will be called in the CORRECT level already (bc of getVector) 
	
	void refresh(bool = false);
	
	void garbageCollect();
	
public: 
	VariableStorage(int = 0);
	VariableStorage(const VariableStorage *);
	~VariableStorage();
	
	std::string variableReferencer(std::string);
	
	bool addVector(std::string, const VariableStorage &, int = -1);
	bool addVariable(std::string = "", std::string = "", int = -1);
	
	bool removeVariable(std::string);
	
	VariableStorage * getVector(std::string, bool = true);
	VariableStorage * vecStringToVector(std::string *, bool = true, bool = false);
	
	std::string getData(std::string, bool = true);
	std::map<std::string, InternalDataType *, strCmp> * getVectorNodes();
	std::string dumpData() const;
	
	int size() const;
	int type(std::string = "") const;
	std::string typeString(std::string = "") const;
	bool variableExists(std::string) const;
};



class DataStorageStack {
protected: 
	std::vector<VariableStorage *> dataStack; // data stack 
	
public: 
	DataStorageStack();
	DataStorageStack(VariableStorage *);
	DataStorageStack(const DataStorageStack *);
	~DataStorageStack();
	void clearMemory();
	void pop(); // pop and destroy the top of the stack 
	
	std::string variableReferencer(std::string);
	
	bool addVector(std::string, const VariableStorage &, int = -1, bool = false);
	bool addVariable(std::string = "", std::string = "", int = -1, bool = false);
	
	bool removeVariable(std::string);
	
	VariableStorage * getVector(std::string, bool = true);
	VariableStorage * vecStringToVector(std::string *, bool = true, bool = false);
	
	std::string getData(std::string, bool = true);
	std::map<std::string, InternalDataType *, strCmp> * getVectorNodes();
	std::string dumpData() const;
	
	int size() const;
	int type(std::string = "") const;
	std::string typeString(std::string = "") const;
	bool variableExists(std::string) const;
	
	void pushStorage(VariableStorage * = NULL);
	
	int stackSize() const;
};
/// ################################ ///

#endif
