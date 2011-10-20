#ifndef __DAMARIS_SERIALIZABLE_H
#define __DAMARIS_SERIALIZABLE_H

#include <stdlib.h>

namespace Damaris {

	class Serializable 
	{
		public:
			virtual size_t size() = 0;
			virtual void toBuffer(void*) = 0;
			virtual	void fromBuffer(const void*) = 0;
	};
}

#endif
