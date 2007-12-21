#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>

#include "operMethods.h"
#include "miscTools.h"


//################ GJYE OPERATOR METHODS ################//
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
			
			std::map<std::string, InternalDataType *, strCmp> * leftNodes = leftVector->getVectorNodes();
			std::map<std::string, InternalDataType *, strCmp> * rightNodes = rightVector->getVectorNodes();
			
			std::map<std::string, InternalDataType *>::const_iterator iter;
			std::string addName = "";
			for (iter = leftNodes->begin(); iter != leftNodes->end(); ++iter) {
				if (leftVector->type() >= 0) addName = ""; else addName = iter->first;
				if ( ((*leftNodes)[iter->first]->getType() & 3) != 0) dataStructure->getVector(tokenQualifier)->addVector( addName   , *leftVector->getVector(iter->first)    ); // it's a vector type 
				else dataStructure->getVector(tokenQualifier)->addVariable(addName, leftVector->getData(iter->first));
			}
			
			for (iter = rightNodes->begin(); iter != rightNodes->end(); ++iter) {
				if (rightVector->type() >= 0) addName = ""; else addName = iter->first;
				if ( ((*rightNodes)[iter->first]->getType() & 3) != 0) dataStructure->getVector(tokenQualifier)->addVector(    addName, *rightVector->getVector(iter->first)    ); // it's a vector type 
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
//class Unaries_Obj : public FuncObj 
	
	/* public */ 
	Unaries_Obj::Unaries_Obj(DataStorageStack * storage) : FuncObj(storage) {
		
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
	
	std::string Unaries_Obj::executeCode() {
		std::string returnValue = "", unaryType = "", varData = "", vecNameCpy = "";
		VariableStorage * rightVector = NULL;
		bool scalarR = false;
		
		
		if (vocab["right"].length() < 2) 
			std::cout << "WARNING :: Unary: Malformed Operand (len)!" <<std::endl; // the (right) operand isn't there ... 
		else {
			
			if (vocab["right"].at(0) == '+' && vocab["right"].at(1) == '+') {
				unaryType = "preInc";
				vocab["right"] = vocab["right"].substr( 2 );
			}
			else if (vocab["right"].at(0) == '-' && vocab["right"].at(1) == '-') {
				unaryType = "preDec";
				vocab["right"] = vocab["right"].substr( 2 );
			}
			else if (vocab["right"].at(0) == '+') {
				unaryType = "pos";
				vocab["right"] = vocab["right"].substr( 1 );
			}
			else if (vocab["right"].at(0) == '-') {
				unaryType = "neg";
				vocab["right"] = vocab["right"].substr( 1 );
			}
			else if (vocab["right"].at(0) == '!') {
				unaryType = "lNot";
				vocab["right"] = vocab["right"].substr( 1 );
			}
			else if (vocab["right"].at( vocab["right"].length()-1 ) == '+' && vocab["right"].at( vocab["right"].length()-2 ) == '+') {
				unaryType = "postInc";
				vocab["right"] = vocab["right"].substr( 0, vocab["right"].length()-2 );
			}
			else if (vocab["right"].at( vocab["right"].length()-1 ) == '-' && vocab["right"].at( vocab["right"].length()-2 ) == '-') {
				unaryType = "postDec";
				vocab["right"] = vocab["right"].substr( 0, vocab["right"].length()-2 );
			}
			else {std::cout << "WARNING :: Unary: Unknown Operation (NaN)!" <<std::endl;}
			
			
			if (vocab["right"].at(0) == '%') {
				vecNameCpy = vocab["right"].substr(1,vocab["right"].length()-1);
				rightVector = dataStructure->vecStringToVector(&vecNameCpy);
				vecNameCpy = tools::prepareVectorData(dataStructure, vecNameCpy);
				if (rightVector->type(vecNameCpy) == 0) {scalarR = true;}
			}
			
			if ( unaryType != "" && (vocab["right"].at(0) == '$' || scalarR) ) { 
				 varData = tools::prepareVectorData(dataStructure, vocab["right"]);
				 
				if ( tools::isNumber(varData) ) {
					double doubleData;
					
					if ( unaryType == "preInc" ) {
						doubleData =  tools::stringToInt(varData);
						doubleData = doubleData + 1;
						returnValue = tools::intToString(doubleData);
					}
					else if ( unaryType == "preDec" ) {
						doubleData =  tools::stringToInt(varData);
						doubleData = doubleData - 1;
						returnValue = tools::intToString(doubleData);
					}
					else if ( unaryType == "pos" ) {
						doubleData =  tools::stringToInt(varData);
						returnValue = tools::intToString(doubleData);
					}
					else if ( unaryType == "neg" ) {
						doubleData =  tools::stringToInt(varData);
						returnValue = tools::intToString(-doubleData);
					}
					else if ( unaryType == "lNot" ) {
						doubleData =  tools::stringToInt(varData);
						doubleData = (doubleData == 0) ? 1 : 0;
						returnValue = tools::intToString(doubleData);
					}
					else if ( unaryType == "postInc" ) {
						doubleData =  tools::stringToInt(varData);
						returnValue = tools::intToString(doubleData);
						doubleData = doubleData + 1;
					}
					else if ( unaryType == "postDec" ) {
						doubleData =  tools::stringToInt(varData);
						returnValue = tools::intToString(doubleData);
						doubleData = doubleData - 1;
					}
					else {doubleData = 0;} // this shouldn't be possible by now (but compiler will warn us if we don't) 
					
					// update value 
					if ( unaryType != "pos" && unaryType != "neg" && unaryType != "lNot" ) {
						
						if (scalarR)  // vector 
							 rightVector->addVariable( vecNameCpy, tools::intToString(doubleData) );
						else 
							dataStructure->addVariable( vocab["right"].substr(1,vocab["right"].length()-1), tools::intToString(doubleData) );
						
					}
					
				} else {std::cout << "WARNING :: Unary: Unknown Data Value Type!" <<std::endl;}
				
			} else {std::cout << "WARNING :: Unary: Unknown Data Type!" <<std::endl;}
		}
		
		
		// turn return string into a variable 
		std::string tokenQualifier = dataStructure->variableReferencer("_STRING_");
		dataStructure->addVariable(tokenQualifier,returnValue, -1, true);
		tokenQualifier.insert(0,"$");
		return tokenQualifier;
	}
/// ### ///
