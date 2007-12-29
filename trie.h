#ifndef TRIE_H
#define TRIE_H

extern bool SHOW_DEBUGGING;

/*
*	Note: since this runs in O(k) time (worst case, k = variable/key length), it is only useful when it beats R&B trees
*	this means there must be > 1000 variables (log1024 = 10)  -OR-
*	the avg. variable length <= 7 && > 100 variables  -OR-
*	the avg. variable legnth < 4
*	and we don't care about space
*/

namespace gstl {


//################ Hashing algorithm ################//
int hash(const char &);
/// ################################ ///



//################ Patricia Trie ################//
template <class oType>
class trie {
private: 
	trie<oType> ** blackptrs;
	
public: 
	std::string substr;
	oType * whiteptr;
	int numChildren;
	
	trie() {
		std::cout << "CREATE" <<std::endl;
		this->blackptrs = new trie<oType>*[63];
		this->whiteptr = NULL;
		this->substr = "";
		for (int i = 0; i < 63; ++i) {
			this->blackptrs[i] = NULL;
		}
		this->numChildren = 0;
	}
	trie( const trie<oType> & cpTrie ) {
		std::cout << "COPY" <<std::endl;
		for (int i = 0; i < 63; ++i) {
			if (cpTrie.blackptrs[i] != NULL) this->blackptrs[i] = new trie<oType>( *cpTrie.blackptrs[i] );
		}
		this->substr = cpTrie.substr;
		if (cpTrie.whiteptr != NULL) this->whiteptr = new oType( *cpTrie.whiteptr );
		this->numChildren = cpTrie.numChildren;
	}
	trie<oType>& operator=( const trie<oType> & cpTrie ) {
		std::cout << "CPYASSGN" <<std::endl;
		if (this != &cpTrie) {
			this->~trie();
			for (int i = 0; i < 63; ++i) {
				if (cpTrie.blackptrs[i] != NULL) this->blackptrs[i] = new trie<oType>( *cpTrie.blackptrs[i] );
			}
			this->substr = cpTrie.substr;
			if (cpTrie.whiteptr != NULL) this->whiteptr = new oType( *cpTrie.whiteptr );
			this->numChildren = cpTrie.numChildren;
		}
		return *this;
	}
	~trie() {
		std::cout << "DESTROY" <<std::endl;
		delete this->whiteptr;
		for (int i = 0; i < 63; ++i) {
			if (this->blackptrs[i] != NULL) {
				delete this->blackptrs[i];
				this->blackptrs[i] = NULL;
			}
		}
	}
	
	
	
	oType& find( std::string key ) {
		trie<oType> * basePtr = NULL;
		std::string * prefix;
		unsigned int hashIndex, i;
		
		while (true) { // iterative soln (no recursive overhead) 
			if (key.length() == 0) return *(new oType); // memory leak ... wtf would you do this for (will be fixed with iterators) 
			prefix = NULL;
			if (basePtr == NULL) basePtr = this;
			
			hashIndex = hash( key.at(0) );
			if (basePtr != NULL) prefix = &basePtr->blackptrs[hashIndex]->substr;
			else return *(new oType);
			
			if (prefix->length() == 0) return *(new oType);
			
			for (i = 1; i < key.length() && i < prefix->length(); ++i) { // we already know the first char matches 
				if ( key.at(i) != prefix->at(i) ) break;
			}
			
			if ( i == prefix->length() ) { // key is a subset of the prefix (note: i != key.length() or we would've hit above) 
				if (key.length() == i && basePtr->blackptrs[hashIndex]->whiteptr != NULL) {
					return *basePtr->blackptrs[hashIndex]->whiteptr; // exact match 
				}
				else {
					key = key.substr( i );
					basePtr = basePtr->blackptrs[hashIndex];
				}
			} else { // otherwise it's not there! (eg tes12 vs test) 
				return *(new oType);
			}
		}
	}
	
	
	
	bool insert( std::string key, oType & value ) {
		trie<oType> * basePtr = NULL;
		std::string * prefix, newPrefix, newSuffix;
		unsigned int hashIndex, cHashIndex, i;
		trie<oType> * tmpChild;
		
		while (true) {
			if (key.length() == 0) return false;
			prefix = NULL;
			if (basePtr == NULL) basePtr = this;
			
			hashIndex = hash( key.at(0) );
			if (basePtr->blackptrs[hashIndex] != NULL) prefix = &basePtr->blackptrs[hashIndex]->substr;
			
			if (prefix == NULL || key == *prefix) { // first entry, or white entry ( key == prefix ) 
				if ( basePtr->blackptrs[hashIndex] == NULL ) {
					basePtr->blackptrs[hashIndex] = new trie<oType>;
					basePtr->numChildren++;
				}
				if (basePtr->blackptrs[hashIndex]->whiteptr != NULL) delete basePtr->blackptrs[hashIndex]->whiteptr; // no memory leaks 
				basePtr->blackptrs[hashIndex]->whiteptr = new oType(value); // copy 
				if (prefix == NULL) basePtr->blackptrs[hashIndex]->substr = key; // not overwriting (new entry) 
				std::cout << "VALVALVLA1: " << key << " vs " << basePtr->blackptrs[hashIndex]->substr << std::endl;
				std::cout << "IN: " << *basePtr->blackptrs[hashIndex]->whiteptr << std::endl;
				return true;
			}
			
			// otherwise we have a (maybe partial) hit 
			//prefix = basePtr->blackptrs[hashIndex]->substr; // update the value 
			for (i = 1; i < key.length() && i < prefix->length(); ++i) { // we already know the first char matches 
				if ( key.at(i) != prefix->at(i) ) break;
			}
			std::cout << "VALVALVLA2: " << key << " vs " << basePtr->blackptrs[hashIndex]->substr << std::endl;
			std::cout << "PASVAL: " << i << std::endl;
			
			if ( i == prefix->length() ) { // key is a subset of the prefix (note: i != key.length() or we would've hit above) 
				basePtr = basePtr->blackptrs[hashIndex];
				key = key.substr( i );
				continue; // simulate recurse 
			}
			
			// otherwise we only matched partially to the prefix! (eg. tes12 vs test) 
			newPrefix = prefix->substr( 0, i ); // (eg tes) 
			newSuffix = prefix->substr( i ); // (eg t) 
			cHashIndex = hash( newSuffix.at(0) );
			
			// pull the rug out from under ourselves 
			tmpChild = new trie<oType>;
			basePtr->blackptrs[hashIndex]->substr = newSuffix;
			tmpChild->blackptrs[cHashIndex] = basePtr->blackptrs[hashIndex];
			basePtr->blackptrs[hashIndex] = tmpChild;
			basePtr->blackptrs[hashIndex]->substr = newPrefix;
			
			if (key == newPrefix) { // white entry (eg tes vs test) 
				basePtr->blackptrs[hashIndex]->whiteptr = new oType(value); // copy 
				std::cout << "VALVALVLA3: " << key << " vs " << basePtr->blackptrs[hashIndex]->substr << std::endl;
				std::cout << "IN: " << *basePtr->blackptrs[hashIndex]->whiteptr << std::endl;
				return true; /// the number of children stays the same! (we just swapped it out) 
			} else { // black ... keep poking 
				basePtr = basePtr->blackptrs[hashIndex]; // note: this is different than above! (basePtr->blackptrs[hashIndex] changed) 
				key = key.substr( i );
				continue; // simulate recurse 
			}
		}
		
		return false; // we'll never get this far 
	}
	
	
	
	bool remove( std::string key ) {
		trie<oType> * basePtr = NULL;
		std::string * prefix;
		int hashIndex, i;
		
		while (true) { // iterative soln (no recursive overhead) 
			if (key.length() == 0) return false; // wtf would you do this for (will be fixed with iterators) 
			prefix = NULL;
			if (basePtr == NULL) basePtr = this;
			
			hashIndex = hash( key.at(0) );
			if (basePtr != NULL) prefix = &basePtr->blackptrs[hashIndex]->substr;
			else return false;
			
			if (prefix->length() == 0) return false;
			
			for (i = 1; i < key.length() && i < prefix->length(); ++i) { // we already know the first char matches 
				if ( key.at(i) != prefix->at(i) ) break;
			}
			
			if ( i == prefix->length() ) { // key is a subset of the prefix (note: i != key.length() or we would've hit above) 
				if (key.length() == i && basePtr->blackptrs[hashIndex]->whiteptr != NULL) {
					return *basePtr->blackptrs[hashIndex]->whiteptr; // exact match 
				}
				else {
					key = key.substr( i );
					basePtr = basePtr->blackptrs[hashIndex];
				}
			} else { // otherwise it's not there! (eg tes12 vs test) 
				return *(new oType);
			}
		}	}
	
};
/// ################################ ///


}

#endif
