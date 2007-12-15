#include <sstream>
#include <stdlib.h>
#include <iostream>

#include "createTokenStruct.h"
#include "gjye++.h"
#include "miscTools.h"


namespace create {

//################ PREPARE INPUT (TOKENIZE) ################//
void prepareTokenInput(EnviroWrap * environment, TokenGroup * tGroup, std::string * input) { // store STRINGS as TOKENS, numbers as vars, handle escape sequences
	int oQuot = 0, lQuot = 0, oDQuot = 0, lDQuot = 0; // open quote; last known quote location
	std::string tokID;
	
	*input = tools::taintEscapes(*input); // replace escape sequences
	
	for (int mIndex = input->find_first_of("'\""); mIndex != std::string::npos && oQuot >= 0 && oDQuot>= 0 && mIndex < input->length(); ++mIndex) { // strip out all strings, replacing as tokens 
		
		if (input->at(mIndex) == '"' && oQuot < 1) { // doubleqt block (begin - end)
			if (oDQuot == 0) {
				++oDQuot;
				lDQuot = mIndex;
			} else {
				std::string strData;
				strData = input->substr(lDQuot,mIndex-lDQuot+1);
				
				tokID = "«"+tGroup->setData(strData)+"»"; // TOKENIZE THEM TAINTED!! 
				
				if (SHOW_DEBUGGING) std::cout << "prepareTokenInput:: level " << tokID << ": " << strData <<std::endl; //TMP
				
				input->replace(lDQuot,mIndex-lDQuot+1,tokID);
				
				mIndex = lDQuot+tokID.length(); // adjust for replacement 
				--oDQuot;
			}
		}
		else if (input->at(mIndex) == '\'' && oDQuot < 1) { // singleqt block (begin - end) 
			if (oQuot == 0) {
				++oQuot;
				lQuot = mIndex;
			} else {
				std::string strData;
				strData = input->substr(lQuot,mIndex-lQuot+1);
				
				tokID = "«"+tGroup->setData(strData)+"»"; // TOKENIZE THEM TAINTED!! 
				
				if (SHOW_DEBUGGING) std::cout << "prepareTokenInput:: level " << tokID << ": " << strData <<std::endl; //TMP

				input->replace(lQuot,mIndex-lQuot+1,tokID);
				
				mIndex = lQuot+tokID.length(); // adjust for replacement 
				--oQuot;
			}
		}
		else {} // handle rest -- regex?
		
	}
	
	if (oDQuot != 0) {std::cout << "CRITERROR :: Malformation: DQuot not closed!" <<std::endl;exit(1);} /* ERROR HANDLE */ 
	if (oQuot != 0) {std::cout << "CRITERROR :: Malformation: Quot not closed!" <<std::endl;exit(1);} /* ERROR HANDLE */ 
	
	
	// strip out whitespace //
	std::string stripMatches[] = {"\t","\r","\n","  ", " /", "/ ", " (", "( ", " )", " ,", ", ", " ]", "] ", " {", "{ ", " }", "} "};
	std::string stripReplaces[] = {"","",""," ", "/", "/", "(", "(", ")",",", ",", "]", "]","{","{","}","}"};
	for (int i = 0; i < 17; ++i) {
		while (input->find(stripMatches[i]) != std::string::npos) {
			input->replace(input->find(stripMatches[i]), stripMatches[i].length(), stripReplaces[i]);
		}
	}
	//// 
	
	for (int mIndex = 0; mIndex < input->length(); ++mIndex) { // strip out all numbers, replacing as vars 
		if (isdigit(input->at(mIndex)) > 0) { // numbers (to vars) -- leap forward  ---  || input->at(mIndex) == '.' removed bc we MUST have #.# and not .# 
			std::string strData = "";
			int numberEnd;
			
			if (input->find_last_of(tokenizerStarts+" .",mIndex) != mIndex-1) continue; // it should be tokenized IMMEDIATELY before the number -- >$<_STRING_>000000 (so we don't swallow variables, tokens, etc) 
			
			numberEnd = input->find_first_not_of(numericalCharsUnsigned,mIndex); // unsigned because we don't want to swallow subtraction 
			if (numberEnd == std::string::npos) numberEnd = input->length();
			
			if (numberEnd < input->length() && input->at(numberEnd) == '|') {mIndex = numberEnd;continue;} // don't swallow tokens! 
			
			bool hexCheck = (input->substr(mIndex,numberEnd-mIndex+1) == "0x");
			if (numberEnd < input->length() && (input->at(numberEnd) == 'r' || hexCheck == true)) { // we found a radix definition (NO DECIMAL PLACES!!  10r10.5 -> 10) 
				std::string radix;
				
				if (hexCheck == true) radix = "16";
				else radix = input->substr(mIndex,numberEnd-mIndex);
				
				int tmpmIndex = mIndex;
				mIndex = numberEnd+1;
				
				if (tools::stringToInt(radix) >= 63) {numberEnd = input->find_first_not_of(alphaNumChars+"+/",mIndex);}
				else if (tools::stringToInt(radix) <= 10) {numberEnd = input->find_first_not_of(numberChars,mIndex);}
				else {numberEnd = input->find_first_not_of(alphaNumChars,mIndex);}
				
				std::string number = input->substr(mIndex,numberEnd-mIndex);
				
				if (SHOW_DEBUGGING) std::cout << "radix: " << radix << "   numb: " << number << std::endl;
				
				strData = tools::xToDec(radix, number);
				
				mIndex = tmpmIndex;
			} else {
				while (numberEnd-1 > 0 && input->at(numberEnd-1) == '.') --numberEnd;
				
				strData = input->substr(mIndex,numberEnd-mIndex);
			}
			
			if (tools::isNumber(strData) == true) {
				std::string tokenQualifier = environment->dataStructure.variableReferencer("_STRING_");
				environment->dataStructure.addVariable(tokenQualifier,strData, -1, true);
				tokenQualifier.insert(0,"$");
				input->replace(mIndex,numberEnd-mIndex,tokenQualifier);
			}
		}
	}
	
	*input = tools::cleanEscapes(*input); // get rid of escape identifiers
}
/// ################################ ///





//################ TOKENIZE BLOCKS ################//
void blockHandler(TokenGroup * tGroup, std::string * fullToken) { // tokenizes block declarations, OKAY to call before prepareTokenInput 
	int nBlo = 0, lBlo = 0; // open block; index block
	int oQuot = 0, oDQuot = 0; // open quote
	
	for (int mIndex = fullToken->find_first_of("{}'\""); mIndex != std::string::npos && mIndex < fullToken->length(); ++mIndex) {
		
		if (fullToken->at(mIndex) == '"' && oQuot < 1) {
			(oDQuot == 0 ? oDQuot = 1 : oDQuot = 0);
		}
		else if (fullToken->at(mIndex) == '\'' && oDQuot < 1) {
			(oQuot == 0 ? oQuot = 1 : oQuot = 0);
		}
		else if (fullToken->at(mIndex) == '{' && nBlo == 0 && oDQuot == 0 && oQuot == 0) {++nBlo;lBlo = mIndex;} 
		
		else if (fullToken->at(mIndex) == '{' && oDQuot == 0 && oQuot == 0) {++nBlo;} // only poke the inner blocks! 
		else if (fullToken->at(mIndex) == '}' && nBlo > 1 && oDQuot == 0 && oQuot == 0)  {--nBlo;}
		
		else if (fullToken->at(mIndex) == '}' && nBlo == 1 && oDQuot == 0 && oQuot == 0) { // handle OUTER blocks 
			--nBlo;
			
			std::string blockData = fullToken->substr(lBlo,mIndex-lBlo+1);
			
			std::string tokID = "«" + tGroup->setData(blockData) + "»";
			fullToken->replace(lBlo,mIndex-lBlo+1,tokID);
			
			if (SHOW_DEBUGGING) std::cout << "blockHandler:: level " << tokID << ": " << blockData << " (lBlo: " << lBlo << ") (mIndex: " << mIndex << ") :: block" <<std::endl; //TMP
			
			mIndex = lBlo;
			lBlo = fullToken->rfind("{",lBlo-1);
			if (lBlo == std::string::npos) {lBlo = 0;}
		}
		else if (fullToken->at(mIndex) == '}' && oDQuot == 0 && oQuot == 0) {std::cout << "CRITERROR :: Malformation: Block not open!" <<std::endl;exit(1);} /* ERROR HANDLE */ 
	}
	
	if (nBlo > 0) {std::cout << "CRITERROR :: Malformation: Block not closed! " << nBlo <<std::endl;exit(1);} /* ERROR HANDLE */ 
	
}
/// ################################ ///





//################ TOKENIZE FUNCTIONS ################//
void functionHandler(TokenGroup * tGroup, std::string * fullToken) { // tokenizes function declarations, call prepareTokenInput BEFORE this
	int endFunc = fullToken->find_last_of(validKeyChars), startFunc = 0;
	
	while (endFunc != std::string::npos) {
		if (endFunc > 0) {startFunc = fullToken->find_last_not_of(validKeyChars, endFunc-1);}
		else {startFunc = -1;} // the error handling should catch this below 
		
		if ((startFunc == std::string::npos || fullToken->at(startFunc) != '$' && fullToken->at(startFunc) != '%') && tools::isInteger(fullToken->substr(startFunc+1,endFunc-startFunc)) == false) { // is there any other acceptable instance? 
			++startFunc;
			
			std::string tokID = "", funcName = fullToken->substr(startFunc,endFunc-startFunc+1);
			
			int grabTokenCount = (funcName == "If" || funcName == "ElseIf" ? 2 : 1); // only If and ElseIf take 2 tokens 
			
			do { // grab the term to the right of the function (and keep grabbing if there's a comma (eg  Func $a, $b  =>   Func($a, $b)  ) 
				
				if (endFunc+1 < fullToken->length() && fullToken->at(endFunc+1) == ' ') ++endFunc; // we allow a space, but ignore it (jump over it) 
				
				if (endFunc+1 < fullToken->length() && fullToken->at(endFunc+1) == '«' || fullToken->at(endFunc) == '«') { // it's a token 
					endFunc = fullToken->find('»',endFunc+1); // NOT GREEDY (only take one token) 
					if (endFunc == std::string::npos) {std::cout << "CRITERROR :: Malformation: Function format (no end-of-token found)!" <<std::endl;exit(1);}
					++endFunc; // include the » (for the subtraction later) 
				} 
				else if (endFunc+1 < fullToken->length() && (fullToken->at(endFunc+1) == '$' || fullToken->at(endFunc+1) == '%')) { // it's a variable or vector $/% 
					endFunc = fullToken->find_first_not_of(validKeyChars,endFunc+3); // 3 because we have func $var (with a space) 
				}
				else { // no parameter(s) given 
					++endFunc;
				}
				
				--grabTokenCount;
			} while (endFunc < fullToken->length() && grabTokenCount > 0);
			
			if (endFunc == std::string::npos) {endFunc = fullToken->length();}
			
			
			std::string subToken= fullToken->substr(startFunc,endFunc-startFunc);
			if (subToken.length() <= 0) {
					continue;
					endFunc = fullToken->find_last_of(validKeyChars, startFunc-1);
			}
			
			tokID = "«" + tGroup->setData(subToken) + "»";
			
			fullToken->replace(startFunc,endFunc-startFunc,tokID);
			
			if (SHOW_DEBUGGING) std::cout << "functionHandler:: level " << tokID << ": " << subToken << " (startFunc: " << startFunc << ") (endFunc: " << endFunc << ") :: tName" <<std::endl; //TMP
			
			endFunc = fullToken->find_last_of(validKeyChars, startFunc-1);
			
		} else {
			if (startFunc == std::string::npos || startFunc == 0) {endFunc = -1;}
			else {endFunc = fullToken->find_last_of(validKeyChars, startFunc-1);}
		}
	}
	
}
/// ################################ ///





//################ TOKENIZE HIGHER PRECEDENCIES (called by precedenceHandler) ################//
void highPrecedenceHandler(TokenGroup * tGroup, std::string * fullToken) { // handles level 1 and 2 precedencies ( ++, --, !, ++, --, -, +), call prepareTokenInput BEFORE this
	// note: the highs only have one operand! 
	
	
	
}
/// ################################ ///





//################ TOKENIZE OTHER PRECEDENCIES ################//
void precedenceHandler(TokenGroup * tGroup, std::string * fullToken) { // handle (lower) operators, we should have only tokens here (call prepareTokenInput BEFORE this) 
	
	highPrecedenceHandler(tGroup, fullToken); // the highs are handled separately! (because i said so) 
	
	std::string operatorsOrdered[] = {"*/%", "+-", "<>", "!=", "="}; // in order of precedence  ... eventually this is ::       "*/%","+-",           "< <= > >=","== !=","and &&","or ||","?:",             "=" 
	
	for (int i = 0; i < 5; ++i) { // the five (so far) orders of precedence 
		
		std::string tokID;
		
		int operatorPivot, operatorPivot2, forceJump = 0;
		if (i == 4) forceJump = fullToken->length()-1;
		while ((operatorPivot = (i == 4?fullToken->find_last_of(operatorsOrdered[i], forceJump):fullToken->find_first_of(operatorsOrdered[i], forceJump))) != std::string::npos) { // lToR vs rToL
			int beginPos = 0, termPos = 0;
			
			if ( i == 3 && fullToken->at(operatorPivot+1) != '=' ) {
				forceJump = operatorPivot+2;
				continue;
			}
			
			// grab the left side -- beginPos 
			int operatorPivotTemp = operatorPivot;
			if (operatorPivotTemp-1 >= 0 && fullToken->at(operatorPivotTemp-1) == ' ') --operatorPivotTemp; // allow for leading spaces, but ignore
			
			if (operatorPivotTemp-1 >= 0 && fullToken->at(operatorPivotTemp-1) == '»') {
				beginPos = fullToken->find_last_not_of(validTokenChars,operatorPivotTemp-1)+1; // token -- greedy (take all tokens before it!)
			} 
			else if (operatorPivotTemp-1 >= 0) {
				beginPos = fullToken->find_last_not_of(validKeyChars,operatorPivotTemp-1);
				if (beginPos != std::string::npos && fullToken->at(beginPos) != '$' && fullToken->at(beginPos) != '%') {++beginPos;} // don't snag unless it declares variable type
			}
			
			if (beginPos == std::string::npos) beginPos = 0;
			if (fullToken->at(beginPos) == ' ') ++beginPos; // don't swallow any extra spaces! 
			
			if (fullToken->at(operatorPivot) == '%' && operatorPivot+1 < fullToken->length() && (isalpha(fullToken->at(operatorPivot+1)) || fullToken->at(operatorPivot+1) == '_')  ) { // allow for vectors --  %a.  this allows skipping of (false) operators (lToR only) 
				forceJump=operatorPivot+1;
				continue;
			}
			
			if ((operatorPivotTemp-beginPos) <= 1 && fullToken->at(operatorPivot) != '-' && fullToken->at(operatorPivot) != '+' || operatorPivot+1 >= fullToken->length()) { // ERROR HANDLE 
				std::cout << "CRITERROR :: Malformation: Operator format (left)!" <<std::endl;
				exit(1);
			}
			
			
			
			// grab the right side -- termPos 
			operatorPivot2 = operatorPivot;
			if ( (fullToken->at(operatorPivot) == '<' || fullToken->at(operatorPivot) == '>' || fullToken->at(operatorPivot) == '!' || fullToken->at(operatorPivot) == '=') 
				&& operatorPivot+1 < fullToken->length() && fullToken->at(operatorPivot+1) == '=') 
					++operatorPivot2; // support >= <= != == 
			
			operatorPivotTemp = operatorPivot2;
			if (operatorPivotTemp+1 < fullToken->length() && fullToken->at(operatorPivotTemp+1) == ' ') ++operatorPivotTemp; // allow for trailing spaces, but ignore 
			
			if (operatorPivotTemp+1 < fullToken->length() && fullToken->at(operatorPivotTemp+1) == '«') {
				termPos = fullToken->find_first_not_of(validTokenChars,operatorPivotTemp+1); // token -- greedy (take all tokens after it!) +1 
			} 
			else if (operatorPivotTemp+2 < fullToken->length() && (fullToken->at(operatorPivotTemp+1) == '$' || fullToken->at(operatorPivotTemp+1) == '%')) {
				termPos = fullToken->find_first_not_of(validKeyChars,operatorPivotTemp+2); // 2 because operators are not of validKeyChars +1 
			}
			else if (operatorPivotTemp+1 < fullToken->length() &&  isalpha(fullToken->at(operatorPivotTemp+1)) ) { // presumably there is a function to the right, so skip it this time 
				(i == 4 ? forceJump=operatorPivot-1 : forceJump=operatorPivot+1);
				continue;
			}
			else {std::cout << "CRITERROR :: Malformation: Operator format (right)!" <<std::endl;exit(1); /* ERROR HANDLE */ }
			
			if (termPos == std::string::npos) {termPos = fullToken->length();}
			if (fullToken->at(termPos-1) == ' ') --termPos; // don't swallow any extra spaces! 
			
			
			
			std::string subToken = fullToken->substr(beginPos,termPos-beginPos);
			
			// strip out surrounding whitespace //
			int replSize = 2;
			std::string rmvRepl = "" + tools::charToString(fullToken->at(operatorPivot));
			std::string rmvRepl2 = "" + tools::charToString(fullToken->at(operatorPivot2));
			std::string stripMatches[] = {" " + tools::charToString(fullToken->at(operatorPivot)), tools::charToString(fullToken->at(operatorPivot)) + " ", 
									" " + tools::charToString(fullToken->at(operatorPivot2)), tools::charToString(fullToken->at(operatorPivot2)) + " "};
			std::string stripReplaces[] = {rmvRepl, rmvRepl, rmvRepl2, rmvRepl2};
			
			if (operatorPivot != operatorPivot2) 
				replSize = 4;
			
			for (int i = 0; i < replSize; ++i) {
				while (subToken.find(stripMatches[i]) != std::string::npos) {
					subToken.replace(subToken.find(stripMatches[i]), 2, stripReplaces[i]);
				}
			}
			//// 
			
			tokID = "«" + tGroup->setData(subToken) + "»";
			
			fullToken->replace(beginPos,termPos-beginPos,tokID);
			
			if (SHOW_DEBUGGING) std::cout << "precedenceHandler:: level " << tokID << ": " << subToken <<std::endl; //TMP
			
		}
		
	}
}
/// ################################ ///





//################ TOKENIZATION DRIVER (PARENS/VECS) ################//
void createTokenStruct(std::string input, TokenGroup * tGroup) { // one line at a time -- handles parentheses, functions, and vector decs 
	int nPar = 0, lPar = 0, nBrac = 0, lBrac = 0; // open parens; index parens; last known paren location; highest level (locally-global); Brac maps with Par functions 
	
	if (SHOW_DEBUGGING) std::cout << "\ncreateTokenStruct:: " << input <<std::endl; //TMP
	
	// we already called this before the commands were broken up
	//prepareTokenInput(dataStructure, tGroup, &input); // store strings as tokens, numbers as vars, handle escape sequences (otherwise they can cause problems here) 
	
	for (int mIndex = input.find_first_of("([])"); mIndex != std::string::npos && mIndex < input.length(); ++mIndex) {
		
		if (input.at(mIndex) == '(') {++nPar;lPar = mIndex;} 
		else if (input.at(mIndex) == '[') {++nBrac;lBrac = mIndex;}
		
		else if (input.at(mIndex) == ')' && nPar > 0) { // handle parenthesis (highest precedence) -- $x=(5+4) for instance
			--nPar;
			
			std::string fullToken = input.substr(lPar,mIndex-lPar+1);
			
			precedenceHandler(tGroup, &fullToken); // tokenize enclosed operators 
			functionHandler(tGroup, &fullToken); // tokenize enclosed functions  (floating precedence) 
			precedenceHandler(tGroup, &fullToken); // tokenize enclosed operators
			
			std::string tokID = "«" + tGroup->setData(fullToken) + "»";
			input.replace(lPar,mIndex-lPar+1,tokID);
			
			if (SHOW_DEBUGGING) std::cout << "createTokenStruct:: level " << tokID << ": " << fullToken << " (lPar: " << lPar << ") (mIndex: " << mIndex << ") :: parens" <<std::endl; //TMP
			
			mIndex = lPar;
			lPar = input.rfind("(",lPar-1);
			if (lPar == std::string::npos) {lPar = 0;}
		}
		else if (input.at(mIndex) == ']' && nBrac > 0) { // handle bracket group (high precedence) -- %x=[5,4] for instance
			--nBrac;
			
			while (mIndex < input.length()-1 && input.at(mIndex+1) == '[') { // find the last of the string %vec[1][2][3]< 
				mIndex = input.find(']', mIndex+1);
			}
			
			int lOccur = lBrac; // the beginning of our block
			if (lOccur > 0 && isalnum(input.at(lOccur-1))) { 
				lOccur = input.find_last_not_of(validKeyChars,lOccur-1); // find the REAL beginning >%vec[1][2][3] 
				std::cout << input << " : " << lOccur <<std::endl;
			}
			
			std::string fullToken = input.substr(lOccur,mIndex-lOccur+1);
			
			precedenceHandler(tGroup, &fullToken); // tokenize enclosed operators 
			functionHandler(tGroup, &fullToken); // tokenize enclosed functions (floating precedence) 
			precedenceHandler(tGroup, &fullToken); // tokenize enclosed operators
			
			std::string tokID = "«" + tGroup->setData(fullToken) + "»";
			input.replace(lOccur,mIndex-lOccur+1,tokID);
			
			if (SHOW_DEBUGGING) std::cout << "createTokenStruct:: level " << tokID << ": " << fullToken << " (lOccur: " << lOccur << ") (mIndex: " << mIndex << ") :: brak" <<std::endl; //TMP
			
			mIndex = lBrac;
			if (lOccur > 0 && input.rfind("[",lOccur-1) != std::string::npos) {lOccur = input.rfind("[",lOccur-1)+1;}
			else if (lOccur > 0 && input.rfind("(",lOccur-1) != std::string::npos) {lOccur = input.rfind("(",lOccur-1)+1;}
			else {lOccur=1;}
			lBrac = lOccur-1;
		}
		else if (input.at(mIndex) == ')') {std::cout << "CRITERROR :: Malformation: Parenthesis not open!" <<std::endl;exit(1);} /* ERROR HANDLE */ 
		else if (input.at(mIndex) == ']') {std::cout << "CRITERROR :: Malformation: Bracket not open!" <<std::endl;exit(1);} /* ERROR HANDLE */ 
	}
	
	if (nPar > 0) {std::cout << "CRITERROR :: Malformation: Parenthesis not closed! " << nPar <<std::endl;exit(1);} /* ERROR HANDLE */ 
	if (nBrac > 0) {std::cout << "CRITERROR :: Malformation: Bracket not closed! " << nBrac <<std::endl;exit(1);} /* ERROR HANDLE */ 
	
	
	// finish up stripping out white space // 
	while (input.find("« ") != std::string::npos) {input.replace(input.find("« "), 2, "«");}
	while (input.find(" «") != std::string::npos) {input.replace(input.find(" «"), 2, "«");}
	////
	
	precedenceHandler(tGroup, &input); // tokenize enclosed operators (null level) 
	functionHandler(tGroup, &input); // tokenize enclosed functions (null level)  (floating precedence) 
	precedenceHandler(tGroup, &input); // tokenize leftover operators (null level) 
	
	tGroup->catalyst = input;
}
/// ################################ ///

}
