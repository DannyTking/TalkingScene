//
//  HashFunctions.h
//  WalaberEngine
//
//  Created by Jason Pecho on 12/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef _HASH_FUNCTIONS_H_
#define _HASH_FUNCTIONS_H_

namespace Walaber
{
	class HashFunctions
	{
		unsigned long djb2(unsigned char *str)
		{
			unsigned long hash = 5381;
			int c;
			
			while (c = *str++)
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
			
			return hash;
		}
	};
}

#endif
