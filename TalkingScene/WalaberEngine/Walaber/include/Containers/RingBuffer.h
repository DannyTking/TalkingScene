#ifndef _RINGBUFFER_H_
#define _RINGBUFFER_H_


#include <cassert>

namespace Walaber 
{

	
template <class T>

// ----====----====----====----====----====----====----====----====----====----====
// ----====----====----====----====----====----====----====----====----====----====
class RingBuffer
{
	
private:
	T* mArray;
	int mArrayLength;
	
	int mHead;
	int mCount;

	
public:
	// ----====----====----====----====----====----====----====----====----====----====
	RingBuffer()
	{
		mArrayLength = 5;
		mArray = new T[mArrayLength];
		
		mHead = mCount = 0;
	}
	
	// ----====----====----====----====----====----====----====----====----====----====
	RingBuffer( int count )
	{
		mArrayLength = count;
		mArray = new T[count];
		
		mHead = mCount = 0;
	}
	
	// ----====----====----====----====----====----====----====----====----====----====
	~RingBuffer()
	{
		delete[] mArray;
	}
	
	// ----====----====----====----====----====----====----====----====----====----====
	void setCapacity( int s )
	{
		if (mArray != NULL)
		{
			delete[] mArray;
		}
		
		mArrayLength = s;
		mArray = new T[s];
		
		mHead = mCount = 0;
	}

	// ----====----====----====----====----====----====----====----====----====----====
	void push( T val )
	{
		mHead++;
		if (mHead >= (mArrayLength)) { mHead = 0; }

		mArray[mHead] = val;

		if (mCount < mArrayLength) { mCount++; }
	}

	// ----====----====----====----====----====----====----====----====----====----====
	const T& goBack(int a)
	{
        assert( a < mCount );
        
		int index = mHead - a;
		while (index < 0) { index += mCount; }

		return mArray[index];
	} 

	// ----====----====----====----====----====----====----====----====----====----====
	int size()
	{
		return mCount;
	}
	
	// ----====----====----====----====----====----====----====----====----====----====
	int capacity()
	{
		return mArrayLength;
	}

	// ----====----====----====----====----====----====----====----====----====----====
	void clear()
	{
		mCount = 0;
		mHead = 0;
	}
};

	
}

#endif	// _RINGBUFFER_H_
