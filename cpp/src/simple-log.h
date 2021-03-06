//
//  simple-log.h
//  ndnrtc
//
//  Copyright 2013 Regents of the University of California
//  For licensing details see the LICENSE file.
//
//  Author:  Peter Gusev 
//  Created: 8/8/13
//

#ifndef __ndnrtc__simple__
#define __ndnrtc__simple__

#include <iostream>
#include <pthread.h>
#include <string>

#if !defined(NDN_LOGGING)
#undef NDN_TRACE
#undef NDN_INFO
#undef NDN_WARN
#undef NDN_ERROR
#undef NDN_DEBUG
#endif

// if defined detailed logging - print whole signature of the function.
//#if defined(NDN_DETAILED)
#define __NDN_FNAME__ __PRETTY_FUNCTION__
//#else
//#define __NDN_FNAME__ __func__
//#endif

// following macros are used for NdnRtcObject logging
// each macro checks, whether a logger, associated with object has been
// initialized and use it instead of global logger
#if defined (NDN_TRACE) //&& defined(DEBUG)
#define TRACE(fmt, ...) if (this->logger_) this->logger_->log(NdnLoggerLevelTrace, fmt, ##__VA_ARGS__); \
else NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelTrace, fmt, ##__VA_ARGS__)
#else
#define TRACE(fmt, ...)
#endif

#if defined (NDN_DEBUG) //&& defined(DEBUG)
#define DBG(fmt, ...) if (this->logger_) this->logger_->log(NdnLoggerLevelDebug, fmt, ##__VA_ARGS__); \
else NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelDebug, fmt, ##__VA_ARGS__)
#else
#define DBG(fmt, ...)
#endif

#if defined (NDN_INFO)
#define INFO(fmt, ...) if (this->logger_) this->logger_->log(NdnLoggerLevelInfo, fmt, ##__VA_ARGS__); \
else NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelInfo, fmt, ##__VA_ARGS__)
#else
#define INFO(fmt, ...)
#endif

#if defined (NDN_WARN)
#define WARN(fmt, ...) if (this->logger_) this->logger_->log(NdnLoggerLevelWarning, fmt, ##__VA_ARGS__); \
else NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelWarning, fmt, ##__VA_ARGS__)
#else
#define WARN(fmt, ...)
#endif

#if defined (NDN_ERROR)
#define NDNERROR(fmt, ...) if (this->logger_) this->logger_->log(NdnLoggerLevelError, fmt, ##__VA_ARGS__); \
else NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelError, fmt, ##__VA_ARGS__)
#else
#define NDNERROR(fmt, ...)
#endif

// following macros are used for logging usign global logger
#if defined (NDN_TRACE) //&& defined(DEBUG)
#define LOG_TRACE(fmt, ...) NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelTrace, fmt, ##__VA_ARGS__)
#else
#define LOG_TRACE(fmt, ...)
#endif

#if defined (NDN_DEBUG) //&& defined(DEBUG)
#define LOG_DBG(fmt, ...) NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelDebug, fmt, ##__VA_ARGS__)
#else
#define LOG_DBG(fmt, ...)
#endif

#if defined (NDN_INFO)
#define LOG_INFO(fmt, ...) NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelInfo, fmt, ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)
#endif

#if defined (NDN_WARN)
#define LOG_WARN(fmt, ...) NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelWarning, fmt, ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)
#endif

#if defined (NDN_ERROR)
#define LOG_NDNERROR(fmt, ...) NdnLogger::log(__NDN_FNAME__, NdnLoggerLevelError, fmt, ##__VA_ARGS__)
#else
#define LOG_NDNERROR(fmt, ...)
#endif

namespace ndnlog {
    typedef enum _NdnLoggerLevel {
        NdnLoggerLevelTrace = 0,
        NdnLoggerLevelDebug = 1,
        NdnLoggerLevelInfo = 2,
        NdnLoggerLevelWarning = 3,
        NdnLoggerLevelError = 4
    } NdnLoggerLevel;
    
    typedef enum _NdnLoggerDetailLevel {
        NdnLoggerDetailLevelNone = NdnLoggerLevelError+1,
        NdnLoggerDetailLevelDefault = NdnLoggerLevelInfo,
        NdnLoggerDetailLevelDebug = NdnLoggerLevelDebug,
        NdnLoggerDetailLevelAll = NdnLoggerLevelTrace
    } NdnLoggerDetailLevel;
    
    /**
     * Thread-safe logger class to stdout or file
     */
    class NdnLogger
    {
    public:
        // construction/desctruction
        NdnLogger(NdnLoggerDetailLevel logDetailLevel, const char *logFileFmt, ...);
        NdnLogger(const char *logFile = NULL, NdnLoggerDetailLevel logDetailLevel = NdnLoggerDetailLevelDefault);
        ~NdnLogger();
        
        // public static attributes go here
        
        // public static methods go here
        static void initialize(const char *logFile, NdnLoggerDetailLevel logDetailLevel);
        static void log(const char *fName, NdnLoggerLevel level, const char *format, ...);
        static std::string currentLogFile(); // returns "" if log to stdout
        static NdnLoggerDetailLevel currentLogLevel();

        // public attributes go here
        
        // public methods go here 
        void logString(const char *str);
        void log(NdnLoggerLevel level, const char *format, ...);
    private:
        // private static attributes go here
        
        // private static methods go here
        static NdnLogger* getInstance();
        static const char* stingify(NdnLoggerLevel lvl);
        
        // private attributes go here
        FILE *outLogStream_;
        char *buf_;
        NdnLoggerDetailLevel loggingDetailLevel_;
        std::string logFile_;
        int64_t lastFileFlush_;
        
        static pthread_mutex_t logMutex_;
        pthread_mutex_t instanceMutex_;
        
        // private methods go here
        void log(const char *str);
        void flushBuffer(char *buf);
        char* getBuffer(){ return buf_; }
        NdnLoggerDetailLevel getLoggingDetailLevel() { return loggingDetailLevel_; }
        int64_t millisecondTimestamp();
    };
    
    /**
     * Abstract class for an object which can print logs into its' logger if 
     * it is initialized
     */
    class LoggerObject
    {
    public:
        
        /**
         * Default constructor
         */
        LoggerObject(){}
        
        /**
         * Initializes object and creates a logger to file using file name 
         * provided. Logger will be deleted in destruction process.
         *
         * @param logFile Log file name. File could exist or not, depending on
         * case, logger will overwrite existing or create a new file
         */
        LoggerObject(const char *logFile) : isLoggerCreated_(true) {
            logger_ = new NdnLogger(logFile);
        }
        
        /**
         * Initializes object with existing logger. Logger will not be deleted 
         * in destruction process.
         * 
         * @param logger Logger which will be used for logging of the object
         * being created
         */
        LoggerObject(NdnLogger *logger) : logger_(logger) {}
        
        virtual ~LoggerObject() {
            if (isLoggerCreated_)
                delete logger_;
        }
        
        /**
         * Returns current logger of the object
         * @return Current logger
         */
        NdnLogger* getLogger() { return logger_; }
        
        /**
         * Sets a logger only if it was not previously created. This logger 
         * will not be deleted upon object destruction
         * @param logger Logger which will be used for logging by the object
         */
        virtual void setLogger(NdnLogger *logger) {
            if (!isLoggerCreated_)
                logger_ = logger;
        }
        
    protected:
        bool isLoggerCreated_ = false;
        ndnlog::NdnLogger *logger_ = nullptr;
        
        /**
         * Initializes logger with filename composed by parameters passed. 
         * Resulting filename should not exceed 256 symbols.
         */
//        void initializeLogger(const char *format, ...);
    };
}

#endif /* defined(__ndnrtc__simple__) */
