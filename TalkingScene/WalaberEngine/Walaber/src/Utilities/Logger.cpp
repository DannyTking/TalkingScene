#include "Logger.h"

#include <cassert>
#include <cstdarg>
#include <cstdio>

#ifdef TARGET_NDK
#include <android/log.h>
#endif

namespace Walaber
{
	Logger::Severity Logger::mMinimalSeverityToLog = Logger::SV_VERBOSE;

	namespace
	{
		const char* severityToString(Logger::Severity severity)
		{
			const char* str[] = {"VERBOSE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
			int idx = (int)severity;

			assert(0 <= idx && idx <= 5 && "Unknown severity");

			return str[idx];
		}

#ifdef TARGET_NDK
		int severityToAndroidCode(Logger::Severity severity)
		{
			int codes[] = {ANDROID_LOG_VERBOSE, ANDROID_LOG_DEBUG, ANDROID_LOG_INFO, ANDROID_LOG_WARN, ANDROID_LOG_ERROR, ANDROID_LOG_FATAL};
			int idx = (int)severity;

			assert(0 <= idx && idx <= 5 && "Unknown severity");

			return codes[idx];

		}
#endif

	}

	void Logger::printf(const char* category, Severity severity, const char* fmt, ...)
	{
		if (severity < mMinimalSeverityToLog)
			return;

		va_list args;
		va_start(args, fmt);
#ifdef TARGET_NDK
		__android_log_vprint( severityToAndroidCode(severity), category, fmt, args );
#else
		fprintf( stderr, "[%s] %s:", category, severityToString(severity) );
		vfprintf( stderr, fmt, args );
#endif
		va_end(args);
	}

    void Logger::printfVerbose( bool verbose, const char* fmt, ...)
	{
        if( verbose )
        {        
            va_list args;
            va_start(args, fmt);
            vprintf( fmt, args );
            va_end(args);
        }
	}

}
