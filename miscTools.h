#ifndef MISCTOOLS_H
#define MISCTOOLS_H

#include "varStorage.h"

//################ PROTOTYPES ################//
namespace tools {
	std::string intToString(double);
	double stringToInt(std::string);
	std::string charToString(char);
	int randomNumber(int, int);
	bool isNumber(std::string);
	bool isInteger(std::string);
	
	std::string xToDec(std::string, std::string);
	std::string prepareVectorData(DataStorageStack *, std::string);
	std::string cleanEscapes(std::string);
	std::string taintEscapes(std::string);
}
/// ################################ ///

#endif
