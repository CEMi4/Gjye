#include <sstream>
#include <stdlib.h>
#include <iostream>

#include "tokenGroups.h"
#include "miscTools.h"
#include "gjye++.h"


//################ TOKEN LEVEL ################//
// class TokenLevel  --  stores tokens horizontally

	/* public */
	TokenLevel::TokenLevel() {
		this->tokCount=0;
		this->sealed=false;
	}
	TokenLevel::TokenLevel(int a, bool b) {
		this->tokCount=a;
		this->sealed=b;
	}
/// ################################ ///





//################ TOKEN GROUP ################//
//class TokenGroup -- stores vertical tokens (exploded)
	
	/* public */
	TokenGroup::TokenGroup() {
		this->currLevel = 0;
		this->insideIfBlock = false;
		this->openIfBlock = false;
		TokenLevel * tmp = new TokenLevel;
		this->token.push_back(*tmp); // set up the zeroth one 
		delete tmp;
	}
	
	TokenGroup::TokenGroup(const TokenGroup * tg) {
		this->token = tg->token;
		this->insideIfBlock = tg->insideIfBlock;
		this->openIfBlock = tg->openIfBlock;
		this->currLevel = tg->currLevel;
		this->catalyst = tg->catalyst;
	}
	
	int TokenGroup::isSealed(int level) const {
		if (level < 0) return -1;
		if ((unsigned int) level < numTokens) {return (token.at(level).sealed == true ? 1 : 0);}
		else {return -1;} // overflow 
	}
	
	bool TokenGroup::seal(int level) {
		if (level < 0) return -1;
		if ((unsigned int) level < numTokens) {token.at(level).sealed = true;return true;}
		else {return false;} // overflow 
	}
	
	std::string TokenGroup::setData(std::string data) {
		if (this->currLevel >= numTokens) {std::cout << "CRITERROR :: Overflow: Vertical token overflow!" <<std::endl;exit(1);} // overflow of levels! 
		
		unsigned int tokCnt = token.at(this->currLevel).tokCount;
		if (tokCnt < numTokens) {token.at(this->currLevel).data.push_back(data);}
		else {
			this->seal(this->currLevel); // seal it since it's full 
			while (this->currLevel < numTokens && token.at(this->currLevel).tokCount >= numTokens) { // fix the level problem (until we hit a level that isn't full) 
				++(this->currLevel);
				token.push_back(*new TokenLevel);
			}
			if (token.size() == numTokens && token.at(numTokens-1).tokCount >= numTokens || this->currLevel >= numTokens) { // if the highest level is full (or we've run out of levels) 
				std::cout << "CRITERROR :: Overflow: Horizontal token overflow!" <<std::endl;exit(1);
			} else {return this->setData(data);} // otherwise attempt repair 
		} // attempt to jump to the next level for them! 
		
		++(token.at(this->currLevel).tokCount);
		
		return tools::intToString(this->currLevel) + "¬" + tools::intToString(tokCnt);
	}
	
	std::string TokenGroup::getData(int level, int tokCnt) const {
		if (level < 0 || tokCnt < 0) {
			std::cout << "CRITERROR :: Invalid parameters given!" <<std::endl;
			exit(1);
		}
		if ((unsigned int) level <= this->currLevel && (unsigned int) tokCnt < token.at(level).tokCount) {return token.at(level).data.at(tokCnt);}
		else {
			std::cout << "CRITERROR :: Overflow: TokenGroup::getData() level overflow!" <<std::endl;
			exit(1);
		} // there's an overflow 
	}
/// ################################ ///
