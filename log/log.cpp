#include"log.h"
#include<memory>
#include<functional>
namespace fxk {

	void LogEvent::format(const char* fmt, ...) {
		va_list al;
		va_start(al, fmt);
		format(fmt, al);
		va_end(al);
	}
	void LogEvent::format(const char* fmt, va_list al)
	{
		char* buf = nullptr;
		int len = vsprintf(buf, fmt, al);
		if (len != -1) {
			m_ss << std::string(buf, len);
			free(buf);
		}
	}
	std::stringstream& LogEventWrap::getSS() {
		return m_event->getSS();
	}
	//设置日志格式
	void LogAppender::setFormatter(LogFormatter::ptr val) {
		MutexType::Lock lock(m_mutex);
		m_formatter = val;
		if (m_formatter)
		{
			m_hasFormatter = true;
		}
		else {
			m_hasFormatter = false;
		}

	}
	//日志输出地 获取输出格式
	LogFormatter::ptr LogAppender::getFormatter()
	{
		MutexType::Lock lock(m_mutex);
		return m_formatter;
	}
	LogEvent::LogEvent(std::shared_ptr<Logger>logger,LogLevel::Level level,const char*file,int32_t line,uint32_t elapse,uint32_t thread_id,uint32_t fiber_id,uint64_t time,const std::string&thread_name):m_file(file)
		,m_line(line),m_elapse(elapse),m_threadId(thread_id),m_fiberId(fiber_id),m_time(time),m_threadName(thread_name),m_logger(logger),m_level(level){}

	Logger::Logger(const std::string& name) : m_name(name), m_level(LogLevel::DEBUG) {
		m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M;%S}[%rms]%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
	}
	void Logger::setFormatter(LogFormatter::ptr val) {
		MutexType::Lock lock(m_mutex);
		m_formatter = val;
		for (auto& i : m_appenders)
		{
			MutexType::Lock ll(i->m_mutex);
			if (!i->m_hasFormatter)
			{
				i->m_formatter = m_formatter;
			}
		}
	}
	void Logger::clearAppenders()
	{
		MutexType::Lock lock(m_mutex);
		m_appenders.clear();
	}
	void Logger::log(LogLevel::Level level, LogEvent::ptr event) {

		if (level >= m_level) {
			auto self = shared_from_this();
			MutexType::Lock lock(m_mutex);
			if (!m_appenders.empty())
			{for(auto& i:m_appenders)
				i->log(self, level, event);
			}
			else if (m_root) {
				m_root->log(level, event);
			}
		}
	}
	Logger:: Logger(const std::string& name = "root") {

	}
	void Logger::debug(LogEvent::ptr event){}
	void Logger::info(LogEvent::ptr event){}
	void Logger::warn(LogEvent::ptr event){}
	void Logger::error(LogEvent::ptr event){}
	void Logger::fatal(LogEvent::ptr event){}
	void Logger::addLogAppender(LogAppender::ptr appender){
		m_appenders.push_back(appender);
	}
	void Logger::delLogAppender(LogAppender::ptr appender){
		for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {

			if (*it == appender) {
				m_appenders.erase(it);
				break;
			}
	}
	}
	void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event){}
	void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {}

}

