## Utilities for C

Utilities implemented in style [stb-style single-file library](https://github.com/nothings/stb).

To compile the project with these header files, simply include them in the files, without specifying a source file to the compiler.

To achieve this, here's an example of main.c:

```C
#include "utils/macros.h"
#include "utils/strings.h"
#include "utils/arena.h"
#include "utils/logging.h"
#include "utils/random.h"
#include "utils/matrix.h"

int main(void) {
    Arena a = {0};
    String_Builder sb = arena_sb_from_cstr(&a,"foo");
    arena_sb_append_cstr(&sb, &a,"bar");
    sb_to_uppercase(&sb);
    append(&sb, 'c');
    arena_sb_to_cstr(&sb, &a);
    log_info("%s", sb.data);
    arena_free(&a);

    init_random();
    int *mtx = generate_random_matrix(10,5,-5,5);
    free(mtx);
}
```
**Note:** you need the firts two macros for importing the implementation.

Move the include directory on your project and run:

```console
$ gcc -Wall -Wextra -o main -I ./include main.c 
```
**Inspired by:** [nob.h](https://github.com/tsoding/musializer/blob/master/nob.h) ,[arena](https://github.com/tsoding/arena).