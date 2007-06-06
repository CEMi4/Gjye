#ifndef CREATETOKENSTRUCT_H
#define CREATETOKENSTRUCT_H

extern bool SHOW_DEBUGGING;

#include "enviroWrap.h"
#include "varStorage.h"
#include "tokenGroups.h"

//################ PROTOTYPES ################//
namespace create {
	void prepareTokenInput(EnviroWrap *, TokenGroup *, std::string *);
	void blockHandler(TokenGroup *, std::string *);
	void functionHandler(TokenGroup *, std::string *);
	void highPrecedenceHandler(TokenGroup *, std::string *);
	void precedenceHandler(TokenGroup *, std::string *);
	void createTokenStruct(std::string, TokenGroup *);
}
/// ################################ ///

#endif
