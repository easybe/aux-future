/*
 * getopt() and getopt_long() as in glibc. Options and non-option
 * arguments may be mixed. The options are moved to the front, unless
 * the option string starts with "+" or POSIXLY_CORRECT is set.
 */
#ifndef FUTURE_GETOPT_H
#define FUTURE_GETOPT_H

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

#define no_argument        0
#define required_argument  1
#define optional_argument  2

extern char *optarg;
extern int optind, opterr, optopt;

int getopt(int argc, char * const argv[], const char *shortopts);
int getopt_long(int argc, char * const argv[], const char *shortopts,
                const struct option *longopts, int *longindex);

#endif
