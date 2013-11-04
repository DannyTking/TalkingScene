/*
 *  DeviceManager.h
 *  JellyCar3
 *
 *  Created by jpecho on 12/3/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _PLATFORM_MANAGER_H_
#define _PLATFORM_MANAGER_H_

#include <vector>
#include <string>
#include <map>
#include <set>

namespace Walaber
{
	class PlatformManager 
	{
	public:
        
        static std::string PlatformStrings[];
        
        enum PlatformStringNames
        {
            PSN_Standard,   // standard handheld device
            PSN_HD,         // "HD" handheld device (high pixel density)
            PSN_Tablet,     // standard tablet device
            PSN_TabletHD,   // "HD" tablet device (high pixel density)
            PSN_SIZE
        };
		
		enum PlatformCPUQuality
        {
            PCPU_QualityLevel_1 =     1, // lowest quality level
            PCPU_QualityLevel_2 = (1<<2),
            PCPU_QualityLevel_3 = (1<<3),
            PCPU_QualityLevel_4 = (1<<4),
            PCPU_QualityLevel_5 = (1<<5),// highest quality level
        };
        
        enum PlatformGPUQuality
        {
            PGPU_QualityLevel_1 = (1<< 6), // lowest quality level
            PGPU_QualityLevel_2 = (1<< 7),
            PGPU_QualityLevel_3 = (1<< 8),
            PGPU_QualityLevel_4 = (1<< 9),
            PGPU_QualityLevel_5 = (1<<10),// highest 
        };
        
		enum PlatformDisplayType
        {
            PDT_StandardDefinition =(1<<11),
            PDT_HighDefinition =    (1<<12),
        };		
		
		enum PlatformFormFactor
		{
			PFF_Handheld = (1<<13),
			PFF_Tablet =   (1<<14),
		};
		
        enum PlatformType
        {
            PT_Device,
            PT_Simulator
        };
		
		static PlatformManager* getInstancePtr();
		
		std::string getPlatformSpecificFilename(const std::string& filename);
        std::string appendSpecificPlatformTagToFilename(const std::string& filename, const PlatformStringNames platformString);
        std::string stripPlatformSpecificFilename(const std::string& filename);
		
		std::vector< std::string > getPrioritizedPlatformTags();
	
		void addIgnoreExtension(const std::string& ext );
		
		bool extensionShouldBeIgnored(const std::string& filename );
				
		void setPlatform(PlatformType pType, PlatformDisplayType pDisplayType, PlatformFormFactor pFormFactor, PlatformCPUQuality pCPUQuality, PlatformGPUQuality pGPUQuality) 
        { 
            mType = pType;
            mDisplayType = pDisplayType; 
            mFormFactor = pFormFactor; 
            mCPUQuality = pCPUQuality;
            mGPUQuality = pGPUQuality;
            
            mPlatformBitMask = ( (int)mDisplayType | (int)mFormFactor | (int)mCPUQuality | (int)mGPUQuality);
        }
		
		void setDisplayScale( float s ) { mDisplayScale = s; }
        
        PlatformType getPlatformType() {return mType; }
        PlatformDisplayType getDisplayType() { return mDisplayType; }
		PlatformFormFactor getFormFactor() { return mFormFactor; }
		PlatformCPUQuality getCPUQuality() { return mCPUQuality; }
		PlatformGPUQuality getGPUQuality() { return mGPUQuality; }
        float getDisplayScale() { return mDisplayScale; }
		
        int getDeviceBitMask() { return mPlatformBitMask; }
		
	protected:
		PlatformManager();
		explicit PlatformManager(const PlatformManager& platformMgr){};
		PlatformManager& operator=(const PlatformManager& platformMgr){ return *this; }
		~PlatformManager(){};
		
		static PlatformManager* instance;
		
		std::set< std::string >					mIgnoreExtensions;

        PlatformType                            mType;
		PlatformDisplayType						mDisplayType; 
		PlatformFormFactor						mFormFactor;
        PlatformCPUQuality                      mCPUQuality;
        PlatformGPUQuality                      mGPUQuality;
		float									mDisplayScale;
        int                                     mPlatformBitMask;
        int                                     mLargestPlatformStringSize;
	};
}
#endif

