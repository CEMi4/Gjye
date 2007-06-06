#ifndef TOKENGROUPS_H
#define TOKENGROUPS_H

#include <vector>


//################ PROTOTYPES ################//
class TokenLevel { // stores tokens horizontally
public:
	std::vector<std::string> data; // store (up to numTokens) tokens on each level 
	int tokCount;
	bool sealed;
	
	TokenLevel();
	TokenLevel(int, bool);
	
};

class TokenGroup { // stores vertical tokens (exploded)
private: 
	std::vector<TokenLevel> token;
	int currLevel;
	
public:
	std::string catalyst;
	
	TokenGroup();
	TokenGroup(TokenGroup *);
	
	int isSealed(int);
	bool seal(int);
	std::string setData(std::string);
	std::string getData(int, int);
	
};
/// ################################ ///

#endif
