#ifndef __TOOLS_LOGGER_H__
#define __TOOLS_LOGGER_H__


#define LOG_LEVEL_FATAL   0
#define LOG_LEVEL_ERROR   0
#define LOG_LEVEL_WARNING 1
#define LOG_LEVEL_INFO    2
#define LOG_LEVEL_DEBUG   3
#define LOG_LEVEL_TRACE1  4
#define LOG_LEVEL_TRACE2  5
#define LOG_LEVEL_TRACE3  6

#define NO_CATEGERY -1

void Logger_init();

void logPrint(int categery, int level, const char *fmt, ... );

/**
 * @brief set log level
 * the log level from 0 ~ 6
 * @param setLevel the log level
 * 0 - FATL / ERROR
 * 1.- WARN
 * 2.- INFO
 * 3.- DEBUG
 * 4.- TRACE1
 * 5.- TRACE2
 * 6.- TRACE3
 */
void Logger_set_level(int setLevel);

/**
 * @brief get loger level
 *
 * @return int the log level from 0~6
 */
int Logger_get_level();

/**
 * @brief set a user defined tag to print log
 * if set userTag to NULL, the user tag will be removed
 *
 * @return the category of this object print
 */
int Logger_set_userTag(size_t object, char *userTag);

/**
 * @brief set log file,the filepath must is a absolute path,
 * if set filepath null, will close file and print log to stderr
 * the stderr is a default print out file
 * @param filepath the file absolute path
 */
void Logger_set_file(char *filepath);

#define INT_FATAL(CAT,FORMAT, ...)      logPrint(CAT,LOG_LEVEL_FATAL, "FATAL: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_ERROR(CAT,FORMAT, ...)      logPrint(CAT,LOG_LEVEL_FATAL, "ERROR: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_WARNING(CAT,FORMAT, ...)    logPrint(CAT,LOG_LEVEL_WARNING, " WARN: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_INFO(CAT,FORMAT, ...)       logPrint(CAT,LOG_LEVEL_INFO, " INFO: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_DEBUG(CAT,FORMAT, ...)      logPrint(CAT,LOG_LEVEL_DEBUG, "DEBUG: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_TRACE1(CAT,FORMAT, ...)     logPrint(CAT,LOG_LEVEL_TRACE1, "TRACE1: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_TRACE2(CAT,FORMAT, ...)     logPrint(CAT,LOG_LEVEL_TRACE2, "TRACE2: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_TRACE3(CAT,FORMAT, ...)     logPrint(CAT,LOG_LEVEL_TRACE3, "TRACE3: %s,%s:%d " FORMAT "\n",TAG,__func__, __LINE__, __VA_ARGS__)
#define INT_TRACE4(CAT,FORMAT, ...)     logPrint(CAT,LOG_LEVEL_TRACE3, "TRACE4: %s,%s:%d " FORMAT "\n",TAG,__FILE__, __LINE__, __VA_ARGS__)

#define FATAL_CAT(CAT,...)                  INT_FATAL(CAT,__VA_ARGS__, "")
#define ERROR_CAT(CAT,...)                  INT_ERROR(CAT,__VA_ARGS__, "")
#define WARNING_CAT(CAT,...)                INT_WARNING(CAT,__VA_ARGS__, "")
#define INFO_CAT(CAT,...)                   INT_INFO(CAT,__VA_ARGS__, "")
#define DEBUG_CAT(CAT,...)                  INT_DEBUG(CAT,__VA_ARGS__, "")
#define TRACE1_CAT(CAT,...)                 INT_TRACE1(CAT,__VA_ARGS__, "")
#define TRACE2_CAT(CAT,...)                 INT_TRACE2(CAT,__VA_ARGS__, "")
#define TRACE3_CAT(CAT,...)                 INT_TRACE3(CAT,__VA_ARGS__, "")

#define FATAL(...)                  INT_FATAL(NO_CATEGERY,__VA_ARGS__, "")
#define ERROR(...)                  INT_ERROR(NO_CATEGERY,__VA_ARGS__, "")
#define WARNING(...)                INT_WARNING(NO_CATEGERY,__VA_ARGS__, "")
#define INFO(...)                   INT_INFO(NO_CATEGERY,__VA_ARGS__, "")
#define DEBUG(...)                  INT_DEBUG(NO_CATEGERY,__VA_ARGS__, "")
#define TRACE1(...)                 INT_TRACE1(NO_CATEGERY,__VA_ARGS__, "")
#define TRACE2(...)                 INT_TRACE2(NO_CATEGERY,__VA_ARGS__, "")
#define TRACE3(...)                 INT_TRACE3(NO_CATEGERY,__VA_ARGS__, "")

#endif /*__TOOLS_LOGGER_H__*/