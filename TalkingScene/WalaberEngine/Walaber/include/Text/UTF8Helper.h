/*
 *  UTF8Helper.h
 *  JellyCar3
 *
 *  Created by jpecho on 12/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _UTF8_HELPER_H
#define _UTF8_HELPER_H

#include <vector>

// explanation of UTF8 encoding
// http://en.wikipedia.org/wiki/UTF-8

namespace Walaber
{
	class UTF8Helper 
	{
	public:
		enum UTF8_DECODER {
			// and 
			SINGLE_BYTE_MASK   = 0x80,
			DOUBLE_BYTE_MASK   = 0xE0,
			TRIPLE_BYTE_MASK   = 0xF0,
			FOUR_BYTE_MASK     = 0xF8,
			TRAILING_BYTE_MASK = 0xC0,
			 
			// xor
			SINGLE_BYTE_CODE   = 0x00,
			DOUBLE_BYTE_CODE   = 0xC0,
			TRIPLE_BYTE_CODE   = 0xE0,
			FOUR_BYTE_CODE     = 0xF0,
			TRAILING_BYTE_CODE = 0x80,
		};
		
		enum Shift_Key 
		{
			SK_LEFT,
			SK_RIGHT
		};
		
		// everything is unsigned ints so as to force a logical shift
		static unsigned int shiftBytes(unsigned int toShift, const unsigned int numBytes, Shift_Key sk);
		static unsigned int shiftBits(unsigned int toShift, const unsigned int numBits, Shift_Key sk);
		
		static unsigned int decimalForUTF8Bytes(const unsigned char* const bytes, const int numBytes);
		static unsigned int getNumCodeBytes(const unsigned int utf8Byte);
		
	private:
		
		static unsigned int _shiftLeft(unsigned int toShift, const unsigned int numBits);
		static unsigned int _shiftRight(unsigned int toShift, const unsigned int numBits);
	};	
}

#endif
