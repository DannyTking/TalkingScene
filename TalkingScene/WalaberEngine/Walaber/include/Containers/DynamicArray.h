//
//  DynamicArray.h
//  WalaberEngine
//
//  Created by Jason Pecho on 7/26/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef _DYNAMIC_ARRAY_H_
#define _DYNAMIC_ARRAY_H_

#include <cassert>
#include <cstring>

namespace Walaber
{
    template < typename T >
    class DynamicArray
    {
    public:
        //<Summary>
        // default ctor - gives capacity of DEFAULT_CAPACITY
        //</Summary>
        DynamicArray();
        
        //<Summary>
        // ctor that laods from a buffer - expects that first bytes will be an unsigned int saying how many
        // objects follow
        //</Summary>
        DynamicArray(char *buf, unsigned int& bytesRead);
        
        //<Summary>
        // fill an array with 'size' copies of t
        //</Summary>
        DynamicArray(const size_t size, const T& t = T());
        
        //<Summary>
        // copy ctor - deep copy
        //</Summary>
        DynamicArray(const DynamicArray<T>& dynamicArray);
        
        //<Summary>
        // op= deep copy
        //</Summary>
        DynamicArray& operator=(const DynamicArray<T>& dynamicArray);
        
        //<Summary>
        // delete allocated space
        //</Summary>
        ~DynamicArray();
        
        //<Summary>
        // return the capacity of the array
        //</Summary>
        inline size_t capacity() const
        {
            return mCapacity;
        }
        
        //<Summary>
        // calls the destructor of all objects and decreases size to 0 but do not deallocate anything
        //</Summary>
        inline void clear();
        
        //<Summary>
        // returns true if the array is empty
        //</Summary>
        inline bool empty() const
        {
            return (mSize == 0);
        }
        
        //<Summary>
        // adds a copy of T to the end of the array. A reallocation me occur here if mSize+1 > mCapacity
        //</Summary>
        inline void push_back(const T& t);
        
        //<Summary>
        // if mCapacity < capacity, a reallocation occurs, otherwise nothing happens
        //</Summary>
        inline void reserve(const size_t capacity);
        
        //<Summary>
        // if size < mSize, the end of the array is trimmed and the capacity is reduced
        // if size > mSize, the difference is made up of copies of T
        //</Summary>
        inline void resize(const size_t size, const T& t = T());
        
        //<Summary>
        // returns how many objects are currently in the array
        //</Summary>
        inline size_t size() const
        {
            return mSize;
        }
        
        //<Summary>
        // returns a reference to the object at n - uses asserts for bounds checks
        //</Summary>
        inline T& operator[](const size_t n)
        {
            assert(mData);		
            assert(n < mSize);
            
            return mData[n]; 
        }
        
        //<Summary>
        // returns a const reference to the object at n - uses asserts for bounds checks
        //</Summary>
        const T& operator[](const size_t n) const
        {
            assert(mData);		
            assert(n < mSize);
            
            return mData[n]; 
        }
        
		//<Summary>
        // gets the size of the data to write
        //</Summary>
        unsigned int getWriteSize()
        {            
            return sizeof(mSize) + (sizeof(T) * mSize);
        }
		
        //<Summary>
        // copies contents of mData to buf - returns number of bytes written
        //</Summary>
        unsigned int writeToBuffer(char* buf)
        {
            unsigned int bytesWritten = 0;
			
			// write number of objects
            memcpy(buf, &mSize, sizeof(mSize));
            bytesWritten += sizeof(mSize);						
            
            memcpy(&buf[bytesWritten], mData, sizeof(T) * mSize);
            bytesWritten += (sizeof(T) * mSize);
            
            return bytesWritten;
        }
        
    private:
        size_t         mSize;
        size_t         mCapacity;
        T*             mData;
        
        static const int  DEFAULT_CAPACITY;
        static const int  DEFAULT_ARRAY_EXPANSION_FACTOR;
    };
    
    template<typename T> 
    const int DynamicArray<T>::DEFAULT_CAPACITY = 4;
    
    template<typename T> 
    const int DynamicArray<T>::DEFAULT_ARRAY_EXPANSION_FACTOR = 2;
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    DynamicArray<T>::DynamicArray():
    mSize(0),
    mCapacity(DEFAULT_CAPACITY),
    mData(reinterpret_cast<T*>(new char[DEFAULT_CAPACITY * sizeof(T)]))
    {}
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    DynamicArray<T>::DynamicArray(char *buf, unsigned int& bytesRead):
    mSize(*(size_t*)buf),
    mCapacity(*(size_t*)buf),
    mData(reinterpret_cast<T*>(new char[(*(size_t*)buf) * sizeof(T)]))
    {
        bytesRead += sizeof(size_t);
        memcpy(mData, &buf[bytesRead], mSize * sizeof(T));
        bytesRead += mSize * sizeof(T);
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    DynamicArray<T>::DynamicArray(const size_t size, const T& t):
    mSize(size),
    mCapacity(size),
    mData(reinterpret_cast<T*>(new char[size * sizeof(T)]))
    {
        for(unsigned int i=0; i < size; ++i)
        {
            new(mData + i) T(t);
        }
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    DynamicArray<T>::DynamicArray(const DynamicArray& dynamicArray):
    mSize(dynamicArray.mSize),
    mCapacity(dynamicArray.mSize),
    mData(reinterpret_cast<T*>(new char[dynamicArray.mSize * sizeof(T)]))
    {
        for(unsigned int i=0; i < mSize; ++i)
        {
            new(mData + i) T(*(dynamicArray.mData+i));
        }
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray& dynamicArray)
    {
        // handle self assignment here
        if(&dynamicArray != this)
        {   
            // only alloc if we don't have the capacity
            if(dynamicArray.mSize > mCapacity)
            {
                // only alloc if we don't have the capacity
                delete [] reinterpret_cast<char*>(mData);
                
                mCapacity = dynamicArray.mSize;                
                mData = reinterpret_cast<T*>(new char[dynamicArray.mSize * sizeof(T)]);
            }
            
            mSize = dynamicArray.mSize;            
            
            for(unsigned int i=0; i < mSize; ++i)
            {
                new(mData + i) T(*(dynamicArray.mData+i));
            }
        }
        
        return *this;
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    DynamicArray<T>::~DynamicArray()
    {
        delete [] reinterpret_cast<char*>(mData);
        mSize = 0;
        mCapacity = 0;
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    void DynamicArray<T>::clear()
    {
        for(unsigned int i = 0; i < mSize; ++i)
        {
            mData[i].~T();
        }
        mSize = 0;
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    void DynamicArray<T>::push_back(const T& t)
    {
        // check if we need to reallocate
        if((mSize+1) > mCapacity)
        {
            // we do, so create a new buffer 
            mCapacity *= DEFAULT_ARRAY_EXPANSION_FACTOR;
            T* tBuf = reinterpret_cast<T*>(new char[mCapacity * sizeof(T)]);
            
            // copy the current data in
            memcpy(tBuf, mData, mSize * sizeof(T));
            
            // swap the buffers
            T* temp = mData;
            mData = tBuf;
            
            // delete the old buffer
            delete [] reinterpret_cast<char*>(temp);           
        }
        
        // and add the new data
        mData[mSize] = t;
        ++mSize;
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    void DynamicArray<T>::reserve(const size_t capacity)
    {
        if(capacity > mCapacity)
        {
            // we do, so create a new buffer 
            mCapacity = capacity;
            T* tBuf = reinterpret_cast<T*>(new char[mCapacity * sizeof(T)]);
            
            // copy the current data in
            memcpy(tBuf, mData, mSize * sizeof(T));
            
            // swap the buffers
            T* temp = mData;
            mData = tBuf;
            
            // delete the old buffer
            delete [] reinterpret_cast<char*>(temp); 
        }
    }
    
    //---------============------------============------------===========--------------============-------------
    template<typename T>
    void DynamicArray<T>::resize(const size_t size, const T& t)
    {
        if(size < mSize)
        {
            // trim the array - change this to call erase on each object when erase created 
            for(unsigned int i = mSize-1; i >= size; --i)
            {
                mData[i].~T();
            }
        }
        else if(size > mSize)
        {
            // we do, so create a new buffer 
            mCapacity = size;
            T* tBuf = reinterpret_cast<T*>(new char[mCapacity * sizeof(T)]);
            
            // copy the current data in
            memcpy(tBuf, mData, mSize * sizeof(T));
            
            // swap the buffers
            T* temp = mData;
            mData = tBuf;
            
            // delete the old buffer
            delete [] reinterpret_cast<char*>(temp); 
            
            // fill with copies of t
            for(unsigned int i=mSize; i < size; ++i)
            {
                mData[i] = t;
            }
        }
        
        mSize = size;
    }
}

#endif