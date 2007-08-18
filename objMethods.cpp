#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>

#include "objMethods.h"
#include "miscTools.h"


//################ GJYE OBJ METHODS ################//
//class FuncObj 
	
	/* protected */
	bool FuncObj::isValidLevel(std::string strCheck) {
		if (vocab.count(strCheck) > 0) {return true;}
		return false;
	}
	
	
	/* public */ 
	FuncObj::FuncObj(DataStorageStack * storage) {
		this->isSTDL = true;
		this->levelPointer = 0;
		this->isPostFixFunc = false;
		this->dataStructure = storage;
	}
	FuncObj::~FuncObj() {}
	
	void FuncObj::importDataStack(DataStorageStack * storage) {
		this->dataStructure = storage;
	}
	
	std::string FuncObj::getLevelData(std::string strCheck) {
		return vocab[strCheck];
	}
	
	bool FuncObj::setLevelData(std::string lName, std::string lContents) {
		if (this->isValidLevel(lName) == true) {
			vocab[lName] = lContents;
			return true;
		}
		return false;
	}
/// ### ///





// ## //
//class Addition_Obj : public FuncObj 
	
	/* public */ 
	Addition_Obj::Addition_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["right"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);		
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("%");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // LEFT
		tempVec.push_back("$"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("%"); // LEFT
		tempVec.push_back("%"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["left"] = "";
		vocab["right"] = "";
	}
	
	std::string Addition_Obj::executeCode() {
		std::string returnValue = "", tokenQualifier = "";
		bool scalarL = false, scalarR = false;
		VariableStorage * rightVector = NULL, * leftVector = NULL;
		
		if (vocab["left"].at(0) == '%') {
			std::string varDataL = vocab["left"].substr(1,vocab["left"].length()-2);
			leftVector = dataStructure->vecStringToVector(&varDataL);
			varDataL = tools::prepareVectorData(dataStructure, varDataL);
			if (leftVector->type(varDataL) == 0) {scalarL = true;}
			leftVector = leftVector->getVector(varDataL); // jump in prep for VECTOR combination (though we may not need it) 
		}
		
		if (vocab["right"].at(0) == '%') {
			std::string varDataR = vocab["right"].substr(1,vocab["right"].length()-1);
			rightVector = dataStructure->vecStringToVector(&varDataR);
			varDataR = tools::prepareVectorData(dataStructure, varDataR);
			if (rightVector->type(varDataR) == 0) {scalarR = true;}
			rightVector = rightVector->getVector(varDataR); // jump in prep for VECTOR combination (though we may not need it) 
		}
		
		if (vocab["left"].at(0) == '$' || vocab["right"].at(0) == '$' || scalarL || scalarR) { // addition/concatenation (SCALAR)
			std::string varDataL = tools::prepareVectorData(dataStructure, vocab["left"].substr(0,vocab["left"].length()-1));
			std::string varDataR =  tools::prepareVectorData(dataStructure, vocab["right"]);
			
			if (varDataL == "") {varDataL = "0";} // positive then
			
			if (tools::isNumber(varDataL) && tools::isNumber(varDataR)) { // addition 
				double doubleDataL,doubleDataR;
				doubleDataL =  tools::stringToInt(varDataL);
				doubleDataR =  tools::stringToInt(varDataR);
				returnValue = tools::intToString(doubleDataL + doubleDataR);
			}
			else { // concatenation
				returnValue = varDataL.append(varDataR);
			}
			
			// turn return string into a variable 
			tokenQualifier = dataStructure->variableReferencer("_STRING_");
			dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
			tokenQualifier.insert(0,"$");
		}
		else if (!(scalarL || scalarR)) { // combination (VECTOR) 
			tokenQualifier = dataStructure->variableReferencer("_VECTOR_");
			dataStructure->addVector(tokenQualifier, *(new VariableStorage), -1, true); // create an empty set 
			
			std::map<std::string, bool, strCmp> * leftNodes = leftVector->getVectorNodes();
			std::map<std::string, bool, strCmp> * rightNodes = rightVector->getVectorNodes();
			
			std::map<std::string, bool>::const_iterator iter;
			std::string addName = "";
			for (iter = leftNodes->begin(); iter != leftNodes->end(); ++iter) {
				if (leftVector->type() >= 0) addName = ""; else addName = iter->first;
				if ((*leftNodes)[iter->first] == false) dataStructure->getVector(tokenQualifier)->addVector( addName   , *leftVector->getVector(iter->first)    ); // it's a vector type 
				else dataStructure->getVector(tokenQualifier)->addVariable(addName, leftVector->getData(iter->first));
			}
			
			for (iter = rightNodes->begin(); iter != rightNodes->end(); ++iter) {
				if (rightVector->type() >= 0) addName = ""; else addName = iter->first;
				if ((*rightNodes)[iter->first] == false) dataStructure->getVector(tokenQualifier)->addVector(    addName, *rightVector->getVector(iter->first)    ); // it's a vector type 
				else dataStructure->getVector(tokenQualifier)->addVariable(addName, rightVector->getData(iter->first));
			}
			
			tokenQualifier.insert(0,"%");
		}
		else {std::cout << "WARNING :: Addition: Unknown Data Type!" <<std::endl;}
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Assignation_Obj : public FuncObj 
	
	/* public */ 
	Assignation_Obj::Assignation_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["right"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("%");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // LEFT
		tempVec.push_back("$"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("%"); // LEFT
		tempVec.push_back("%"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["left"] = "";
		vocab["right"] = "";
	}
	
	std::string Assignation_Obj::executeCode() {
		std::string tokenQualifier = "";
		
		if (vocab["left"].at(0) == '$') { // assignation (VARIABLE)
			std::string varName = vocab["left"].substr(1,vocab["left"].length()-2), varData = tools::prepareVectorData(dataStructure, vocab["right"]);
			dataStructure->addVariable(varName,varData);
			tokenQualifier = "$" + varName;
		}
		else if (vocab["left"].at(0) == '%') { // assignation (VECTOR)
			std::string vecData = "", vecName, vecNameCopy = vecName = vocab["left"].substr(1,vocab["left"].length()-2);
			
			VariableStorage * leftVector = NULL;
			leftVector = dataStructure->vecStringToVector(&vecNameCopy, false); // jump to the vector object we want to modify (modifies vecNameCopy to make it the highest level) 
			vecNameCopy = tools::prepareVectorData(dataStructure, vecNameCopy); // this is the highest level!  ie)  %topName[index][vecNameCopy];  or  %vecNameCopy; 
			
			vecData = vocab["right"].substr(1,vocab["right"].length()-1);
			
			if (vocab["right"].at(0) == '%') {
				VariableStorage * rightVector = NULL;
				rightVector = dataStructure->vecStringToVector(&vecData);
				vecData = tools::prepareVectorData(dataStructure, vecData);
				
				if (rightVector->type(vecData) > 0) { // vector to vector 
					leftVector->addVector(vecNameCopy, *rightVector->getVector(vecData));
					tokenQualifier = "%" + vecName;
				}
				else if (vocab["left"].find("[") != std::string::npos) { // scalar vector, check -- it must NOT be the lowest level!  (%vec=$scalar;  is INVALID) 
					leftVector->addVariable(vecNameCopy, rightVector->getData(vecData)); // we could to prepareVectorData(dataStructure, vocab["right"]) 
					tokenQualifier = "%" + vecName;
				}
				else {std::cout << "ERROR :: Mishandling: Cannot convert scalar to vector (vec)!" <<std::endl;exit(1);}
			}
			else if (vocab["right"].at(0) == '$' && vocab["left"].find("[") != std::string::npos) { // scalar into vector -- it must NOT be the lowest level!  (%vec=$var;  is INVALID) 
				leftVector->addVariable(vecNameCopy, tools::prepareVectorData(dataStructure, "$" + vecData));
				tokenQualifier = "%" + vecName;
			} 
			else {std::cout << "ERROR :: Mishandling: Cannot convert scalar to vector!" <<std::endl;exit(1);}
		}
		else {std::cout << "WARNING :: Assignation: Unknown Data Type!" <<std::endl;}
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Compares_Obj : public FuncObj 
	
	/* public */ 
	Compares_Obj::Compares_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["right"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITION
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // LEFT
		tempVec.push_back("$"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["left"] = "";
		vocab["right"] = "";
	}
	
	std::string Compares_Obj::executeCode() {
		std::string returnValue = "";
		bool scalarL = false, scalarR = false;
		int type = -1, skipAmt = 1;
		
		if ( vocab["left"].at(vocab["left"].length()-1) == '>' ) type = 0;
		else if ( vocab["left"].at(vocab["left"].length()-1) == '<' ) type = 1;
		else if ( vocab["left"].at(vocab["left"].length()-2) == '>' ) {type = 2;skipAmt = 2;}
		else if ( vocab["left"].at(vocab["left"].length()-2) == '<' ) {type = 3;skipAmt = 2;}
		else if ( vocab["left"].at(vocab["left"].length()-2) == '=' && vocab["left"].at(vocab["left"].length()-1) == '=' ) {type = 4;skipAmt = 2;}
		else if ( vocab["left"].at(vocab["left"].length()-2) == '!' && vocab["left"].at(vocab["left"].length()-1) == '=' ) {type = 5;skipAmt = 2;}
		else {std::cout << "ERROR :: Compares: Unknown type!" <<std::endl;exit(1);}
		
		if (vocab["left"].at(0) == '%') {
			std::string varDataL = vocab["left"].substr(1,vocab["left"].length()-(skipAmt+1));
			VariableStorage * leftVector = dataStructure->vecStringToVector(&varDataL);
			varDataL = tools::prepareVectorData(dataStructure, varDataL);
			if (leftVector->type(varDataL) == 0) {scalarL = true;}
		}
		
		if (vocab["right"].at(0) == '%') {
			std::string varDataR = vocab["right"].substr(1,vocab["right"].length()-1);
			VariableStorage * rightVector = dataStructure->vecStringToVector(&varDataR);
			varDataR = tools::prepareVectorData(dataStructure, varDataR);
			if (rightVector->type(varDataR) == 0) {scalarR = true;}
		}
		
		if (vocab["left"].at(0) == '$' || vocab["right"].at(0) == '$' || scalarL || scalarR) { // compare (SCALAR)
			std::string varDataL = tools::prepareVectorData(dataStructure, vocab["left"].substr(0,vocab["left"].length()-skipAmt));
			std::string varDataR =  tools::prepareVectorData(dataStructure, vocab["right"]);
			
			if (tools::isNumber(varDataL) && tools::isNumber(varDataR)) {
				double doubleDataL,doubleDataR;
				doubleDataL =  tools::stringToInt(varDataL);
				doubleDataR =  tools::stringToInt(varDataR);
				
				if ( doubleDataL > doubleDataR ) {
					if ( type == 0 || type == 2 || type == 5 ) returnValue = "1";
					else returnValue = "0";
				}
				else if ( doubleDataL < doubleDataR ) {
					if ( type == 0 || type == 2 || type == 4 ) returnValue = "0";
					else returnValue = "1";
				}
				else { // == 
					if ( type == 2 || type == 3 || type == 4 ) returnValue = "1";
					else returnValue = "0";
				}
				
			} else {std::cout << "WARNING :: Compares: Unknown Operation (NaN)!" <<std::endl;}
			
		} else {std::cout << "WARNING :: Compares: Unknown Data Type!" <<std::endl;}
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Division_Obj : public FuncObj 
	
	/* public */ 
	Division_Obj::Division_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["right"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);		
		
		
		// 1 DEFINITION
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // LEFT
		tempVec.push_back("$"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["left"] = "";
		vocab["right"] = "";
	}
	
	std::string Division_Obj::executeCode() {
		std::string returnValue = "";
		bool scalarL = false, scalarR = false;
		
		if (vocab["left"].at(0) == '%') {
			std::string varDataL = vocab["left"].substr(1,vocab["left"].length()-2);
			VariableStorage * leftVector = dataStructure->vecStringToVector(&varDataL);
			varDataL = tools::prepareVectorData(dataStructure, varDataL);
			if (leftVector->type(varDataL) == 0) {scalarL = true;}
		}
		
		if (vocab["right"].at(0) == '%') {
			std::string varDataR = vocab["right"].substr(1,vocab["right"].length()-1);
			VariableStorage * rightVector = dataStructure->vecStringToVector(&varDataR);
			varDataR = tools::prepareVectorData(dataStructure, varDataR);
			if (rightVector->type(varDataR) == 0) {scalarR = true;}
		}
		
		if (vocab["left"].at(0) == '$' || vocab["right"].at(0) == '$' || scalarL || scalarR) { // division (SCALAR)
			std::string varDataL = tools::prepareVectorData(dataStructure, vocab["left"].substr(0,vocab["left"].length()-1));
			std::string varDataR =  tools::prepareVectorData(dataStructure, vocab["right"]);
			
			if (tools::isNumber(varDataL) && tools::isNumber(varDataR)) {
				double doubleDataL,doubleDataR;
				doubleDataL =  tools::stringToInt(varDataL);
				doubleDataR =  tools::stringToInt(varDataR);
				returnValue = tools::intToString(doubleDataL/doubleDataR);
			} else {std::cout << "WARNING :: Division: Unknown Operation (NaN)!" <<std::endl;}
			
		} else {std::cout << "WARNING :: Division: Unknown Data Type!" <<std::endl;}
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Modulus_Obj : public FuncObj 
	
	/* public */ 
	Modulus_Obj::Modulus_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["right"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);		
		
		
		// 1 DEFINITION
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // LEFT
		tempVec.push_back("$"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["left"] = "";
		vocab["right"] = "";
	}
	
	std::string Modulus_Obj::executeCode() {
		std::string returnValue = "";
		bool scalarL = false, scalarR = false;
		
		if (vocab["left"].at(0) == '%') {
			std::string varDataL = vocab["left"].substr(1,vocab["left"].length()-2);
			VariableStorage * leftVector = dataStructure->vecStringToVector(&varDataL);
			varDataL = tools::prepareVectorData(dataStructure, varDataL);
			if (leftVector->type(varDataL) == 0) {scalarL = true;}
		}
		
		if (vocab["right"].at(0) == '%') {
			std::string varDataR = vocab["right"].substr(1,vocab["right"].length()-1);
			VariableStorage * rightVector = dataStructure->vecStringToVector(&varDataR);
			varDataR = tools::prepareVectorData(dataStructure, varDataR);
			if (rightVector->type(varDataR) == 0) {scalarR = true;}
		}
		
		if (vocab["left"].at(0) == '$' || vocab["right"].at(0) == '$' || scalarL || scalarR) { // modulus (SCALAR)
			std::string varDataL = tools::prepareVectorData(dataStructure, vocab["left"].substr(0,vocab["left"].length()-1));
			std::string varDataR =  tools::prepareVectorData(dataStructure, vocab["right"]);
			
			if (tools::isNumber(varDataL) && tools::isNumber(varDataR)) {
				double doubleDataL,doubleDataR;
				doubleDataL =  tools::stringToInt(varDataL);
				doubleDataR =  tools::stringToInt(varDataR);
				returnValue = tools::intToString(fmod(doubleDataL, doubleDataR));
			} else {std::cout << "WARNING :: Modulus: Unknown Operation (NaN)!" <<std::endl;}
			
		} else {std::cout << "WARNING :: Modulus: Unknown Data Type!" <<std::endl;}
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Multiplication_Obj : public FuncObj 
	
	/* public */ 
	Multiplication_Obj::Multiplication_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["right"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);		
		
		
		// 1 DEFINITION
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // LEFT
		tempVec.push_back("$"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["left"] = "";
		vocab["right"] = "";
	}
	
	std::string Multiplication_Obj::executeCode() {
		std::string returnValue = "";
		bool scalarL = false, scalarR = false;
		
		if (vocab["left"].at(0) == '%') {
			std::string varDataL = vocab["left"].substr(1,vocab["left"].length()-2);
			VariableStorage * leftVector = dataStructure->vecStringToVector(&varDataL);
			varDataL = tools::prepareVectorData(dataStructure, varDataL);
			if (leftVector->type(varDataL) == 0) {scalarL = true;}
		}
		
		if (vocab["right"].at(0) == '%') {
			std::string varDataR = vocab["right"].substr(1,vocab["right"].length()-1);
			VariableStorage * rightVector = dataStructure->vecStringToVector(&varDataR);
			varDataR = tools::prepareVectorData(dataStructure, varDataR);
			if (rightVector->type(varDataR) == 0) {scalarR = true;}
		}
		
		if (vocab["left"].at(0) == '$' || vocab["right"].at(0) == '$' || scalarL || scalarR) { // multiplication (SCALAR)
			std::string varDataL = tools::prepareVectorData(dataStructure, vocab["left"].substr(0,vocab["left"].length()-1));
			std::string varDataR =  tools::prepareVectorData(dataStructure, vocab["right"]);
			
			if (tools::isNumber(varDataL) && tools::isNumber(varDataR)) {
				double doubleDataL,doubleDataR;
				doubleDataL =  tools::stringToInt(varDataL);
				doubleDataR =  tools::stringToInt(varDataR);
				returnValue = tools::intToString(doubleDataL*doubleDataR);
			} else {std::cout << "WARNING :: Multiplication: Unknown Operation (NaN)!" <<std::endl;}
			
		} else {std::cout << "WARNING :: Multiplication: Unknown Data Type!" <<std::endl;}
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Subtraction_Obj : public FuncObj 
	
	/* public */ 
	Subtraction_Obj::Subtraction_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["right"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);		
		
		
		// 1 DEFINITION
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // LEFT
		tempVec.push_back("$"); // RIGHT
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["left"] = "";
		vocab["right"] = "";
	}
	
	std::string Subtraction_Obj::executeCode() {
		std::string returnValue = "";
		bool scalarL = false, scalarR = false;
		
		if (vocab["left"].at(0) == '%') {
			std::string varDataL = vocab["left"].substr(1,vocab["left"].length()-2);
			VariableStorage * leftVector = dataStructure->vecStringToVector(&varDataL);
			varDataL = tools::prepareVectorData(dataStructure, varDataL);
			if (leftVector->type(varDataL) == 0) {scalarL = true;}
		}
		
		if (vocab["right"].at(0) == '%') {
			std::string varDataR = vocab["right"].substr(1,vocab["right"].length()-1);
			VariableStorage * rightVector = dataStructure->vecStringToVector(&varDataR);
			varDataR = tools::prepareVectorData(dataStructure, varDataR);
			if (rightVector->type(varDataR) == 0) {scalarR = true;}
		}
		
		if (vocab["left"].at(0) == '$' || vocab["right"].at(0) == '$' || scalarL || scalarR) { // subtraction (SCALAR)
			std::string varDataL = tools::prepareVectorData(dataStructure, vocab["left"].substr(0,vocab["left"].length()-1));
			std::string varDataR =  tools::prepareVectorData(dataStructure, vocab["right"]);
			
			if (varDataL == "") {varDataL = "0";} // negative then
			
			if (tools::isNumber(varDataL) && tools::isNumber(varDataR)) {
				double doubleDataL,doubleDataR;
				doubleDataL =  tools::stringToInt(varDataL);
				doubleDataR =  tools::stringToInt(varDataR);
				returnValue = tools::intToString(doubleDataL - doubleDataR);
			} else {std::cout << "WARNING :: Subtraction: Unknown Operation (NaN)!" <<std::endl;} // allow difference of vectors?  (like addition has) 
			
		} else {std::cout << "WARNING :: Subtraction: Unknown Data Type!" <<std::endl;}
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Add_Obj : public FuncObj 
	
	/* private */ 
	std::string Add_Obj::addToStringVar() {
		std::string addThisData = "", addType = "", toThisData = "";
		if (vocab["Prefix"] != "") {addThisData = tools::prepareVectorData(dataStructure, vocab["Prefix"]);addType = "prefix";}
		else if (vocab["Postfix"] != "") {addThisData = tools::prepareVectorData(dataStructure, vocab["Postfix"]);addType = "postfix";}
		else {addThisData = tools::prepareVectorData(dataStructure, vocab["Add"]);addType = "postfix";}
		
		toThisData = tools::prepareVectorData(dataStructure, vocab["to"]);
		
		if (vocab["before"] != "") {toThisData.insert((int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["before"])), addThisData);} // inherent precedence 
		else if (vocab["after"] != "") {toThisData.insert((int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["after"]))+1, addThisData);}
		else if (vocab["at"] != "") {toThisData.insert((int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["at"])), addThisData);}
		else if (addType == "prefix") {toThisData = addThisData.append(toThisData);}
		else if (addType == "postfix") {toThisData.append(addThisData);}
		
		std::string varData = vocab["to"];
		if (varData.at(0) == '%') { // it's a (scalar) vector level (we checked to make sure in exec) 
			VariableStorage * vectorStorage = NULL;
			varData = varData.substr(1,varData.length()-1);
			
			vectorStorage = dataStructure->vecStringToVector(&varData); // jump to the vector object we want to check (modifies varData to make it the highest level) 
			varData = tools::prepareVectorData(dataStructure, varData); // this is the highest level!  ie)  %topName[index][varData];  or  %varData; 
			
			if(vectorStorage->addVariable(varData,toThisData) != true) {return "-1";}
		}
		else { // just a plain old variable 
			varData = varData.substr(1,varData.length()-1);
			if(dataStructure->addVariable(varData,toThisData) != true) {return "-1";}
		}
				
		return tools::intToString(toThisData.length());
	}
	
	
	std::string Add_Obj::addToVector(VariableStorage * toVectorStorage) { // to is indeed a vector (not a scalar level of a vector) (already jumped) 
		std::string addThisData = "", addType = "";
		if (vocab["Prefix"] != "") {addThisData = vocab["Prefix"];addType = "prefix";}
		else if (vocab["Postfix"] != "") {addThisData = vocab["Postfix"];addType = "postfix";}
		else {addThisData = vocab["Add"];addType = "postfix";}
		
		VariableStorage * addVectorStorage = NULL;
		std::string addData = addThisData.substr(1,addThisData.length()-1);
		addVectorStorage = dataStructure->vecStringToVector(&addData); // jump to the vector object we want to check (modifies varData to make it the highest level) 
		addData = tools::prepareVectorData(dataStructure, addData); // this is the highest level!  ie)  %topName[index][varData];  or  %varData; 
		
		if(addVectorStorage->type(addData) == 0) { // add a scalar to a vector 
			if (vocab["before"] != "") {toVectorStorage->addVariable("", tools::prepareVectorData(dataStructure, addThisData), (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["before"])));} // inherent precedence 
			else if (vocab["after"] != "") {toVectorStorage->addVariable("", tools::prepareVectorData(dataStructure, addThisData), (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["after"]))+1);}
			else if (vocab["at"] != "") {toVectorStorage->addVariable("", tools::prepareVectorData(dataStructure, addThisData), (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["at"])));}
			else if (addType == "prefix") {toVectorStorage->addVariable("", tools::prepareVectorData(dataStructure, addThisData), 0);}
			else if (addType == "postfix") {toVectorStorage->addVariable("", tools::prepareVectorData(dataStructure, addThisData));}
		} else { // add a vector to a vector 
			addVectorStorage = addVectorStorage->getVector(addData);
			if (vocab["before"] != "") {toVectorStorage->addVector("", *addVectorStorage, (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["before"])));} // inherent precedence 
			else if (vocab["after"] != "") {toVectorStorage->addVector("", *addVectorStorage, (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["after"]))+1);}
			else if (vocab["at"] != "") {toVectorStorage->addVector("", *addVectorStorage, (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["at"])));}
			else if (addType == "prefix") {toVectorStorage->addVector("", *addVectorStorage, 0);}
			else if (addType == "postfix") {toVectorStorage->addVector("", *addVectorStorage);}
		}
		
		return tools::intToString(toVectorStorage->size());
	}
	
	
	/* public */ 
	Add_Obj::Add_Obj(DataStorageStack * storage) : FuncObj(storage) { // Put in (alias) 
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Add"] = "";
			tempMap["Insert"] = ""; // alias 
		tempMap["Prefix"] = "";
			tempMap["Prepend"] = ""; // alias 
		tempMap["Postfix"] = "";
			tempMap["Append"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["to"] = "";
			tempMap["in"] = ""; // alias 
			tempMap["into"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 3RD PARAMETER
		tempMap["before"] = "";
			tempMap["after"] = ""; // level alias 
			tempMap["at"] = ""; // level alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		
		// 3 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
		tempVec.push_back("$"); // PARA3
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("$"); // VERB
		tempVec.push_back("%"); // PARA2
		tempVec.push_back("$"); // PARA3
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #3
		tempVec.push_back("%"); // VERB
		tempVec.push_back("%"); // PARA2
		tempVec.push_back("$"); // PARA3
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		
		vocab["Add"] = "";
			vocab["Insert"] = ""; // alias 
		vocab["Prefix"] = "";
			vocab["Prepend"] = ""; // alias 
		vocab["Postfix"] = "";
			vocab["Append"] = ""; // alias 
		
		vocab["to"] = "";
			vocab["in"] = ""; // alias 
			vocab["into"] = ""; // alias 
		
		vocab["before"] = "";
		vocab["after"] = "";
		vocab["at"] = "";
	}
	
	std::string Add_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["Prepend"] != "") {vocab["Prefix"] = vocab["Prepend"];}
		else if (vocab["Append"] != "") {vocab["Postfix"] = vocab["Append"];}
		else if (vocab["Insert"] != "") {vocab["Add"] = vocab["Insert"];}
		
		if (vocab["in"] != "") {vocab["to"] = vocab["in"];}
		else if (vocab["into"] != "") {vocab["to"] = vocab["into"];}
		
		
		if (vocab["to"].length() > 0 && vocab["to"].at(0) == '%') {
			VariableStorage * vectorStorage = NULL;
			std::string toData = vocab["to"].substr(1,vocab["to"].length()-1);
			vectorStorage = dataStructure->vecStringToVector(&toData); // jump to the vector object we want to check (modifies toData to make it the highest level) 
			toData = tools::prepareVectorData(dataStructure, toData); // this is the highest level!  ie)  %topName[index][toData];  or  %toData; 
			
			if (vectorStorage->type(toData) == 0) {returnValue = this->addToStringVar();} // it's actually a scalar 
			else { // it is indeed a vector 
				vectorStorage = vectorStorage->getVector(toData);
				returnValue = this->addToVector(vectorStorage);
			}
		} // vectors
		else if (vocab["to"] != "") {
			returnValue = this->addToStringVar();
		} // stringVar
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		return tokenQualifier;
	}
/// ### ///





// ## //
//class beginsWith_Obj : public FuncObj 
	
	/* private */ 
	std::string beginsWith_Obj::beginsWithStringVar(int type) { // type 0 is begins with, 1 is endswith 
		std::string tmpSto, beWithData;
		std::string thisData = tools::prepareVectorData(dataStructure, vocab["left"]);
		
		if (type == 0) {
			beWithData = tools::prepareVectorData(dataStructure, vocab["beginsWith"]); // must be a scalar (so we force it) 
			tmpSto = thisData.substr(0,beWithData.length());
		}
		else {
			beWithData = tools::prepareVectorData(dataStructure, vocab["endsWith"]); // must be a scalar (so we force it) 
			tmpSto = thisData.substr(thisData.length()-beWithData.length(),beWithData.length());
		}
		
		if (tmpSto == beWithData) return "1";
		else return "0";
	}
	
	std::string beginsWith_Obj::beginsWithVector(int type, VariableStorage * leftVector) {
		std::string beWithData;
		std::map<std::string, bool, strCmp> * leftNodes = leftVector->getVectorNodes();
		
		if (type == 0) {
			beWithData = tools::prepareVectorData(dataStructure, vocab["beginsWith"]); // must be a scalar (so we force it)   this might be ambiguous in the case of  %VEC beginsWith %VEC 
			std::map<std::string, bool>::const_iterator iter = leftNodes->begin();
			if (leftVector->getData(iter->first) == beWithData) return "1";
			else return "0";
		}
		else {
			beWithData = tools::prepareVectorData(dataStructure, vocab["endsWith"]); // must be a scalar (so we force it) 
			std::map<std::string, bool>::reverse_iterator iter = leftNodes->rbegin();
			if (leftVector->getData(iter->first) == beWithData) return "1";
			else return "0";
		}
	}
	
	
	/* public */ 
	beginsWith_Obj::beginsWith_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		this->isPostFixFunc = true;
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["beginsWith"] = "";
			tempMap["endsWith"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("$"); // VERB
		tempVec.push_back("%"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["beginsWith"] = "";
		vocab["endsWith"] = "";
		
		vocab["left"] = "";
	}
	
	std::string beginsWith_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["left"].length() > 0 && vocab["left"].at(0) == '%') {
			VariableStorage * vectorStorage = NULL;
			std::string toData = vocab["left"].substr(1,vocab["left"].length()-1);
			
			vectorStorage = dataStructure->vecStringToVector(&toData); // jump to the vector object we want to check (modifies toData to make it the highest level) 
			toData = tools::prepareVectorData(dataStructure, toData); // this is the highest level!  ie)  %topName[index][toData];  or  %toData; 
			
			int type = (vocab["beginsWith"] != ""?0:1);
			if (vectorStorage->type(toData) == 0) { // it's actually a scalar 	
				returnValue = this->beginsWithStringVar(type);
			}
			else { // it is indeed a vector 
				vectorStorage = vectorStorage->getVector(toData);
				returnValue = this->beginsWithVector(type, vectorStorage);
			}
		} // vectors
		else if (vocab["left"] != "") {
			int type = (vocab["beginsWith"] != ""?0:1);
			returnValue = beginsWithStringVar(type);
		} // stringVar
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## // maybe rename (or alias) ElementAt?   does CharAt work with vectors? 
//class CharAt_Obj : public FuncObj  // non-destructive
	
	/* private */ 
	std::string CharAt_Obj::charAtInStringVar() {
		std::string inData = tools::prepareVectorData(dataStructure, vocab["in"]), returnString = "";
		int charAtData = (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["CharAt"]));
		if (charAtData >= inData.length() || charAtData < 0) {return "";} // out of bounds
		
		returnString = inData.at(charAtData);
		return returnString;
	}
		
	
	/* public */ 
	CharAt_Obj::CharAt_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["CharAt"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["in"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["CharAt"] = "";
		
		vocab["in"] = "";
	}
	
	std::string CharAt_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["in"].length() > 0 && vocab["in"].at(0) == '%') {
			VariableStorage * vectorStorage = NULL;
			std::string toData = vocab["in"].substr(1,vocab["in"].length()-1);
			
			vectorStorage = dataStructure->vecStringToVector(&toData); // jump to the vector object we want to check (modifies toData to make it the highest level) 
			toData = tools::prepareVectorData(dataStructure, toData); // this is the highest level!  ie)  %topName[index][toData];  or  %toData; 
			
			if (vectorStorage->type(toData) == 0) { // it's actually a scalar 	
				returnValue = this->charAtInStringVar();
			} else {} // it is indeed a vector 
		} // VECTOR
		else if (vocab["in"] != "") {returnValue = this->charAtInStringVar();} // stringVar
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Contains_Obj : public FuncObj 
	
	/* private */ 
	std::string Contains_Obj::containsStringVar(int type) { // type 0 is contains, 1 is lacks 
		std::string thisData = tools::prepareVectorData(dataStructure, vocab["left"]), containsData;
		
		if (type == 0) {
			containsData = tools::prepareVectorData(dataStructure, vocab["contains"]);
			if (thisData.find(containsData) == std::string::npos) {return "0";}
			else {return "1";}
		} else {
			containsData = tools::prepareVectorData(dataStructure, vocab["lacks"]);
			if (thisData.find(containsData) == std::string::npos) {return "1";}
			else {return "0";}
		}
	}
	
	std::string Contains_Obj::containsVector(int type, VariableStorage * leftVector) {
		std::map<std::string, bool, strCmp> * leftNodes = leftVector->getVectorNodes();
		std::string containsData;
		bool contains = false;
		
		if (type == 0) containsData = tools::prepareVectorData(dataStructure, vocab["contains"]);
		else containsData = tools::prepareVectorData(dataStructure, vocab["lacks"]);
		
		std::map<std::string, bool>::const_iterator iter;
		for (iter = leftNodes->begin(); iter != leftNodes->end(); ++iter) {
			if ((*leftNodes)[iter->first] == true) { // we can only compare scalars (don't convert -- ambiguous) 
				if (leftVector->getData(iter->first) == containsData) contains = true;
			}
		}
		
		if (type == 1) contains = !contains;
		if (contains) return "1"; else return "0";
	}
	
	
	/* public */ 
	Contains_Obj::Contains_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		this->isPostFixFunc = true;
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["contains"] = "";
			tempMap["lacks"] = ""; // level alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("$"); // VERB
		tempVec.push_back("%"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["contains"] = "";
		vocab["lacks"] = "";
		
		vocab["left"] = "";
	}
	
	std::string Contains_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["left"].length() > 0 && vocab["left"].at(0) == '%') {
			VariableStorage * vectorStorage = NULL;
			std::string toData = vocab["left"].substr(1,vocab["left"].length()-1);
			
			vectorStorage = dataStructure->vecStringToVector(&toData); // jump to the vector object we want to check (modifies toData to make it the highest level) 
			toData = tools::prepareVectorData(dataStructure, toData); // this is the highest level!  ie)  %topName[index][toData];  or  %toData; 
			
			int type = (vocab["contains"] != ""?0:1);
			if (vectorStorage->type(toData) == 0) { // it's actually a scalar 	
				returnValue = this->containsStringVar(type);
			}
			else { // it is indeed a vector 
				vectorStorage = vectorStorage->getVector(toData);
				if (vectorStorage->type() >= 0) returnValue = this->containsVector(type, vectorStorage); // arrays only 
				else return "";
			}
		} // vectors
		else if (vocab["left"] != "") {
			int type = (vocab["contains"] != ""?0:1);
			returnValue = this->containsStringVar(type);
		} // stringVar
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Exit_Obj : public FuncObj 
	
	/* public */ 
	Exit_Obj::Exit_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Exit"] = "";
			tempMap["Die"] = ""; // alias 
			tempMap["Quit"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Exit"] = "";
			vocab["Die"] = ""; // alias
			vocab["Quit"] = ""; // alias
	}
	
	std::string Exit_Obj::executeCode() {
		exit(0);
		return "";
	}
/// ### ///





// ## //
//class IndexOf_Obj : public FuncObj 
	
	/* private */ 
	std::string IndexOf_Obj::indexOfInStringVar(int type) { // type 0 is indexof, 1 is lastindexof 
		int returnInt;
		std::string findData, varData = tools::prepareVectorData(dataStructure, vocab["in"]);
		
		if (type == 0) {
			findData = tools::prepareVectorData(dataStructure, vocab["IndexOf"]);
			returnInt = varData.find(findData);
		}
		else {
			findData = tools::prepareVectorData(dataStructure, vocab["LastIndexOf"]);
			returnInt = varData.rfind(findData);
		}
		
		if (returnInt == std::string::npos) {returnInt = -1;}
		
		return tools::intToString(returnInt);
	}
	
	std::string IndexOf_Obj::indexOfInVector(int type, VariableStorage * inVector) {
		std::map<std::string, bool, strCmp> * inNodes = inVector->getVectorNodes();
		
		if (type == 0) {
			std::string findData = tools::prepareVectorData(dataStructure, vocab["IndexOf"]);
			std::map<std::string, bool>::const_iterator iter;
			for (iter = inNodes->begin(); iter != inNodes->end(); ++iter) {
				if ((*inNodes)[iter->first] == true) { // we can only compare scalars (don't convert -- ambiguous) 
					if (inVector->getData(iter->first) == findData) return iter->first;
				}
			}
		}
		else {
			std::string findData = tools::prepareVectorData(dataStructure, vocab["LastIndexOf"]);
			std::map<std::string, bool>::reverse_iterator iter;
			for (iter = inNodes->rbegin(); iter != inNodes->rend(); ++iter) {
				if ((*inNodes)[iter->first] == true) { // we can only compare scalars (don't convert -- ambiguous) 
					if (inVector->getData(iter->first) == findData) return iter->first;
				}
			}
		}
		
		return "-1";
	}
	
	
	/* public */ 
	IndexOf_Obj::IndexOf_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["IndexOf"] = "";
			tempMap["LastIndexOf"] = ""; // level alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["in"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("$"); // VERB
		tempVec.push_back("%"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["IndexOf"] = "";
		vocab["LastIndexOf"] = "";
		
		vocab["in"] = "";
	}
	
	std::string IndexOf_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["in"].length() > 0 && vocab["in"].at(0) == '%') {
			VariableStorage * vectorStorage = NULL;
			std::string toData = vocab["in"].substr(1,vocab["in"].length()-1);
			
			vectorStorage = dataStructure->vecStringToVector(&toData); // jump to the vector object we want to check (modifies toData to make it the highest level) 
			toData = tools::prepareVectorData(dataStructure, toData); // this is the highest level!  ie)  %topName[index][toData];  or  %toData; 
			
			int type = (vocab["IndexOf"] != ""?0:1);
			if (vectorStorage->type(toData) == 0) { // it's actually a scalar 	
				returnValue = this->indexOfInStringVar(type);
			}
			else { // it is indeed a vector 
				vectorStorage = vectorStorage->getVector(toData);
				if (vectorStorage->type() >= 0) returnValue = this->indexOfInVector(type, vectorStorage); // arrays only (there is no first and last in a vector) 
				else return "";
			}
		} // vectors
		else if (vocab["in"].length() > 0 && vocab["in"].at(0) == '#') {} // FH
		else if (vocab["in"] != "") {
			int type = (vocab["IndexOf"] != ""?0:1);
			returnValue = this->indexOfInStringVar(type);
		} // stringVar
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class isDefined_Obj : public FuncObj 
	
	/* public */ 
	isDefined_Obj::isDefined_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		this->isPostFixFunc = true;
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["isDefined"] = "";
			tempMap["exists"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		// 2ND PARAMETER 
		tempMap["left"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("$"); // VERB
		tempVec.push_back("%"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["isDefined"] = "";
			vocab["exists"] = ""; // alias
		
		vocab["left"] = "";
	}
	
	std::string isDefined_Obj::executeCode() {
		std::string returnValue = "0";
		
		if (vocab["left"].length() > 0 && vocab["left"].at(0) == '%') {
			std::string thisData = vocab["left"].substr(1,vocab["left"].length()-1);
			VariableStorage * vectorStorage = dataStructure->vecStringToVector(&thisData, false, true); // a don't die version 
			thisData = tools::prepareVectorData(dataStructure, thisData);
			if (vectorStorage != NULL && vectorStorage->variableExists(thisData)) returnValue = "1";
		} // vectors
		else if (vocab["left"].length() > 0 && vocab["left"].at(0) == '#') {} // FH
		else if (vocab["left"] != "") {
			std::string thisData = vocab["left"].substr(1,vocab["left"].length()-1);
			if (dataStructure->variableExists(thisData)) returnValue = "1";
		} // var
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Join_Obj : public FuncObj 
	
	/* public */ 
	Join_Obj::Join_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Join"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["by"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("%"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Join"] = "";
		
		vocab["by"] = "";
	}
	
	std::string Join_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["Join"].length() > 0 && vocab["Join"].at(0) == '%') { // vectors only 
			VariableStorage * vectorStorage = NULL;
			std::string joinData = vocab["Join"].substr(1,vocab["Join"].length()-1);
			
			vectorStorage = dataStructure->vecStringToVector(&joinData); // jump to the vector object we want to check (modifies toData to make it the highest level) 
			joinData = tools::prepareVectorData(dataStructure, joinData); // this is the highest level!  ie)  %topName[index][toData];  or  %toData; 
			
			if (vectorStorage->type(joinData) > 0) { // it's a vector 
				vectorStorage = vectorStorage->getVector(joinData);
				std::map<std::string, bool, strCmp> * inNodes = vectorStorage->getVectorNodes();
				
				std::string juncture = tools::prepareVectorData(dataStructure, vocab["by"]);
				std::map<std::string, bool>::const_iterator iter;
				for (iter = inNodes->begin(); iter != inNodes->end(); ++iter) {
					returnValue += vectorStorage->getData(iter->first) + juncture; // only the values (strip keys for vectors) 
				}
				
				returnValue.erase(returnValue.length()-juncture.length(), juncture.length());
			}
		}
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Local_Obj : public FuncObj 
	
	/* public */ 
	Local_Obj::Local_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Local"] = "";
			tempMap["My"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("%");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("%"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Local"] = "";
			vocab["My"] = ""; // alias
	}
	
	std::string Local_Obj::executeCode() {
		std::string tokenQualifier = "";
		
		if (vocab["My"] != "") {vocab["Local"] = vocab["My"];}
		
		if (vocab["Local"].at(0) == '$') { // create (localized) (VARIABLE)
			std::string varName = vocab["Local"].substr(1,vocab["Local"].length()-1);
			
			//std::cout << varName <<std::endl;
			
			dataStructure->addVariable(varName,"", -1, true);
			tokenQualifier = "$" + varName;
		}
		else if (vocab["Local"].at(0) == '%') { // create (localized) (VECTOR)
			std::string vecName = vocab["Local"].substr(1,vocab["Local"].length()-1);
			
			dataStructure->addVector(vecName, *(new VariableStorage), -1, true);
			tokenQualifier = "%" + vecName;
		}
		else {std::cout << "WARNING :: Assignation: Unknown Data Type!" <<std::endl;}
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class LowerUpperCase_Obj : public FuncObj 
	
	/* private */ 
	std::string LowerUpperCase_Obj::lowerUpperStringVar() {
		std::string luCaseData = "";
		if (vocab["Lc"] != "") {luCaseData = tools::prepareVectorData(dataStructure, vocab["Lc"]);}
		else if (vocab["Uc"] != "") {luCaseData = tools::prepareVectorData(dataStructure, vocab["Uc"]);}
		
		for(int i=0; i < luCaseData.length(); ++i) {
			if (vocab["Lc"] != "") {luCaseData[i] = tolower(luCaseData[i]);}
			else {luCaseData[i] = toupper(luCaseData[i]);}
		}
		
		return luCaseData;
	}
	
	
	/* public */ 
	LowerUpperCase_Obj::LowerUpperCase_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Lc"] = "";
			tempMap["Lower"] = ""; // alias 
			tempMap["Lowercase"] = ""; // alias 
		tempMap["Uc"] = "";
			tempMap["Upper"] = ""; // alias 
			tempMap["Uppercase"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Lc"] = "";
			vocab["Lower"] = ""; // alias
			vocab["Lowercase"] = ""; // alias
		vocab["Uc"] = "";
			vocab["Upper"] = ""; // alias
			vocab["Uppercase"] = ""; // alias
	}
	
	std::string LowerUpperCase_Obj::executeCode() {
		std::string returnValue = "", typeValue = "";
		
		if (vocab["Lower"] != "") {vocab["Lc"] = vocab["Lower"];}
		else if (vocab["Lowercase"] != "") {vocab["Lc"] = vocab["Lowercase"];}
		else if (vocab["Upper"] != "") {vocab["Uc"] = vocab["Upper"];}
		else if (vocab["Uppercase"] != "") {vocab["Uc"] = vocab["Uppercase"];}
		
		if (vocab["Lc"] != "") {typeValue = vocab["Lc"];}
		else if (vocab["Uc"] != "") {typeValue = vocab["Uc"];}
		
		if (typeValue != "") {returnValue = this->lowerUpperStringVar();} // stringVar
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class LowerUpperCaseFirst_Obj : public FuncObj 
	
	/* private */ 
	std::string LowerUpperCaseFirst_Obj::luCaseFirstStringVar() {
		std::string luCaseData = "";
		
		if (vocab["LcFirst"] != "") {
			luCaseData = tools::prepareVectorData(dataStructure, vocab["LcFirst"]);
			luCaseData[0] = tolower(luCaseData[0]);
		}
		else {
			luCaseData = tools::prepareVectorData(dataStructure, vocab["UcFirst"]);
			luCaseData[0] = toupper(luCaseData[0]);
		}
		
		return luCaseData;
	}
	
	
	/* public */ 
	LowerUpperCaseFirst_Obj::LowerUpperCaseFirst_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["LcFirst"] = "";
			tempMap["UcFirst"] = ""; // level alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["LcFirst"] = "";
		vocab["UcFirst"] = "";
	}
	
	std::string LowerUpperCaseFirst_Obj::executeCode() {
		std::string returnValue = "", typeValue = "";
		
		if (vocab["LcFirst"] != "") {typeValue = vocab["LcFirst"];}
		else if (vocab["UcFirst"] != "") {typeValue = vocab["UcFirst"];}
		
		if (typeValue != "") {returnValue = this->luCaseFirstStringVar();} // stringVar
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Length_Obj : public FuncObj 
	
	/* public */ 
	Length_Obj::Length_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["LengthOf"] = "";
			tempMap["Length"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("%"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["LengthOf"] = "";
			vocab["Length"] = ""; // alias
	}
	
	std::string Length_Obj::executeCode() {
		std::string returnValue = "", typeValue = "";

		if (vocab["Length"] != "") {typeValue = vocab["LengthOf"] = vocab["Length"];}
		else if (vocab["LengthOf"] != "") {typeValue = vocab["LengthOf"];}
		
		if (typeValue.length() > 0 && typeValue.at(0) == '%') {
			returnValue = tools::prepareVectorData(dataStructure, typeValue);
		} // VECTOR
		else if (typeValue != "") { // stringVar
			returnValue = tools::intToString(tools::prepareVectorData(dataStructure, typeValue).length());
		}
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Print_Obj : public FuncObj 
	
	/* public */ 
	Print_Obj::Print_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Print"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		
		// 2 DEFINITIONS 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Print"] = "";
	}
	
	std::string Print_Obj::executeCode() {
		if (vocab["Print"] == "") {return "";}
		
		std::string returnValue = tools::prepareVectorData(dataStructure, vocab["Print"]);
		
		std::cout << returnValue; // NOT for debugging (actually prints out) !! 
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");

		return tokenQualifier;
	}
/// ### ///





// ## //
//class Random_Obj : public FuncObj 
	
	/* private */ 
	std::string Random_Obj::randomStringVar() {
		std::ostringstream outs;
		
		if (vocab["Random"] == "" || tools::prepareVectorData(dataStructure, vocab["Random"]) == "") { // [0-1)
			outs << "0.";
			for (int i = 0; i < 12; ++i) {
				outs << tools::randomNumber(0, 9);
			}
		}
		else {
			int returnInt;
			int max = (int) tools::stringToInt(   tools::prepareVectorData(dataStructure, vocab["Random"])   );
			if (max < 0) {returnInt = tools::randomNumber(max,0);}
			else {returnInt = tools::randomNumber(0,max);}
			outs << returnInt;
		}
		
		return outs.str();
	}
	
	
	/* public */ 
	Random_Obj::Random_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Random"] = "";
			tempMap["Rand"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Random"] = "";
			vocab["Rand"] = ""; // alias
	}
	
	std::string Random_Obj::executeCode() {
		std::string returnValue = "", typeValue = "";

		if (vocab["Rand"] != "") {typeValue = vocab["Random"] = vocab["Rand"];}
		else if (vocab["Random"] != "") {typeValue = vocab["Random"];}
		
		if (typeValue.length() > 0 && typeValue.at(0) == '%') {} // VECTOR
		else {returnValue = this->randomStringVar();} // stringVar/NULL
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Remove_Obj : public FuncObj 
	
	/* private */ 
	std::string Remove_Obj::deleteVar() {
		std::string deleteData = vocab["Remove"];
		deleteData = deleteData.substr(1,deleteData.length()-1);
		
		if (dataStructure->removeVariable(deleteData)) {return "1";}
		else {return "0";}
	}
	
	std::string Remove_Obj::deleteVec() {
		std::string deleteData = vocab["Remove"];
		deleteData = deleteData.substr(1,deleteData.length()-1);
		VariableStorage * vector = NULL;
		
		vector = dataStructure->vecStringToVector(&deleteData);
		deleteData = tools::prepareVectorData(dataStructure, deleteData); 
		
		if (vector->removeVariable(deleteData)) {return "1";}
		else {return "0";}
	}
	
	
	/* public */ 
	Remove_Obj::Remove_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Remove"] = "";
			tempMap["Delete"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["from"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("%"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Remove"] = "";
			vocab["Delete"] = ""; //alias 
			
		vocab["from"] = "";
	}
	
	std::string Remove_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["Delete"] != "") {vocab["Remove"] = vocab["Delete"];}

		if (vocab["Remove"].length() > 0 && vocab["Remove"].at(0) == '%' && vocab["from"] == "") { // delete a vector 
			returnValue = this->deleteVec();
		}
		else if (vocab["Remove"] != "" && vocab["from"] == "") { // delete a variable 
			returnValue = this->deleteVar();
		}
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Reverse_Obj : public FuncObj 
	
	/* private */ 
	std::string Reverse_Obj::reverseStringVar() {
		std::string reverseData = tools::prepareVectorData(dataStructure, vocab["Reverse"]), reverseDataTmp = reverseData;
		int revDataLen = reverseData.length()-1;
		
		for (int i = 0; i <= revDataLen; ++i) {
			reverseDataTmp[i] = reverseData.at(revDataLen-i);
		}
		
		reverseData = reverseDataTmp;
		
		if (vocab["Reverse"].find("$_STRING_") == std::string::npos) { //destructive
			std::string varData = vocab["Reverse"];
			varData = varData.substr(1,varData.length()-1);
			if(dataStructure->addVariable(varData,reverseData) != true) {return "-1";}
		}
		
		return reverseData;
	}
	
	
	/* public */ 
	Reverse_Obj::Reverse_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Reverse"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Reverse"] = "";
	}
	
	std::string Reverse_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["Reverse"].length() > 0 && vocab["Reverse"].at(0) == '%') {} // VECTOR
		else if (vocab["Reverse"] != "") {returnValue = this->reverseStringVar();} // stringVar
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Round_Obj : public FuncObj 
	
	/* private */ 
	std::string Round_Obj::roundStringVar() {
		int preciseData;
		std::string returnValue = "", roundData = tools::prepareVectorData(dataStructure, vocab["Round"]);
		double doubleRoundData = tools::stringToInt(roundData);
		if (roundData.find(".") == std::string::npos) {return roundData;} // integers can't be rounded further 
		
		if (vocab["to"] != "" && tools::isNumber(tools::prepareVectorData(dataStructure, vocab["to"]))) {
			preciseData = (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["to"]));
		} else {preciseData = 0;}
		if (preciseData == 0) {preciseData=-1;} // behind the decimal, not on it
		
		
		if (roundData.find(".") == 0) { // numbers must have preceding number before decimal (0.0 NOT .0) 
			std::string append = "0";
			roundData = append.append(roundData);
		}
		
		int roundedLoc, nextDigitLoc, decLoc = roundData.find(".");
		
		roundedLoc = (decLoc+preciseData < roundData.length()?decLoc+preciseData:0-1); // the digit location to be rounded (up) 
		
		if (preciseData == -1) {nextDigitLoc = (roundedLoc+2 < roundData.length()?roundedLoc+2:0-1);} // the following digit (after the one we want rounded) taking the . into account 
		else {nextDigitLoc = (roundedLoc+1 < roundData.length()?roundedLoc+1:0-1);}
		
		bool roundUp = false;
		if (roundedLoc != -1 && nextDigitLoc != -1) {
			if ((((int) roundData.at(nextDigitLoc))-48) > 5) {roundUp=true;}
			else if ((((int) roundData.at(nextDigitLoc))-48) == 5) {
				
				bool overFive = false;
				for (int i = nextDigitLoc+1; i < roundData.length(); ++i) { // 4.501 is actually 5
					if ((((int) roundData.at(i))-48) > 0) {overFive = true;break;}
				}
				
				if ((((int) roundData.at(roundedLoc))-48) % 2 != 0 || overFive == true) {roundUp = true;} // odd or over .5 rounds up 
			}
		} // else they want an out-of-bounds precision, return the original number
		
		if (roundUp == true) {
			if (preciseData > 0) {doubleRoundData = doubleRoundData * pow(10,preciseData);}
			doubleRoundData++;
			if (preciseData > 0) {doubleRoundData = doubleRoundData * pow(10,preciseData*-1);}
		}
		
		returnValue = tools::intToString(doubleRoundData);
		
		if (roundData.length() < returnValue.length()) {++decLoc;} // fixes the 9.99 problem
		returnValue = returnValue.substr(0,decLoc+preciseData+1); // chop off to the precision point 
		
		return returnValue;
	}
	
	
	/* public */ 
	Round_Obj::Round_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Round"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["to"] = "";
			tempMap["by"] = ""; // alias 
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Round"] = "";
		
		vocab["to"] = "";
			vocab["by"] = ""; // alias
	}
	
	std::string Round_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["by"] != "") {vocab["to"] = vocab["by"];}
		
		if (vocab["Round"] != "") {returnValue = this->roundStringVar();} // stringVar
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Select_Obj : public FuncObj  // non-destructive
	
	/* private */ 
	std::string Select_Obj::selectFromStringVar() {
		std::string returnValue = "", varData = tools::prepareVectorData(dataStructure, vocab["from"]);
		
		if (vocab["Select"].find(',') != std::string::npos) { // startLen (later make this a VECTOR) 
			int start = (int) tools::stringToInt(  tools::prepareVectorData(dataStructure, vocab["Select"].substr(0,  vocab["Select"].find(',')  ))  );
				if (start < 0) start += varData.length(); // from the end (-1 is the last char) 
			int length = (int) tools::stringToInt(  tools::prepareVectorData(dataStructure, vocab["Select"].substr(vocab["Select"].find(',')+1,vocab["Select"].length()-1))  );
			if (start+length <= varData.length()) {returnValue = varData.substr(start,length);} // don't overflow the string! 
			if (SHOW_DEBUGGING) std::cout << "     start: " << start << " length: " << length << " return: " << returnValue <<std::endl;
		}
		else { // integer (charat)
			int start = (int) tools::stringToInt(tools::prepareVectorData(dataStructure, vocab["Select"]));
			if (start < 0) start += varData.length(); // from the end (-1 is the last char) 
			if (start < varData.length()) {returnValue = varData.substr(start,1);} // don't overflow the string! 
			if (SHOW_DEBUGGING) std::cout << "     start: " << start << " return: " << returnValue <<std::endl;
		}
		
		return returnValue;
	}
		
	
	/* public */ 
	Select_Obj::Select_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Select"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["from"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Select"] = "";
		
		vocab["from"] = "";
	}
	
	std::string Select_Obj::executeCode() {
		std::string returnValue = "";
		
		if (vocab["from"].length() > 0 && vocab["from"].at(0) == '%') {} // VECTOR
		else if (vocab["from"] != "") {returnValue = this->selectFromStringVar();} // stringVar
				
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		
		return tokenQualifier;
	}
/// ### ///





// ## //
//class Split_Obj : public FuncObj 
	
	/* public */ 
	Split_Obj::Split_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Split"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		// 2ND PARAMETER 
		tempMap["by"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 1 DEFINITION 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
		tempVec.push_back("$"); // PARA2
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Split"] = "";
		
		vocab["by"] = "";
	}
	
	std::string Split_Obj::executeCode() {
		
		return "<SPRESULTS>";
	}
/// ### ///





// ## //
//class ValueOf_Obj : public FuncObj  // used internally for grouping (5+3) => valueof(5+3)
	
	/* public */ 
	ValueOf_Obj::ValueOf_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["ValueOf"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(false);
		
		
		// 3 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		this->returnType.push_back("%");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		// PARTYPE DEF #2
		tempVec.push_back("%"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["ValueOf"] = "";
	}
	
	std::string ValueOf_Obj::executeCode() {
		return vocab["ValueOf"];
	}
/// ### ///





// ## //
//class Void_Obj : public FuncObj 
	
	/* public */ 
	Void_Obj::Void_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
		// VERB DECLARATION 
		std::map<std::string, std::string> tempMap;
		tempMap["Void"] = "";
		this->vocabulary.push_back( tempMap );
		tempMap.clear();
		this->isOptional.push_back(true);
		
		
		// 2 DEFINITIONS IN ONE CLASS 
		this->returnType.push_back("$");
		
		// PARTYPE DEF #1
		std::vector<std::string> tempVec;
		tempVec.push_back("$"); // VERB
			this->paramType.push_back(tempVec);
		tempVec.clear();
		
		
		vocab["Void"] = "";
	}
	
	std::string Void_Obj::executeCode() {
		return "";
	}
/// ### ///
/// ################################ ///
