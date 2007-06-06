#ifndef GJYE_H
#define GJYE_H

//################ GLOBAL CONSTANTS ################//
const int numTokens = 128; // store up to 128(*128) tokens
const std::string validTokenChars = "0123456789«»| "; // valids for tokens 
const std::string tokenizers = "$(){}+-*/!=%#[], "; // used to find stops and starts 
const std::string tokenizerStarts = "(+-*/!=%[,"; // used to find starts 
const std::string operators = "=*/%+-><?:";
const std::string validKeyChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_"; // valid names for variables and vectors and functions 
const std::string letterChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; // A-Za-z
const std::string alphaNumChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; // A-Za-z0-9
const std::string numberChars = "0123456789"; // 0-9
const std::string numericalChars = "0123456789.-"; // number matching 
const std::string numericalCharsUnsigned = "0123456789."; // number matching (non-negatives) 
/// ################################ ///

#endif
