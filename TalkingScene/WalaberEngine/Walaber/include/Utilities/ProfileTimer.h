#ifndef _PROFILE_TIMER_H_
#define _PROFILE_TIMER_H_

#include <vector>

#ifdef TARGET_NDK
// to get uint64_t
#include <time.h>
#else
#include <mach/mach_time.h>
#endif

namespace Walaber
{
    class ProfileTimer
    {
    public:
        
        static void reserve( unsigned int sectionCount );
        
        static void startSection( unsigned int sectionID );
        
        static void stopSection( unsigned int sectionID );
        
        static void reset();
        
        static void printLog();
        
        
    private:
        
        struct SectionDetails
        {
        public:
            SectionDetails() : totalElapsed(0), startTime(0), callCount(0) { }
#ifdef TARGET_NDK
			unsigned long long totalElapsed;
			unsigned long long startTime;
			unsigned long long callCount;
#else
            uint64_t        totalElapsed;
            uint64_t        startTime;
            uint64_t        callCount;
#endif
        };
        
#ifdef _PROFILE_ENABLED
        static std::vector< SectionDetails >    mSectionList;
#endif
        
    };
}


#endif  // _PROFILE_TIMER_H_
