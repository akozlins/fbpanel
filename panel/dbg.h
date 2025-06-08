
#include <stdio.h>

#define ERR(fmt, ...) \
    fprintf(stderr, fmt, ##__VA_ARGS__)
#define DBG2(fmt, ...) \
    fprintf(stderr, "%s:%s:%-5d: " fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define DBGE2(fmt, ...) \
    fprintf(stderr, fmt, ##__VA_ARGS__)
#define ENTER2 do { \
    fprintf(stderr, "%s:%s:%-5d: ENTER\n",  __FILE__, __func__, __LINE__); \
    } while(0)
#define RET2(...) do { \
    fprintf(stderr, "%s:%s:%-5d: RETURN\n",  __FILE__, __func__, __LINE__); \
    return __VA_ARGS__; \
    } while(0)

enum { LOG_NONE, LOG_ERR, LOG_WARN, LOG_INFO, LOG_DEBUG, LOG_ALL };

#ifdef DEBUGPRN

#define ENTER do { \
    fprintf(stderr, "%s:%s:%-5d: ENTER\n", __FILE__, __func__, __LINE__); \
    } while(0)
#define RET(...) do { \
    fprintf(stderr, "%s:%s:%-5d: RETURN\n", __FILE__, __func__, __LINE__); \
    return __VA_ARGS__; \
    } while(0)
#define DBG(fmt, ...) \
    fprintf(stderr, "%s:%s:%-5d: " fmt,  __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define DBGE(fmt, ...) \
    fprintf(stderr, fmt, ##__VA_ARGS__)
#define LOG(level, fmt, ...) \
    fprintf(stderr, fmt, ##__VA_ARGS__)

#else

extern int log_level;
#define ENTER do { \
    } while(0)
#define RET(...) \
    return __VA_ARGS__
#define DBG(fmt, ...) do { \
    } while(0)
#define DBGE(fmt, ...) do { \
    } while(0)
#define LOG(level, fmt, ...) do { \
    if (level <= log_level) fprintf(stderr, fmt, ##__VA_ARGS__); \
    } while(0)

#endif
