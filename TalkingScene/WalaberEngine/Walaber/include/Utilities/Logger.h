#ifndef _WALABER_LOGGER_HEADER_FILE_
#define _WALABER_LOGGER_HEADER_FILE_


namespace Walaber
{
	class Logger
	{
	public:
		enum Severity
		{
		    SV_VERBOSE = 0,
		    SV_DEBUG,
		    SV_INFO,
		    SV_WARN,
		    SV_ERROR,
		    SV_FATAL
		};

		static void printf(const char* category, Severity severity, const char* fmt, ...);

        static void printfVerbose( bool verbose, const char* fmt, ...);
        
		static void setMinimalSeverityToLog(Severity minSeverity)
		{ mMinimalSeverityToLog = minSeverity; }

	private:
		static Severity mMinimalSeverityToLog;
	};
}

#endif //_WALABER_LOGGER_HEADER_FILE_
