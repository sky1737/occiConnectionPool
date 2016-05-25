/*************************************************************************
	> File Name: log.h
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Thu 19 May 2016 07:44:07 PM PDT
 ************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

#include <boost/log/sources/severity_logger.hpp>
enum LogLevel
{
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL
};

class CLog
{
public:
	static CLog *GetInstance()
	{
		return m_log;
	};
	static void Initialize();
	static void Finalize();
	static void Log(LogLevel level, long line, char const *file, char const *function, const char *format, ...);
private:
	CLog(){};
	static CLog *m_log;
	static boost::log::sources::severity_logger<LogLevel> m_boostLog;
	class LogConfig
	{
	public:
		bool m_bLogging;
		bool m_bAsync;
		bool m_bConsoleOut;
		int m_iRotationSize;
		LogLevel m_llLevel;
		std::string m_strNameFormat;
		std::string m_strDirectory;
		LogConfig();
	};
	static LogConfig m_logConfig;
	static void InitLog(const LogConfig &logConfig);

	class GC
	{
	public:	
		~GC()
		{
			if (m_log != NULL)	
			{
				delete m_log;	
				m_log = NULL;
			}
		}
	};
	static GC m_gc;
};
#define MYLOG(level, format, ...)			\
	do {									\
		CLog::Log(level, __LINE__, __FILE__, __FUNCTION__, format, ##__VA_ARGS__); \
	}while(0)

#endif
