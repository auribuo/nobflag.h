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
