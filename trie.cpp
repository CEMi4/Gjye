#include <sstream>
#include <stdlib.h>
#include <iostream>

#include "trie.h"


namespace gstl {

//################ Hashing algorithm ################//
	int hash(const char & in) {
		if ( (in ^ 95) == 0 ) return 0; // _ 
		char tmp = in;
		if ( (tmp -= 47) <= 10 ) return tmp;  // 0-9
		if ( (tmp -= 7) <= 36 ) return tmp;  // A-Z
		tmp -= 6;
		return tmp;  // a-z
	}
/// ################################ ///


}
