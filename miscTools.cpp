#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <math.h>

#include "miscTools.h"
#include "gjye++.h"


namespace tools {

//################ VARIOUS TOOLS ################//
std::string intToString(double intVal) {
	std::string thisName;
	std::ostringstream outs;
	outs.precision(16);
	outs << intVal;
	thisName = outs.str();
	return thisName;
}

double stringToInt(std::string strVal) {
	return atof(strVal.c_str());
}

std::string charToString(char charval) {
	std::string thisName;
	std::ostringstream outs;
	outs << charval;
	thisName = outs.str();
	return thisName;
}


int randomNumber(int min, int max) { // generate a random number from min to max
	int randNum = (rand() % (max-min+1))+min;
	return randNum;
}


bool isNumber(std::string streamData) { // make this stronger later on (0-0-.3325.235 would pass!)
	if (streamData != "" && streamData != "." && streamData.find_first_not_of(numericalChars) == std::string::npos) {return true;}
	else {return false;}
}

bool isInteger(std::string streamData) {
	if (streamData.length() > 0 && streamData.find_first_not_of(numberChars) == std::string::npos) {return true;}
	else {return false;}
}


static char base37To64[]  = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'};
std::string xToDec(std::string radix, std::string operand) {
	if (radix == "" || operand == "") {return "-1";} // wtf!? 
	
	int base = (int) stringToInt(radix);
	
	if (base <= 36 && base >= 2) {
		return intToString(strtol(operand.c_str(), NULL, base));
	} else if (base > 36 && base <= 64) {
		std::string finalOperand = "";
		int intOperand = 0;
		
		int baseCnt = 0;
		for (int i = operand.length()-1; i >= 0; --i) {
			int j = 0; for (; j < base; ++j) 
				if (base37To64[j] == operand.at(i)) break;
			if (j == base) return "-1"; // invalid character for this base! 
			
			intOperand += (int) pow(base, baseCnt)*j;
			++baseCnt;
		}
		
		finalOperand = intToString(intOperand);
		while (finalOperand.length() > 1 && finalOperand.at(0) == '0') finalOperand.erase(0,1);
		return finalOperand;
	}
	
	return "-1"; // error!! 
}


std::string prepareVectorData(DataStorageStack * dataStructure, std::string thisData, bool purgeTransient) { // parse datum before using it ... includes "" and '' wrappers!!  note that %vec[a] would be tokenized, so only bottom levels can exist here! 
	if (thisData == "") {return "";} // otherwise .at(0) kills the program 
	
	bool isDQ = (thisData.at(0) == '\''?false:true); // anything but single quote is considered double
	
	if (thisData.at(0) == '"' || thisData.at(0) == '\'') {
		thisData.replace(0,1,"");
		thisData.replace(thisData.length()-1,1,"");
	}
	
	if (isDQ == true) { // no quotes implies double quotes ... eg. select($a)  -->  select("$a")
		int lIndex = 0, mIndex = thisData.find_first_of("$%");
		std::string varData;
		
		while (mIndex != std::string::npos) { // replace variables with values
			varData = "";	
			
			if (thisData.at(mIndex) == '%') { // vectors 
				VariableStorage * vectorStorage = NULL;
				
				lIndex = thisData.find_first_not_of(validKeyChars,mIndex+1);
				if (lIndex == std::string::npos) {lIndex = thisData.length()-1;} else {--lIndex;}
				
				if (lIndex+1 < thisData.length() && thisData.at(lIndex+1) == '[') { // find the end of the vector reference if it's not a simple vector ... %vec[%vec[x]][y]< 
					int oBrac = 0, nIndex = lIndex+1; // number brackets open, current index 
					while (nIndex < thisData.length() && (oBrac > 0 || thisData.at(nIndex) == '[')) {
						if (thisData.at(nIndex) == '[') {++oBrac;}
						else if (thisData.at(nIndex) == ']') {--oBrac;}
						++nIndex;
					}
					lIndex = nIndex;
				}
				
				std::string vecName = thisData.substr(mIndex+1,lIndex-mIndex);
				
				vectorStorage = dataStructure->vecStringToVector(&vecName); // jump to the vector object we want to modify (modifies vecName to make it the highest level) 
				vecName = tools::prepareVectorData(dataStructure, vecName); // this is the highest level!  ie)  %topName[index][vecName];  or  %vecName; 
				varData = vectorStorage->getData(vecName, purgeTransient);
			} else { // variables 
				lIndex = thisData.find_first_not_of(validKeyChars,mIndex+1);
				if (lIndex == std::string::npos) {lIndex = thisData.length()-1;} else {--lIndex;}
				std::string varName = thisData.substr(mIndex+1,lIndex-mIndex);
				varData = dataStructure->getData(varName, purgeTransient);
			}
			
			thisData.replace(mIndex,lIndex-mIndex+1,varData);
			mIndex = thisData.find_first_of("$%",mIndex+varData.length());
		}
	} else {} // single quote / regex? 
	
	if (thisData.find("«{±") != std::string::npos) {thisData = tools::cleanEscapes(thisData);} // remove escape identifiers
	return thisData;
}


std::string cleanEscapes(std::string thisData) { // replace escape identifiers with backslash replacement
	while (thisData.find("«{±") != std::string::npos) {
		std::string tokID;
		int sIndex=0,eIndex=0;
		
		sIndex = (thisData.find("«{±"));
		eIndex = (thisData.find("}»"));
		tokID = thisData.substr(sIndex+3,eIndex-(sIndex+3));
		
		if (tokID == "BKSLH") {thisData.replace(sIndex,eIndex-sIndex+2,"\\");}
		else if (tokID == "SQUOT") {thisData.replace(sIndex,eIndex-sIndex+2,"'");}
		else if (tokID == "DQUOT") {thisData.replace(sIndex,eIndex-sIndex+2,"\"");}
		else if (tokID == "VARSG") {thisData.replace(sIndex,eIndex-sIndex+2,"$");}
		else if (tokID == "VECSG") {thisData.replace(sIndex,eIndex-sIndex+2,"%");}
		else if (tokID == "FHLSG") {thisData.replace(sIndex,eIndex-sIndex+2,"#");}
		else if (tokID == "OBJSG") {thisData.replace(sIndex,eIndex-sIndex+2,"*");}
	}  
	return thisData;
}


std::string taintEscapes(std::string input) { // replace backslash with escape identifiers
	for (int mIndex = 0; mIndex < input.length(); ++mIndex) { // swap out escapes with identifiers -- corresponds with cleanEscapes(string)
		if (input.at(mIndex) == '\\') {
			switch (input.at(mIndex+1)) {
				case '\\': input.replace(mIndex,2,"«{±BKSLH}»");break;
				case '\'': input.replace(mIndex,2,"«{±SQUOT}»");break;
				case '"': input.replace(mIndex,2,"«{±DQUOT}»");break;
				case '$': input.replace(mIndex,2,"«{±VARSG}»");break;
				case '%': input.replace(mIndex,2,"«{±VECSG}»");break;
				case '#': input.replace(mIndex,2,"«{±FHLSG}»");break;
				case '*': input.replace(mIndex,2,"«{±OBJSG}»");break;
				
				// below are committed taintings, which never get converted back 
				case 'n': input.replace(mIndex,2,"\n");break;
				case 'r': input.replace(mIndex,2,"\r");break;
				case 't': input.replace(mIndex,2,"\t");break;
				//default: input.replace(mIndex,1,""); if (mIndex > 0) {--mIndex;}
			}
		}
	}
	return input;
}
/// ################################ ///

}
