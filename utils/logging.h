#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdarg.h>
#include <stdio.h>

#include "macros.h"

#ifdef OMPI_MPI_H
#define MPI_H_
#endif

#ifdef MPI_H_
#include <mpi.h>
#endif

#ifndef LOGGINGDEF
#define LOGGINGDEF static inline
#endif // LOGGINGDEF

typedef enum {
    LOG_DEBUG = 1,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} log_t;

static log_t log_level = LOG_INFO;

LOGGINGDEF Cstr *log_to_cstr(log_t log);
LOGGINGDEF void set_log_level(log_t new_level);
LOGGINGDEF void base_log(log_t level, int err, Cstr *message, ...);

#define log_message(level, err, message, ...)\
    if((level) >= log_level) base_log(level, err, message, __VA_ARGS__)

#define log_debug(...) log_message(LOG_DEBUG,0 , __VA_ARGS__, NULL)
#define log_info(...) log_message(LOG_INFO,0 , __VA_ARGS__, NULL)
#define log_warning(...) log_message(LOG_WARNING,0 , __VA_ARGS__, NULL)
#define log_error(...) log_message(LOG_ERROR,0 , __VA_ARGS__, NULL)
#define log_fatal(...) base_log(LOG_FATAL,-1, __VA_ARGS__) // return utily error
#define log_fatal_err(err, ...) base_log(LOG_FATAL,(err), __VA_ARGS__) // return custom error

#define ERR_MGS log_fatal_err(__result, "check error code %s:%d",__FILE__, __LINE__)

#define Control(f)\
    do {\
        int __result = (f);\
        if(__result != 0) {\
            ERR_MGS;\
        }\
    }while(0)

#define fatal_if(condition, ...)\
    if(condition) log_fatal(__VA_ARGS__)

#define fatal_if_err(condition,err, ...)\
    if(condition) log_fatal_err(err, __VA_ARGS__)

/* ---------------------- IMPLEMENTATION ---------------------- */

Cstr *log_to_cstr(log_t log) {
    switch (log) {
    case LOG_DEBUG:
        return "DEBUG";
    case LOG_INFO:
        return "INFO";
    case LOG_WARNING:
        return "WARNING";
    case LOG_ERROR:
        return "ERROR";
    case LOG_FATAL:
        return "FATAL ERROR";
    default:
        log_fatal_err(-1 ,"log type %d does not exists", log);
    }
    return NULL;
}

void set_log_level(log_t new_level) { log_level = new_level; }

void base_log(log_t level, int err, Cstr *message, ...) {

    va_list ap;
    va_start(ap, message);

    Cstr *log_type = log_to_cstr(level);

    fprintf(stderr, "%s: ", log_type);
    vfprintf(stderr, message, ap);
    putc('\n', stderr);

    va_end(ap);

    if(level == LOG_FATAL) {
    #ifdef MPI_H_
        MPI_Abort(MPI_COMM_WORLD, err);
    #else
        exit(err);
    #endif // MPI_H_
    }
}

#endif // LOGGING_H_