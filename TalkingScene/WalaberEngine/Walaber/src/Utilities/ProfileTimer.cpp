#include "ProfileTimer.h"
#include "Logger.h"

namespace Walaber
{
#ifdef _PROFILE_ENABLED
    std::vector<ProfileTimer::SectionDetails> ProfileTimer::mSectionList;
#endif
    
    void ProfileTimer::reserve( unsigned int sectionCount )
    {
#ifdef _PROFILE_ENABLED
		mSectionList.clear();
        mSectionList.reserve(sectionCount); 
		
		SectionDetails blank;
		for(unsigned int i = 0; i < sectionCount; ++i)
		{
			mSectionList.push_back(blank);
		}
#endif
    }
    
    void ProfileTimer::startSection( unsigned int sectionID )
    {
#ifdef _PROFILE_ENABLED
		
		SectionDetails& sd = mSectionList[sectionID];
		++sd.callCount;

#if TARGET_NDK
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		sd.startTime = (now.tv_sec * 1e9) + now.tv_nsec;
		Logger::printf("WMW", Logger::SV_INFO, "[%i] Start: sec: %il nano: %il", sectionID, now.tv_sec, now.tv_nsec);
#else
        sd.startTime = mach_absolute_time();
#endif
		
#endif
    }
    
    void ProfileTimer::stopSection( unsigned int sectionID )
    {
#ifdef _PROFILE_ENABLED
		
		SectionDetails& sd = mSectionList[sectionID];

#if TARGET_NDK
        struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		unsigned long long ns_now = (now.tv_sec * 1e9) + now.tv_nsec;
		sd.totalElapsed += (ns_now - sd.startTime);
		Logger::printf("WMW", Logger::SV_INFO, "[%i] End: sec: %il nano: %il", sectionID, now.tv_sec, now.tv_nsec);
#else
        uint64_t now = mach_absolute_time();
        sd.totalElapsed += (now - sd.startTime);
#endif
		
#endif
    }
    
    void ProfileTimer::reset()
    {
#ifdef _PROFILE_ENABLED

        Logger::printf("WMW", Logger::SV_INFO, "ProfileTimer::reset()\n");

        for (unsigned int i = 0; i < mSectionList.size(); i++)
        {
            SectionDetails& sd = mSectionList[i];
            sd.totalElapsed = 0;
            sd.startTime = 0;
            sd.callCount = 0;
        }
#endif
    }
    
    
    void ProfileTimer::printLog()
    {
#ifdef _PROFILE_ENABLED
		
#ifdef TARGET_NDK
		Logger::printf("WalaberProfiler", Logger::SV_INFO, "===========================================\n");
        
        Logger::printf("WalaberProfiler", Logger::SV_INFO,"ID\tTICKS\tCALLS\tAVERAGE NS\tAVG S\n");
        
        for (unsigned int i = 0; i < mSectionList.size(); i++)
        {
            SectionDetails& sd = mSectionList[i];
            
            double average = (sd.callCount > 0) ? ((double)sd.totalElapsed / (double)sd.callCount) : (0);
            
            // so here's the delta in seconds:
            double avg_ns = average * 1e-9;            
            
            Logger::printf("WalaberProfiler", Logger::SV_INFO,"%d\t%llu\t%llu\t%4.3f\t%4.3f\n", i, sd.totalElapsed, sd.callCount, average, avg_ns);
        }
        
        Logger::printf("WalaberProfiler", Logger::SV_INFO,"===========================================\n");

#else
        // this is the timebase info
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        double nano = 1e-9 * ( (double) info.numer) / ((double) info.denom);
        
        
        Walaber::Logger::printf("WalaberProfiler", Walaber::Logger::SV_INFO, "===========================================\n");
        
        Walaber::Logger::printf("WalaberProfiler", Walaber::Logger::SV_INFO,"ID\tTICKS\tCALLS\tAVERAGE\tAVG MS\n");
        
        for (unsigned int i = 0; i < mSectionList.size(); i++)
        {
            SectionDetails& sd = mSectionList[i];
            
            double average = (sd.callCount > 0) ? ((double)sd.totalElapsed / (double)sd.callCount) : (0);
            
            // so here's the delta in seconds:
            double avg_ms = ((average) * nano) * 1000.0f;
            
            
            Walaber::Logger::printf("WalaberProfiler", Walaber::Logger::SV_INFO, "%d\t%llu\t%llu\t%4.3f\t%4.3f\n", i, sd.totalElapsed, sd.callCount, average, avg_ms);
        }
        
        Walaber::Logger::printf("WalaberProfiler", Walaber::Logger::SV_INFO, "===========================================\n");
#endif
		
#endif
    }
}
