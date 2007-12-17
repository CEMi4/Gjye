#ifndef EXECTOKENSTRUCT_H
#define EXECTOKENSTRUCT_H

extern bool SHOW_DEBUGGING;

#include "enviroWrap.h"
#include "varStorage.h"
#include "tokenGroups.h"
#include "objMethods.h"
#include "operMethods.h"

//################ PROTOTYPES ################//
namespace exec {
	bool parseTokID(std::string, int []);
	std::string runVectorStruct(EnviroWrap *, TokenGroup *, std::string = "-1");
	
	FuncObj * instantSTDL(std::string *, std::string * = NULL, EnviroWrap * = NULL);
	std::string runTokenStruct(EnviroWrap *, TokenGroup *, std::string = "-1");
}
/// ################################ ///

#endif
