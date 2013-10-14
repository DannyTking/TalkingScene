#ifndef _BITMASK_H
#define _BITMASK_H

namespace Walaber 
{
	struct Bitmask 
	{
		int mask;

		void clear() { mask = 0x00; }

		void setOn( int bit )
		{
			mask |= (0x01 << (bit));
		}

		void setOff(int bit)
		{
			mask &= ~(0x01 << (bit));
		}

		bool getBit( int bit )
		{
			return (mask & (0x01 << (bit)));
		}
	};
}
#endif	// _BITMASK_H
