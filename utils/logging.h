#ifndef LOGGING_H_
#define LOGGING_H_

#include <stdarg.h>
#include <stdio.h>

#include "strings.h"

typedef enum {
    LOG_DEBUG = 1,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
} log_t;

static log_t log_level = LOG_INFO;

void set_log_level(log_t new_level);
void log_message(log_t level, Cstr *message, ...);

#define log_debug(...) log_message(LOG_DEBUG, __VA_ARGS__)
#define log_info(...) log_message(LOG_INFO, __VA_ARGS__)
#define log_warning(...) log_message(LOG_WARNING, __VA_ARGS__)
#define log_error(...) log_message(LOG_ERROR, __VA_ARGS__)
#define log_fatal(...) log_message(LOG_FATAL, __VA_ARGS__)

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
            log_fatal("log type %d does not exists", log);
            assert(false && "Unreachable code");
        }
    }

    void set_log_level(log_t new_level) { log_level = new_level; }

    void log_message(log_t level, Cstr *message, ...) {
        
        if(level < log_level) return;

        va_list ap;
        va_start(ap, message);

        Cstr *log_type = log_to_cstr(level);

        fprintf(stderr, "%s: ", log_type);
        vfprintf(stderr, message, ap);
        putc('\n', stderr);

        va_end(ap);

        if(level == LOG_FATAL) exit(1);
    }


#endif // LOGGING_IMPLEMENTATION


#endif // LOGGING_H_