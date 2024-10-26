#ifndef RANDOM_H_
#define RANDOM_H_

#include <stdlib.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "logging.h"
#include "macros.h"

#ifndef RANDOMDEF
#define RANDOMDEF static inline
#endif // RANDOMDEF

/*
    Inizializza il modulo dei numeri randomici.
    @return seed usato per il generatore di interi randomici.
    @note Viene già fatto il logging del seed dentro questa funzione.
*/
RANDOMDEF unsigned int init_random(void);

/*
    distribuzione uniforme di interi partendo da min fino a max entrambi compresi
    @return min <= result <= max
*/
RANDOMDEF int uniform_int_distribution(int min, int max);

/*
    distribuzione uniforme di numeri reali partendo da min fino a max entrambi compresi
    @return min <= result <= max
*/
RANDOMDEF double uniform_real_distribution(double min, double max);

/*
    @return Numero casuale tra 0 e 1
*/
RANDOMDEF double random_01(void);


#endif // RANDOM_H_

#ifdef RANDOM_IMPLEMENTATION
#undef RANDOM_IMPLEMENTATION 

unsigned int init_random(void) {

    int fd = open("/dev/urandom", O_RDONLY);
    fatal_if(fd < 0, "syscall error: %s", strerror(errno));

    unsigned int seed = 0;
    int err = read(fd, &seed, sizeof(seed));
    if(err == -1) log_fatal("syscall error: %s", strerror(errno));
    if(close(fd) == -1) {
        log_fatal("syscall error: %s", strerror(errno));
    }
    log_info("Seed = %u", seed);
    srand(seed);
    return seed;
}

int uniform_int_distribution(int min, int max) {
    return min + rand() % (max - min + 1);
}

double uniform_real_distribution(double min, double max) {
    return min + (double)rand() / RAND_MAX * (max - min);
}

double random_01(void) {
    return (double)rand() / RAND_MAX;
}

#endif // RANDOM_IMPLEMENTATION 