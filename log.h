#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "util.h"
#include "singleton.h"
#include "thread.h"

/**
 * @brief ʹ����ʽ��ʽ����־����level����־д�뵽logger
 */
#define SYLAR_LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(),\
                sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getSS()

 /**
  * @brief ʹ����ʽ��ʽ����־����debug����־д�뵽logger
  */
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)

  /**
   * @brief ʹ����ʽ��ʽ����־����info����־д�뵽logger
   */
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)

   /**
    * @brief ʹ����ʽ��ʽ����־����warn����־д�뵽logger
    */
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)

    /**
     * @brief ʹ����ʽ��ʽ����־����error����־д�뵽logger
     */
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)

     /**
      * @brief ʹ����ʽ��ʽ����־����fatal����־д�뵽logger
      */
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

      /**
       * @brief ʹ�ø�ʽ����ʽ����־����level����־д�뵽logger
       */
#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, sylar::GetThreadId(),\
                sylar::GetFiberId(), time(0), sylar::Thread::GetName()))).getEvent()->format(fmt, __VA_ARGS__)

       /**
        * @brief ʹ�ø�ʽ����ʽ����־����debug����־д�뵽logger
        */
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)

        /**
         * @brief ʹ�ø�ʽ����ʽ����־����info����־д�뵽logger
         */
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)

         /**
          * @brief ʹ�ø�ʽ����ʽ����־����warn����־д�뵽logger
          */
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)

          /**
           * @brief ʹ�ø�ʽ����ʽ����־����error����־д�뵽logger
           */
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)

           /**
            * @brief ʹ�ø�ʽ����ʽ����־����fatal����־д�뵽logger
            */
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

            /**
             * @brief ��ȡ����־��
             */
#define SYLAR_LOG_ROOT() sylar::LoggerMgr::GetInstance()->getRoot()

             /**
              * @brief ��ȡname����־��
              */
#define SYLAR_LOG_NAME(name) sylar::LoggerMgr::GetInstance()->getLogger(name)

namespace sylar {

    class Logger;
    class LoggerManager;

    /**
     * @brief ��־����
     */
    class LogLevel {
    public:
        /**
         * @brief ��־����ö��
         */
        enum Level {
            /// δ֪����
            UNKNOW = 0,
            /// DEBUG ����
            DEBUG = 1,
            /// INFO ����
            INFO = 2,
            /// WARN ����
            WARN = 3,
            /// ERROR ����
            ERROR = 4,
            /// FATAL ����
            FATAL = 5
        };

        /**
         * @brief ����־����ת���ı����
         * @param[in] level ��־����
         */
        static const char* ToString(LogLevel::Level level);

        /**
         * @brief ���ı�ת������־����
         * @param[in] str ��־�����ı�
         */
        static LogLevel::Level FromString(const std::string& str);
    };

    /**
     * @brief ��־�¼�
     */
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
       
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level
            , const char* file, int32_t line, uint32_t elapse
            , uint32_t thread_id, uint32_t fiber_id, uint64_t time
            , const std::string& thread_name);

        /**
         * @brief �����ļ���
         */
        const char* getFile() const { return m_file; }

        /**
         * @brief �����к�
         */
        int32_t getLine() const { return m_line; }

        /**
         * @brief ���غ�ʱ
         */
        uint32_t getElapse() const { return m_elapse; }

        /**
         * @brief �����߳�ID
         */
        uint32_t getThreadId() const { return m_threadId; }

        /**
         * @brief ����Э��ID
         */
        uint32_t getFiberId() const { return m_fiberId; }

        /**
         * @brief ����ʱ��
         */
        uint64_t getTime() const { return m_time; }

        /**
         * @brief �����߳�����
         */
        const std::string& getThreadName() const { return m_threadName; }

        /**
         * @brief ������־����
         */
        std::string getContent() const { return m_ss.str(); }

        /**
         * @brief ������־��
         */
        std::shared_ptr<Logger> getLogger() const { return m_logger; }

        /**
         * @brief ������־����
         */
        LogLevel::Level getLevel() const { return m_level; }

        /**
         * @brief ������־�����ַ�����
         */
        std::stringstream& getSS() { return m_ss; }

        /**
         * @brief ��ʽ��д����־����
         */
        void format(const char* fmt, ...);

        /**
         * @brief ��ʽ��д����־����
         */
        void format(const char* fmt, va_list al);
    private:
        /// �ļ���
        const char* m_file = nullptr;
        /// �к�
        int32_t m_line = 0;
        /// ����������ʼ�����ڵĺ�����
        uint32_t m_elapse = 0;
        /// �߳�ID
        uint32_t m_threadId = 0;
        /// Э��ID
        uint32_t m_fiberId = 0;
        /// ʱ���
        uint64_t m_time = 0;
        /// �߳�����
        std::string m_threadName;
        /// ��־������
        std::stringstream m_ss;
        /// ��־��
        std::shared_ptr<Logger> m_logger;
        /// ��־�ȼ�
        LogLevel::Level m_level;
    };

    /**
     * @brief ��־�¼���װ��
     */
    class LogEventWrap {
    public:

        /**
         * @brief ���캯��
         * @param[in] e ��־�¼�
         */
        LogEventWrap(LogEvent::ptr e);

        /**
         * @brief ��������
         */
        ~LogEventWrap();

        /**
         * @brief ��ȡ��־�¼�
         */
        LogEvent::ptr getEvent() const { return m_event; }

        /**
         * @brief ��ȡ��־������
         */
        std::stringstream& getSS();
    private:
        /**
         * @brief ��־�¼�
         */
        LogEvent::ptr m_event;
    };

    /**
     * @brief ��־��ʽ��
     */
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        /**
         * @brief ���캯��
         * @param[in] pattern ��ʽģ��
         * @details
         *  %m ��Ϣ
         *  %p ��־����
         *  %r �ۼƺ�����
         *  %c ��־����
         *  %t �߳�id
         *  %n ����
         *  %d ʱ��
         *  %f �ļ���
         *  %l �к�
         *  %T �Ʊ��
         *  %F Э��id
         *  %N �߳�����
         *
         *  Ĭ�ϸ�ʽ "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
         */
        LogFormatter(const std::string& pattern);

        /**
         * @brief ���ظ�ʽ����־�ı�
         * @param[in] logger ��־��
         * @param[in] level ��־����
         * @param[in] event ��־�¼�
         */
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        std::ostream& format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    public:

        
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
           
            virtual ~FormatItem() {}
           
            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

      
        void init();

       
        bool isError() const { return m_error; }

       
        const std::string getPattern() const { return m_pattern; }
    private:
        /// ��־��ʽģ��
        std::string m_pattern;
        /// ��־��ʽ�������ʽ
        std::vector<FormatItem::ptr> m_items;
        /// �Ƿ��д���
        bool m_error = false;

    };

    /**
     * @brief ��־���Ŀ��
     */
    class LogAppender {
        friend class Logger;
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        typedef Spinlock MutexType;

       
        virtual ~LogAppender() {}

        
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

       
        virtual std::string toYamlString() = 0;

        void setFormatter(LogFormatter::ptr val);

       
        LogFormatter::ptr getFormatter();

        /**
         * @brief ��ȡ��־����
         */
        LogLevel::Level getLevel() const { return m_level; }

        /**
         * @brief ������־����
         */
        void setLevel(LogLevel::Level val) { m_level = val; }
    protected:
        /// ��־����
        LogLevel::Level m_level = LogLevel::DEBUG;
        /// �Ƿ����Լ�����־��ʽ��
        bool m_hasFormatter = false;
        /// Mutex
        MutexType m_mutex;
        /// ��־��ʽ��
        LogFormatter::ptr m_formatter;
    };

   
    class Logger : public std::enable_shared_from_this<Logger> {
        friend class LoggerManager;
    public:
        typedef std::shared_ptr<Logger> ptr;
        typedef Spinlock MutexType;

        
        Logger(const std::string& name = "root");

       
        void log(LogLevel::Level level, LogEvent::ptr event);

        
        void debug(LogEvent::ptr event);

        /**
         * @brief дinfo������־
         * @param[in] event ��־�¼�
         */
        void info(LogEvent::ptr event);

        
        void warn(LogEvent::ptr event);

        
        void error(LogEvent::ptr event);

       
        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);

        void delAppender(LogAppender::ptr appender);

        void clearAppenders();

        
        LogLevel::Level getLevel() const { return m_level; }

        
        void setLevel(LogLevel::Level val) { m_level = val; }

        /**
         * @brief ������־����
         */
        const std::string& getName() const { return m_name; }

       
        void setFormatter(LogFormatter::ptr val);

       
        void setFormatter(const std::string& val);

       
        LogFormatter::ptr getFormatter();

        std::string toYamlString();
    private:
        /// ��־����
        std::string m_name;
        /// ��־����
        LogLevel::Level m_level;
        /// Mutex
        MutexType m_mutex;
        /// ��־Ŀ�꼯��
        std::list<LogAppender::ptr> m_appenders;
        /// ��־��ʽ��
        LogFormatter::ptr m_formatter;
        /// ����־��
        Logger::ptr m_root;
    };

    /**
     * @brief ���������̨��Appender
     */
    class StdoutLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;
    };

    /**
     * @brief ������ļ���Appender
     */
    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
        std::string toYamlString() override;

        /**
         * @brief ���´���־�ļ�
         * @return �ɹ�����true
         */
        bool reopen();
    private:
        /// �ļ�·��
        std::string m_filename;
        /// �ļ���
        std::ofstream m_filestream;
        /// �ϴ����´�ʱ��
        uint64_t m_lastTime = 0;
    };

    /**
     * @brief ��־��������
     */
    class LoggerManager {
    public:
        typedef Spinlock MutexType;
       
        LoggerManager();

       
        Logger::ptr getLogger(const std::string& name);

       
        void init();

        Logger::ptr getRoot() const { return m_root; }

        std::string toYamlString();
    private:
        /// Mutex
        MutexType m_mutex;
        /// ��־������
        std::map<std::string, Logger::ptr> m_loggers;
        /// ����־��
        Logger::ptr m_root;
    };

    /// ��־�������൥��ģʽ
    typedef sylar::Singleton<LoggerManager> LoggerMgr;

}

#endif