/*
 *  UTF8Helper.cpp
 *  JellyCar3
 *
 *  Created by jpecho on 12/28/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "UTF8Helper.h"

namespace Walaber
{	
	unsigned int UTF8Helper::shiftBytes(unsigned int toShift, const unsigned int numBytes, Shift_Key sk)
	{
		unsigned int ret = toShift;
		switch (sk) 
		{
			case SK_LEFT:
				ret = _shiftLeft(toShift, (numBytes * 8));
				break;
			case SK_RIGHT:
				ret = _shiftRight(toShift, (numBytes * 8));
			default:
				break;
		}
		
		return ret;
	}

	unsigned int UTF8Helper::shiftBits(unsigned int toShift, const unsigned int numBits, Shift_Key sk)
	{
		unsigned int ret=toShift;
		switch (sk) 
		{
			case SK_LEFT:
				ret = _shiftLeft(toShift, numBits);
				break;
			case SK_RIGHT:
				ret = _shiftRight(toShift, numBits);
			default:
				break;
		}
		
		return ret;
	}
	
	// doesn't do bounds checking
	unsigned int UTF8Helper::decimalForUTF8Bytes(const unsigned char* const bytes, const int numBytes)
	{
		unsigned int ret = 0;
		switch (numBytes) 
		{
			case 1:
				ret = bytes[0];
				break;
			case 2:
			{
				ret = bytes[0] & ~DOUBLE_BYTE_MASK;
				ret = UTF8Helper::shiftBits(ret, 6, UTF8Helper::SK_LEFT);
				ret |= bytes[1] & ~TRAILING_BYTE_MASK;
			}
				break;
			case 3:
			{
				ret = bytes[0] & ~TRIPLE_BYTE_MASK;
				ret = UTF8Helper::shiftBits(ret, 12, UTF8Helper::SK_LEFT);
				ret |= UTF8Helper::shiftBits((bytes[1] & ~TRAILING_BYTE_MASK), 6, UTF8Helper::SK_LEFT);
				ret |= bytes[2] & ~TRAILING_BYTE_MASK;
			}
				break;
			case 4:
			{
				ret = bytes[0] & ~FOUR_BYTE_MASK;				
				ret = UTF8Helper::shiftBits(ret, 18, UTF8Helper::SK_LEFT);
				ret |= UTF8Helper::shiftBits((bytes[1] & ~TRAILING_BYTE_MASK), 12, UTF8Helper::SK_LEFT);
				ret |= UTF8Helper::shiftBits((bytes[2] & ~TRAILING_BYTE_MASK), 6, UTF8Helper::SK_LEFT);
				ret |= bytes[3] & ~TRAILING_BYTE_MASK;
			}
			default:
				break;
		}
		
		return ret;
	}
	
	unsigned int UTF8Helper::getNumCodeBytes(const unsigned int utf8Byte)
	{
		unsigned int ret = 0;
		
		if(!( (utf8Byte & SINGLE_BYTE_MASK) ^ SINGLE_BYTE_CODE))
		{
			ret = 1;
		}
		else if(!( (utf8Byte & DOUBLE_BYTE_MASK) ^ DOUBLE_BYTE_CODE))
		{
			ret = 2;
		}
		else if(!( (utf8Byte & TRIPLE_BYTE_MASK) ^ TRIPLE_BYTE_CODE))
		{
			ret = 3;
		}
		else if(!( (utf8Byte & FOUR_BYTE_MASK) ^ FOUR_BYTE_CODE))
		{
			ret = 4;
		}
		
		return ret;			
	}

	unsigned int UTF8Helper::_shiftLeft(unsigned int toShift, const unsigned int numBits)
	{
		return (toShift << numBits);
	}
	
	unsigned int UTF8Helper::_shiftRight(unsigned int toShift, const unsigned int numBits)
	{
		return (toShift >> numBits);
	}
}