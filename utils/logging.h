#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdarg.h>
#include <stdio.h>

#include "macros.h"
#include "strings.h"

#ifdef OMPI_MPI_H
#define MPI_H_
#endif

#ifdef MPI_H_
#include <mpi.h>
#endif 

typedef enum {
    LOG_DEBUG = 1,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
} log_t;

//TODO: da warnings
static log_t log_level = LOG_INFO;

void set_log_level(log_t new_level);
void log_message(log_t level, int err_val, Cstr *message, ...);

#define log_debug(...) log_message(LOG_DEBUG,0 , __VA_ARGS__)
#define log_info(...) log_message(LOG_INFO,0 , __VA_ARGS__)
#define log_warning(...) log_message(LOG_WARNING,0 , __VA_ARGS__)
#define log_error(...) log_message(LOG_ERROR,0 , __VA_ARGS__)
#define log_fatal(err, ...) log_message(LOG_FATAL,(err), __VA_ARGS__)

#ifdef MPI_H_
#define Control(mpi)\
    do {\
        int result = (mpi);\
        if(result != MPI_SUCCESS) {\
            log_fatal(result, "MPI failed miserably, check error code");\
        }\
    }while(0)
#endif // MPI_H_

#ifdef LOGGING_IMPLEMENTATION

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
            log_fatal(-1 ,"log type %d does not exists", log);
            UNREACHABLE
        }
    }

    void set_log_level(log_t new_level) { log_level = new_level; }

    void log_message(log_t level, int err_value, Cstr *message, ...) {
        
        if(level < log_level) return;

        va_list ap;
        va_start(ap, message);

        Cstr *log_type = log_to_cstr(level);

        fprintf(stderr, "%s: ", log_type);
        vfprintf(stderr, message, ap);
        putc('\n', stderr);

        va_end(ap);

        if(level == LOG_FATAL) {
        #ifdef MPI_H_
            MPI_Abort(MPI_COMM_WORLD, err_value);
        #else
            exit(err_value);
        #endif // MPI_H_
        }
    }




#endif // LOGGING_IMPLEMENTATION

#endif // LOGGING_H_