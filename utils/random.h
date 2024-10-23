#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdlib.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "logging.h"
#include "macros.h"

void init_random(void) {

    int fd = open("/dev/urandom", O_RDONLY);
    fatal_if(fd < 0, "syscall error: %s", strerror(errno));

    unsigned int seed = 0;
    read(fd, &seed, sizeof(seed));
    if(close(fd) == -1) {
        log_fatal("syscall error: %s", strerror(errno));
    }
    log_info("Seed = %u", seed);
    srand(seed);
}

#endif // RANDOM_H_