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
			this->blackptrs[i] = new trie<oType>( *cpTrie.blackptrs[i] );
		}
		this->substr = cpTrie.substr;
		this->whiteptr = new oType( *cpTrie.whiteptr );
		this->numChildren = cpTrie.numChildren;
	}
	trie<oType>& operator=( const trie<oType> & cpTrie ) {
		std::cout << "CPYASSGN" <<std::endl;
		if (this != &cpTrie) {
			this->~trie();
			for (int i = 0; i < 63; ++i) {
				this->blackptrs[i] = new trie<oType>( *cpTrie.blackptrs[i] );
			}
			this->substr = cpTrie.substr;
			this->whiteptr = new oType( *cpTrie.whiteptr );
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
		if (key.length() == 0) return *(new oType); // memory leak ... wtf would you do this for (will be fixed with iterators) 
		std::string prefix = "";
		
		int hashIndex = hash( key.at(0) );
		if (this->blackptrs[hashIndex] != NULL) prefix = this->blackptrs[hashIndex]->substr;
		else return *(new oType);
		
		if (prefix == "") return *(new oType);
		
		int i;
		for (i = 1; i < key.length() && i < prefix.length(); ++i) { // we already know the first char matches 
			if ( key.at(i) != prefix.at(i) ) break;
		}
		
		if ( i == prefix.length() ) { // key is a subset of the prefix (note: i != key.length() or we would've hit above) 
			if (key.length() == prefix.length() && this->blackptrs[hashIndex]->whiteptr != NULL) {
				return *this->blackptrs[hashIndex]->whiteptr; // exact match 
			}
			return this->blackptrs[hashIndex]->find( key.substr( i ) );
		} // otherwise it's not there! (eg tes12 vs test) 
		
		return *(new oType);
	}
	
	bool insert( std::string key, oType & value ) {
		if (key.length() == 0) return false;
		std::string prefix = "";
		
		int hashIndex = hash( key.at(0) );
		if (this->blackptrs[hashIndex] != NULL) prefix = this->blackptrs[hashIndex]->substr;
		
		if (prefix == "" || key == prefix) { // first entry, or white entry ( key == prefix ) 
			if ( this->blackptrs[hashIndex] == NULL ) this->blackptrs[hashIndex] = new trie<oType>;
			if (this->blackptrs[hashIndex]->whiteptr != NULL) delete this->blackptrs[hashIndex]->whiteptr;
			this->blackptrs[hashIndex]->whiteptr = new oType(value); // copy 
			if (prefix == "") this->blackptrs[hashIndex]->substr = key;
			std::cout << "VALVALVLA1: " << key << " vs " << this->blackptrs[hashIndex]->substr << std::endl;
			std::cout << "IN: " << *this->blackptrs[hashIndex]->whiteptr << std::endl;
			return true;
		}
		
		// otherwise we have a (maybe partial) hit 
		int i;
		prefix = this->blackptrs[hashIndex]->substr; // update the value 
		for (i = 1; i < key.length() && i < prefix.length(); ++i) { // we already know the first char matches 
			if ( key.at(i) != prefix.at(i) ) break;
		}
		std::cout << "VALVALVLA2: " << key << " vs " << this->blackptrs[hashIndex]->substr << std::endl;
		std::cout << "PASVAL: " << i << std::endl;
		
		if ( i == prefix.length() ) { // key is a subset of the prefix (note: i != key.length() or we would've hit above) 
			return this->blackptrs[hashIndex]->insert( key.substr( i ), value );
		}
		
		// otherwise we only matched partially to the prefix! (eg. tes12 vs test) 
		std::string newPrefix = prefix.substr( 0, i ); // (eg tes) 
		std::string newSuffix = prefix.substr( i ); // (eg t) 
		int cHashIndex = hash( newSuffix.at(0) );
		
		// pull the rug out from under ourselves 
		trie<oType> * tmpChild = new trie<oType>;
		this->blackptrs[hashIndex]->substr = newSuffix;
		tmpChild->blackptrs[cHashIndex] = this->blackptrs[hashIndex];
		this->blackptrs[hashIndex] = tmpChild;
		this->blackptrs[hashIndex]->substr = newPrefix;
		
		if (key == newPrefix) { // white entry (eg tes vs test) 
			this->blackptrs[hashIndex]->whiteptr = new oType(value); // copy 
			std::cout << "VALVALVLA3: " << key << " vs " << this->blackptrs[hashIndex]->substr << std::endl;
			std::cout << "IN: " << *this->blackptrs[hashIndex]->whiteptr << std::endl;
			return true;
		} else {
			return this->blackptrs[hashIndex]->insert( key.substr( i ), value );
		}
		
		return false;
	}
};
/// ################################ ///


}

#endif
