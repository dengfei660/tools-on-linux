#ifndef __TOOLS_LOG_H__
#define __TOOLS_LOG_H__

#ifdef  __cplusplus
extern "C" {
#endif

void logPrint( int level, const char *fmt, ... );

void Logger_set_level(int setLevel);

#define INT_FATAL(FORMAT, ...)      logPrint(0, "FATAL: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_ERROR(FORMAT, ...)      logPrint(0, "ERROR: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_WARNING(FORMAT, ...)    logPrint(1, "WARN:  %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_INFO(FORMAT, ...)       logPrint(2, "INFO:  %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_DEBUG(FORMAT, ...)      logPrint(3, "DEBUG: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_TRACE1(FORMAT, ...)     logPrint(4, "TRACE: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_TRACE2(FORMAT, ...)     logPrint(5, "TRACE: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_TRACE3(FORMAT, ...)     logPrint(6, "TRACE: %s,%s:%d " FORMAT "\n",TAG,__FILE__, __LINE__, __VA_ARGS__)

#define FATAL(...)                  INT_FATAL(__VA_ARGS__, "")
#define ERROR(...)                  INT_ERROR(__VA_ARGS__, "")
#define WARNING(...)                INT_WARNING(__VA_ARGS__, "")
#define INFO(...)                   INT_INFO(__VA_ARGS__, "")
#define DEBUG(...)                  INT_DEBUG(__VA_ARGS__, "")
#define TRACE1(...)                 INT_TRACE1(__VA_ARGS__, "")
#define TRACE2(...)                 INT_TRACE2(__VA_ARGS__, "")
#define TRACE3(...)                 INT_TRACE3(__VA_ARGS__, "")

#ifdef  __cplusplus
}
#endif

#endif /*__TOOLS_LOG_H__"*/"