/*************************************************************************
	> File Name: Log.cpp
	> Author: jiachuang
	> Mail: jiachuang0210@qq.com 
	> Created Time: Thu 19 May 2016 10:13:38 PM PDT
 ************************************************************************/
#include <thread>
#include <boost/log/common.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/async_frontend.hpp>

#include "Log.h"

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;

static const bool DEFAULT_LOGGING = true;
static const bool DEFAULT_ASYNC = true;
static const bool DEFAULT_CONSOLE_OUT = true;
static const int DEFAULT_ROTATION_SIZE = 10*1024*1024;//10M
//static const LogLevel DEFAULT_LEVEL = DEBUG;
static const LogLevel DEFAULT_LEVEL = INFO;
//static const LogLevel DEFAULT_LEVEL = ERROR;
static const std::string DEFAULT_NAME_FORMAT = "%N_%Y%m%d_%H%M%S.log";
static const std::string DEFAULT_DIRECTORY = "./mylogs";

// 宏定义
#define MIN_LOG_FREE_SPACE           (300 * 1024 * 1024)
#define MAX_LOG_DIR_SIZE             (200* 1024 * 1024)
#define MAX_LOG_MESSAGE_SIZE         (1024)

/// 重载日志级别的输出
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, LogLevel lvl)
{
    static const char* const str[] =
    {
        "DEBUG",
        "INFO",
        "WARNING",
        "ERROR",
        "FATAL"
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}
// 添加Boost属性关键字
BOOST_LOG_ATTRIBUTE_KEYWORD(logSeverity, "Severity", LogLevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(logTimeStamp, "TimeStamp", boost::posix_time::ptime)
//BOOST_LOG_ATTRIBUTE_KEYWORD(log_uptime, "Uptime", attrs::timer::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_scope, "Scope", attrs::named_scope::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(logLineId, "LineID", unsigned int)

CLog::LogConfig::LogConfig()
			:m_bLogging(DEFAULT_LOGGING)
	        ,m_bAsync(DEFAULT_ASYNC)
		   	,m_bConsoleOut(DEFAULT_CONSOLE_OUT)
			,m_iRotationSize(DEFAULT_ROTATION_SIZE)
			,m_llLevel(DEFAULT_LEVEL)
			,m_strNameFormat(DEFAULT_NAME_FORMAT)
			,m_strDirectory(DEFAULT_DIRECTORY){
};
void CLog::InitLog(const LogConfig &logConfig)
{
	//TODO:check logConfig
	typedef sinks::synchronous_sink< sinks::text_file_backend > syncFileSink;
	typedef sinks::asynchronous_sink< sinks::text_file_backend > asyncFileSink;
    boost::shared_ptr< syncFileSink > syncSink = boost::shared_ptr< syncFileSink >(new syncFileSink(
        keywords::file_name = logConfig.m_strDirectory + "/" + logConfig.m_strNameFormat,                  // 文件名
        keywords::rotation_size = logConfig.m_iRotationSize,                                    // 日志轮转大小
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),     // 每天0点进行日志轮转
        keywords::open_mode = (std::ios::out | std::ios::app)                             // 打开上次的日志文件
    ));

    boost::shared_ptr< syncFileSink > syncSinkErr = boost::shared_ptr< syncFileSink >(new syncFileSink(
        keywords::file_name = logConfig.m_strDirectory + "/" + logConfig.m_strNameFormat + "err",                  // 文件名
        keywords::rotation_size = logConfig.m_iRotationSize,                                    // 日志轮转大小
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),     // 每天0点进行日志轮转
        keywords::open_mode = (std::ios::out | std::ios::app)                             // 打开上次的日志文件
    ));

    boost::shared_ptr< asyncFileSink > asyncSink = boost::shared_ptr< asyncFileSink >(new asyncFileSink(
        keywords::file_name = logConfig.m_strDirectory + "/" + logConfig.m_strNameFormat,                  // 文件名
        keywords::rotation_size = logConfig.m_iRotationSize,                                    // 日志轮转大小
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),     // 每天0点进行日志轮转
        keywords::open_mode = (std::ios::out | std::ios::app)                             // 打开上次的日志文件
    ));
    boost::shared_ptr< asyncFileSink > asyncSinkErr = boost::shared_ptr< asyncFileSink >(new asyncFileSink(
        keywords::file_name = logConfig.m_strDirectory + "/" + logConfig.m_strNameFormat + "err",                  // 文件名
        keywords::rotation_size = logConfig.m_iRotationSize,                                    // 日志轮转大小
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),     // 每天0点进行日志轮转
        keywords::open_mode = (std::ios::out | std::ios::app)                             // 打开上次的日志文件
    ));

    // 定义日志记录格式
	logging::formatter formatter = expr::stream
        << "<" << std::setw(9)<< logLineId << ">"
        << "[" << expr::format_date_time(logTimeStamp, "%Y/%m/%d-%H:%M:%S.%f") << "]"
        << "[" << logSeverity << "]"
        << expr::message;

    // 定义日志文件夹
    boost::shared_ptr<sinks::file::collector> file_collector = sinks::file::make_collector(
        keywords::target = logConfig.m_strDirectory,            //文件夹名
        keywords::max_size = MAX_LOG_DIR_SIZE,         //文件夹所占最大空间
        keywords::min_free_space = MIN_LOG_FREE_SPACE  //磁盘最小预留空间
    );

    // 设置日志文件夹
    syncSink->locked_backend()->set_file_collector(file_collector);
    syncSinkErr->locked_backend()->set_file_collector(file_collector);
    asyncSink->locked_backend()->set_file_collector(file_collector);
    asyncSinkErr->locked_backend()->set_file_collector(file_collector);
	
    // 设置日志级别过滤
    syncSink->set_filter(logSeverity >= LogLevel(logConfig.m_llLevel) && logSeverity < LogLevel(WARNING)); 
    syncSinkErr->set_filter(logSeverity >= LogLevel(WARNING)); 
    asyncSink->set_filter(logSeverity >= LogLevel(logConfig.m_llLevel)&& logSeverity < LogLevel(WARNING)); 
    asyncSinkErr->set_filter(logSeverity >= LogLevel(WARNING)); 
	
    // 避免冲突，扫描其他实例建立的日志文件
    syncSink->locked_backend()->scan_for_files();
    syncSinkErr->locked_backend()->scan_for_files();
    asyncSink->locked_backend()->scan_for_files();
    asyncSinkErr->locked_backend()->scan_for_files();

    // 设置日志记录格式
    syncSink->set_formatter(formatter);
    syncSinkErr->set_formatter(formatter);
    asyncSink->set_formatter(formatter);
    asyncSinkErr->set_formatter(formatter);

    // 设置日志不自动刷新，以提高效率（默认值即为false,这里只是进行强调）
    syncSink->locked_backend()->auto_flush(false);
    syncSinkErr->locked_backend()->auto_flush(false);
    asyncSink->locked_backend()->auto_flush(false);
    asyncSinkErr->locked_backend()->auto_flush(false);

    // 添加属性
    logging::add_common_attributes();
    logging::core::get()->add_global_attribute("Scope", attrs::named_scope());

    // 溢出已有sink
    logging::core::get()->remove_all_sinks();

    // 设置/添加sink 异步sink效率较高
    if (logConfig.m_bAsync) { // 添加异步sink
        logging::core::get()->add_sink(asyncSink);
        logging::core::get()->add_sink(asyncSinkErr);
    }
    else{              // 添加同步sink
        logging::core::get()->add_sink(syncSink);
        logging::core::get()->add_sink(syncSinkErr);
    }

    // 添加控制台sink 控制台sink方便调试
    if (logConfig.m_bConsoleOut) {
        logging::add_console_log(std::clog,
            keywords::filter = (logSeverity >= LogLevel(logConfig.m_llLevel)),
            keywords::auto_flush = true,
            keywords::format = expr::stream
                << "[" << expr::format_date_time(logTimeStamp, "%Y/%m/%d-%H:%M:%S.%f") << "]"
                << "[" << logSeverity << "]"
                << expr::message
        );          
    }
    
    // 根据配置使能日志功能
    logging::core::get()->set_logging_enabled(logConfig.m_bLogging);   
}

void CLog::Initialize()
{
	InitLog(m_logConfig);
}

void CLog::Finalize()
{
	logging::core::get()->flush();
	logging::core::get()->remove_all_sinks();
}

/*
static unsigned long getThreadId()
{
	std::string strThreadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	unsigned long uThreadNumber = 0;
	sscanf(strThreadId.c_str(), "%lx", &uThreadNumber);
	return uThreadNumber;
}
*/
void CLog::Log(LogLevel level, long line, char const *file, char const *function, const char *format, ...)
{
    char message[MAX_LOG_MESSAGE_SIZE];

    // 处理变参
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    message[MAX_LOG_MESSAGE_SIZE-1] = '\0'; //防止缓冲区溢出
    //BOOST_LOG_SEV(m_boostLog, level) << "("  << getThreadId()
    //BOOST_LOG_SEV(m_boostLog, level) << "("  << (unsigned int)boost::hash< boost::thread::id >()(boost::this_thread::get_id())
    //BOOST_LOG_SEV(m_boostLog, level) << "("  << (unsigned int)std::hash< std::thread::id >()(std::this_thread::get_id())
    BOOST_LOG_SEV(m_boostLog, level) << "("  << std::this_thread::get_id()
								  	 << ")[" << file 
								     << ":"  << function
									 << "<"  << line
									 << ">]" << message;

	//输出后及时刷新日志缓存，及时的将日志写入到文件中
	logging::core::get()->flush();
}
