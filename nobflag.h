/* nobflag.h - https://github.com/auribuo/nobflag.h - Licensed under MIT. See end of the file for the full license
# nobflag.h

`nobflag.h` is an extension to [`nob.h`](https://github.com/tsoding/nob.h) that adds two main features.

You still need `nob.h` in your project.

- `nob_da_swap` macro to swap two elements in a dynamic array
- Boolean flag argument parsing

## Reasoning

A build system often has some boolean flags needed ex. enable static linking or debug information.

Obviously nob is not a commandline parser and a build system should not but the need of such simple flags arises
more often than not. Therefore my idea is to provide a very simple interface to quickly add the required flags.

### Example

```c
#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIXES
// #define NOB_CMD_FLAGS_CONSUME_HYPHEN // Define this macro if you want '-' to be consumed as flag
#include "nobflag.h" // nobflag.h already includes "nob.h" so you don't need (and with NOB_IMPLEMENTATION cannot) include the header

static bool debug = false;

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd_Flags flags = {0};
    nob_cmd_flags_add(&flags, debug);

    // This function consumes argv[0] so if you need the program name make sure to save the pointer before.
    // The function parses argv until something that is not -[flag] is encountered. argc and argv then contain the rest of the args.
    // The function "removes" the parsed flags from Nob_Cmd_Flags and returns the amount of items matched.
    // The ones left in the array are the ones never encountered.
    size_t parsed = nob_cmd_flags_parse(&flags, &argc, &argv);

    nob_log(INFO, "Parsed %ld items", parsed);
    nob_log(INFO, "==== REMAINING ====");
    for (size_t i = argc; i > 0; --i) {
        nob_log(INFO, "%s", shift(argv, argc));
    }
    nob_log(INFO, "===================");
    nob_log(INFO, "==== PARSED ====");
    for (size_t i = 0; i < flags.count + parsed; ++i) {
        nob_log(INFO, "%s (%s)", flags.items[i].name, *(flags.items[i].value) ? "true" : "false");
    }
    nob_log(INFO, "================");

    Cmd cmd = {0};
    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-Wall", "-Wextra");
    if (debug) {
        cmd_append(&cmd, "-ggdb");
    }
    cmd_append(&cmd, "-o", "main");
    cmd_append(&cmd, "main.c");

    return !cmd_run_sync(cmd);
}
```
*/

#ifndef NOBFLAG_H
#define NOBFLAG_H

#include "nob.h"
#include <stdbool.h>

#define nob_da_swap(da, i, j)                           \
    do {                                                \
        size_t ii = (i);                                \
        size_t jj = (j);                                \
        nob_da_append((da), (da)->items[ii]);           \
        (da)->items[ii] = (da)->items[jj];              \
        (da)->items[jj] = (da)->items[(da)->count - 1]; \
        (da)->count--;                                  \
    } while (0)

typedef struct {
    const char *name;
    bool *value;
} Nob_Cmd_Flag;

typedef struct {
    Nob_Cmd_Flag *items;
    size_t count;
    size_t capacity;
} Nob_Cmd_Flags;

#define nob_cmd_flags_add(flags, flag)   \
    do {                                 \
        Nob_Cmd_Flag f = {#flag, &flag}; \
        nob_da_append(flags, f);         \
    } while (0)

#ifdef NOB_CMD_FLAGS_CONSUME_HYPHEN
#define nob_cmd_flags_add_empty(flags, value) \
    do {                                      \
        Nob_Cmd_Flag f = {"", &value};        \
        nob_da_append(flags, f);              \
    } while (0)
#endif // NOB_CMD_FLAGS_CONSUME_HYPHEN

size_t nob_cmd_flags_parse(Nob_Cmd_Flags *flags, int *argc, char ***argv);

#ifdef NOB_IMPLEMENTATION
#include <string.h>
size_t nob_cmd_flags_parse(Nob_Cmd_Flags *flags, int *argc, char ***argv) {
    NOB_ASSERT(flags != NULL && argc != NULL && argv != NULL);
    if (*argc <= 0) {
        return 0;
    }

    size_t arg_idx = 1;
    for (; arg_idx < *argc; ++arg_idx) {
        char *arg = (*argv)[arg_idx];
        if (!arg[0]) {
            break;
        }
#ifndef NOB_CMD_FLAGS_CONSUME_HYPHEN
        if (!arg[1]) {
            break;
        }
#endif // !NOB_CMD_FLAGS_CONSUME_HYPHEN
        if (arg[0] != '-' || arg[1] == '-') {
            break;
        }
        arg++;
        for (size_t flag_idx = 0; flag_idx < flags->count; ++flag_idx) {
            Nob_Cmd_Flag flag = flags->items[flag_idx];
            if (strcmp(flag.name, arg) == 0) {
                *(flag.value) = true;
                nob_da_swap(flags, flag_idx, flags->count - 1);
                (flags->count)--;
                break;
            }
        }
    }

    (*argc) -= arg_idx;
    (*argv) += arg_idx;
    return arg_idx - 1;
}
#endif // NOB_IMPLEMENTATION

#ifdef NOB_STRIP_PREFIX
#define da_swap nob_da_swap
#define Cmd_Flag Nob_Cmd_Flag
#define Cmd_Flags Nob_Cmd_Flags
#define cmd_flags_add nob_cmd_flags_add
#ifdef NOB_CMD_FLAGS_CONSUME_HYPHEN
#define cmd_flags_add_empty nob_cmd_flags_add_empty
#endif // NOB_CMD_FLAGS_CONSUME_HYPHEN
#define cmd_flags_parse nob_cmd_flags_parse
#endif // NOB_STRIP_PREFIX
#endif // !NOBFLAG_H

/*
MIT License

Copyright (c) 2025 Aurelio Buonomo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
