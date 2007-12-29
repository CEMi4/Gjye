#ifndef TOKENGROUPS_H
#define TOKENGROUPS_H

#include <vector>


//################ PROTOTYPES ################//
class TokenLevel { // stores tokens horizontally
public:
	std::vector<std::string> data; // store (up to numTokens) tokens on each level 
	unsigned int tokCount;
	bool sealed;
	
	TokenLevel();
	TokenLevel(int, bool);
	
};

class TokenGroup { // stores vertical tokens (exploded)
private: 
	std::vector<TokenLevel> token;
	unsigned int currLevel;
	
public:
	std::string catalyst;
	bool insideIfBlock, openIfBlock;
	
	TokenGroup();
	TokenGroup(const TokenGroup *);
	
	int isSealed(int) const;
	bool seal(int);
	std::string setData(std::string);
	std::string getData(int, int) const;
	
};
/// ################################ ///

#endif
