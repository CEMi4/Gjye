#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>



//################ GLOBAL VARIABLES ################//
bool SHOW_DEBUGGING = false; // default no 
/// ################################ ///





//################ IMPORT LIBRARIES ################//
#include "gjye++.h"
#include "blockWrappers.h"
#include "enviroWrap.h"
#include "objMethods.h"
#include "execTokenStruct.h"
/// ################################ ///





//################ MAIN DRIVER ################//
int main(int argc, char * argv[]) {
	srand((unsigned) time(0)); // for random generation
	
	
	// BUILD THE STANDARD LIBRARY //
	EnviroWrap * eStack = new EnviroWrap();
	FuncObj * tempObj = NULL;
	
	// for each object in the STDL 
	eStack->methodStructure.pushStorage(); // add to the empty stack 
	std::string fullSTDLibrary[] = { "Add", "CharAt", "Exit", "IndexOf", "Join", "Local", "Lc", "LcFirst", "Length", "Print", "Random", "Remove", "Reverse", "Round", "Select", "Split", "ValueOf", "contains", "beginsWith", "isDefined", "Void",        "0x0" };
	for (int t = 0; fullSTDLibrary[t] != "0x0"; ++t) {
		tempObj = exec::instantSTDL(&fullSTDLibrary[t]);
		for (unsigned int i = 0; i < tempObj->returnType.size(); ++i) { // multiple types are combined in one, so loop through all possible combos (eg  Add $ to $, Add % to $, etc) 
			eStack->methodStructure.addMethod(tempObj->returnType[i], "", tempObj->vocabulary, tempObj->paramType[i], tempObj->isOptional, tempObj->isSTDL, tempObj->isPostFixFunc);
		}
		delete tempObj;
	}
	
	//eStack->methodStructure.toString();
	
	/* TEST 
	std::vector<std::string> tVecVocab;
	tVecVocab.push_back("Add");
	tVecVocab.push_back("in");
	tVecVocab.push_back("after");

	std::vector<std::string> tVecTypes;
	tVecTypes.push_back("$");
	tVecTypes.push_back("%");
	tVecTypes.push_back("%");
	
	eStack->methodStructure.findMatch(tVecVocab, tVecTypes);
	*/
	///
	
	
	// parse flags 
	char * fileName = NULL;
	for (int i = 1; i < argc; ++i) {
		if (strlen(argv[i]) > 1 && argv[i][0] == '-') { // it's a flag 
			if (argv[i][1] != '-') {
				switch (argv[i][1]) {
					case 'h': 
					case '?': 
							std:: cout << "Usage: gjye++ [options] file ..." << "\nOptions: \n" <<
						"   -h\t\tDisplay this help information\n" <<
						"   -f <file>\tParse <file>\n" <<
						"   -v\t\tVersion\n" <<
						"   -d\t\tShow debugging output\n";
						exit(0);
					case 'd': 
							SHOW_DEBUGGING = true;
							break;
					case 'f': 
							++i;
							fileName = argv[i];
							break;
					case 'v': 
							std::cout << "Gjye++ version Alpha class" <<std::endl;
							exit(0);
				}
			}
		} else  fileName = argv[i]; // not a flag, so it should be the file name 
	}
	/// 
	
	
	if (fileName != NULL && strlen(fileName) > 0) {
		std::ifstream inFile;
		std::string fullUserInput = "", line = "";
		ClassWrap * mainObject = NULL;
		
		inFile.open(fileName, std::ios::in);
		
		if (inFile.is_open()) {
			
			if (SHOW_DEBUGGING) std::cout << "\nProcessing file " << fileName << "!" <<std::endl;
			
			while ( !inFile.eof() ) {
				getline(inFile,line);
				fullUserInput += line;
			}
			
			mainObject = new ClassWrap("Main", fullUserInput, eStack);
			mainObject->executeCode();
			delete mainObject;
			
		} else {std::cout << "Cannot open file " << fileName << "!" <<std::endl;exit(0);}
	}
	else { // command prompt (debugging) 
		std::cout << "cmd> ";
		
		std::string fullUserInput = "";
		ClassWrap * mainObject = NULL;
		while (!getline(std::cin, fullUserInput).fail()) {
			mainObject = new ClassWrap("Main", fullUserInput, eStack);
			mainObject->executeCode();
			delete mainObject;
			std::cout << "\n\ncmd> ";
		}
	}
	
	eStack->methodStructure.clearMemory();
	delete eStack;
	return 0;
}
/// ################################ ///
