/*
 *  DeviceManager.cpp
 *  JellyCar3
 *
 *  Created by jpecho on 12/3/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "PlatformManager.h"

#include "FileManager.h"
#include "StringHelper.h"
#include "WalaberMath.h"

namespace Walaber
{
    std::string PlatformManager::PlatformStrings[] = 
    {
        std::string(""),
        std::string("-HD"),
        std::string("-Tab"),
        std::string("-TabHD")
    };
    
	PlatformManager* PlatformManager::instance = 0;
	
	PlatformManager::PlatformManager():     
	mType(PT_Device),
    mDisplayType(PDT_StandardDefinition),
	mFormFactor(PFF_Handheld),
    mCPUQuality(PCPU_QualityLevel_1),
    mGPUQuality(PGPU_QualityLevel_1),
    mPlatformBitMask(0),
    mLargestPlatformStringSize(0),
	mDisplayScale(1.0f)
	{		
        // determine largest size of platform strings
		// default ignore extensions
//        for(int i = 0; i < PSN_SIZE; ++i)
//        {
//            mLargestPlatformStringSize = maxi(mLargestPlatformStringSize, PlatformStrings[i].size());
//        }
        
		mIgnoreExtensions.insert( ".wav" );
	}
	
	
	
	PlatformManager* PlatformManager::getInstancePtr()
	{
		if(!instance)
		{
			instance = new PlatformManager();
		}
		return instance;
	}
	
	
	
    std::string PlatformManager::stripPlatformSpecificFilename(const std::string& filename)
	{
        size_t pos = filename.find_last_of('.');
        if(pos != std::string::npos)
        {
            // get extension
            std::string dotExt = filename.substr(pos);
            
            // start search from the max size of the plat string before the .
            pos -= (mLargestPlatformStringSize+1);
            for(int i=0; i < PSN_SIZE; ++i)
            {           
                if(PlatformStrings[i].size())
                {
                    size_t platPos = filename.find(PlatformStrings[i],pos);
                    
                    if(platPos != std::string::npos)
                    {
                        // we've found the platform string
                        std::string stripped = filename.substr(0, platPos);
                        stripped += dotExt;
                        return stripped;
                    }
                }
            }
        }
         
        // if we get here, there was no platform tag so just return the string
        return filename;
	}
    
	std::string PlatformManager::getPlatformSpecificFilename(const std::string& filename)
	{
        PlatformStringNames psn;
        
        if(mPlatformBitMask & (int)PFF_Tablet)
        {
            if (mPlatformBitMask & (int)PDT_HighDefinition)
                psn = PSN_TabletHD;
            else
                psn = PSN_Tablet;
        }
        else
        {
            if(mPlatformBitMask & (int)PDT_HighDefinition)
                psn = PSN_HD;
            else
                psn = PSN_Standard;
        }
        
        return StringHelper::removeExtension(filename) + 
				PlatformStrings[(int)psn] + 
				StringHelper::getExtension(filename);	
	}
       
    std::string PlatformManager::appendSpecificPlatformTagToFilename(
                                        const std::string& filename,
                                        const PlatformManager::PlatformStringNames platformString
                                                                     )
    {        
        return StringHelper::removeExtension(filename) +
        PlatformStrings[(int)platformString] +
        StringHelper::getExtension(filename);
    }
	
	
	
	std::vector< std::string > PlatformManager::getPrioritizedPlatformTags()
	{
		std::vector< std::string > ret;
		
        // -Tab
        if(mPlatformBitMask & (int)PFF_Tablet)
        {
            // -TabHD
            if (mPlatformBitMask & (int)PDT_HighDefinition)
                ret.push_back(PlatformStrings[PSN_TabletHD]);
            
            ret.push_back(PlatformStrings[PSN_Tablet]);;
        }
        
        // -HD
        if( mPlatformBitMask & ((int)PFF_Tablet | (int)PDT_HighDefinition) )
        {
            ret.push_back(PlatformStrings[PSN_HD]);
        }
        // standard
        
        ret.push_back(PlatformStrings[PSN_Standard]);

		return ret;
	}
	
	bool PlatformManager::extensionShouldBeIgnored(const std::string& filename )
	{
		std::set< std::string >::iterator it = mIgnoreExtensions.find(StringHelper::getExtension(filename));
		return (it != mIgnoreExtensions.end());
	}
	
	void PlatformManager::addIgnoreExtension( const std::string& ext )
	{
		mIgnoreExtensions.insert( StringHelper::getExtension(ext) );
	}

	
}