#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"

/*
 * Defining these here keeps the libc getopt.o, which defines them as
 * well, out of the link.
 */
char *optarg;
int optind = 1, opterr = 1, optopt;

static char *nextchar;

static const struct option *
find_long(const char *name, size_t len, const struct option *longopts,
          int *index, int *ambig)
{
    int i, match = -1;

    *ambig = 0;
    for (i = 0; longopts && longopts[i].name; i++) {
        if (strncmp(longopts[i].name, name, len) != 0)
            continue;
        if (strlen(longopts[i].name) == len) {
            *index = i;
            *ambig = 0;
            return &longopts[i];
        }
        if (match < 0)
            match = i;
        else
            *ambig = 1;
    }
    if (match < 0 || *ambig)
        return NULL;
    *index = match;
    return &longopts[match];
}

/*
 * Returns 1 if the option in arg takes the next argument as its value.
 */
static int
takes_arg(const char *arg, const char *shortopts,
          const struct option *longopts)
{
    const struct option *o;
    const char *p;
    int index, ambig;

    if (arg[1] == '-') {
        if (strchr(arg, '='))
            return 0;
        o = find_long(arg + 2, strlen(arg + 2), longopts, &index, &ambig);
        return o && o->has_arg == required_argument;
    }

    for (p = arg + 1; *p; p++) {
        const char *s = strchr(shortopts, *p);
        if (*p == ':' || !s || s[1] != ':')
            continue;
        return s[2] != ':' && p[1] == '\0';
    }
    return 0;
}

/*
 * Moves all options and their arguments in front of the non-option
 * arguments. Everything after "--" counts as a non-option argument.
 */
static void
permute(int argc, char **argv, const char *shortopts,
        const struct option *longopts)
{
    char **opts, **rest;
    int i, nopts = 0, nrest = 0;

    opts = malloc(argc * sizeof(char *));
    rest = malloc(argc * sizeof(char *));
    if (!opts || !rest) {
        free(opts);
        free(rest);
        return;
    }

    for (i = optind; i < argc; i++) {
        char *a = argv[i];
        if (strcmp(a, "--") == 0) {
            opts[nopts++] = a;
            while (++i < argc)
                rest[nrest++] = argv[i];
        } else if (a[0] == '-' && a[1] != '\0') {
            opts[nopts++] = a;
            if (takes_arg(a, shortopts, longopts) && i + 1 < argc)
                opts[nopts++] = argv[++i];
        } else {
            rest[nrest++] = a;
        }
    }

    memcpy(argv + optind, opts, nopts * sizeof(char *));
    memcpy(argv + optind + nopts, rest, nrest * sizeof(char *));
    free(opts);
    free(rest);
}

static int
long_option(int argc, char **argv, const char *shortopts,
            const struct option *longopts, int *longindex)
{
    char *name = argv[optind] + 2;
    char *eq = strchr(name, '=');
    size_t len = eq ? (size_t)(eq - name) : strlen(name);
    const struct option *o;
    int index, ambig;
    int missing = shortopts[0] == ':' ? ':' : '?';

    o = find_long(name, len, longopts, &index, &ambig);
    optind++;
    optarg = NULL;
    if (!o) {
        if (opterr)
            fprintf(stderr, "%s: %s option '%s'\n", argv[0],
                    ambig ? "ambiguous" : "unrecognized", argv[optind - 1]);
        optopt = 0;
        return '?';
    }

    if (eq) {
        if (o->has_arg == no_argument) {
            if (opterr)
                fprintf(stderr, "%s: option '--%s' doesn't allow an "
                        "argument\n", argv[0], o->name);
            optopt = o->val;
            return '?';
        }
        optarg = eq + 1;
    } else if (o->has_arg == required_argument) {
        if (optind >= argc) {
            if (opterr)
                fprintf(stderr, "%s: option '--%s' requires an "
                        "argument\n", argv[0], o->name);
            optopt = o->val;
            return missing;
        }
        optarg = argv[optind++];
    }

    if (longindex)
        *longindex = index;
    if (o->flag) {
        *o->flag = o->val;
        return 0;
    }
    return o->val;
}

int
getopt_long(int argc, char * const argv[], const char *shortopts,
            const struct option *longopts, int *longindex)
{
    static int permuted;
    char **av = (char **)argv;
    const char *s;
    int c, missing;

    if (optind == 0) {
        optind = 1;
        nextchar = NULL;
        permuted = 0;
    }
    if (*shortopts == '+') {
        shortopts++;
    } else if (!permuted && !nextchar && !getenv("POSIXLY_CORRECT")) {
        permute(argc, av, shortopts, longopts);
        permuted = 1;
    }
    missing = shortopts[0] == ':' ? ':' : '?';

    if (!nextchar || *nextchar == '\0') {
        nextchar = NULL;
        if (optind >= argc)
            return -1;
        if (av[optind][0] != '-' || av[optind][1] == '\0')
            return -1;
        if (strcmp(av[optind], "--") == 0) {
            optind++;
            return -1;
        }
        if (av[optind][1] == '-' && longopts)
            return long_option(argc, av, shortopts, longopts, longindex);
        nextchar = av[optind] + 1;
    }

    c = (unsigned char)*nextchar++;
    s = strchr(shortopts, c);
    optarg = NULL;

    if (c == ':' || !s) {
        if (opterr && shortopts[0] != ':')
            fprintf(stderr, "%s: invalid option -- '%c'\n", av[0], c);
        optopt = c;
        if (*nextchar == '\0') {
            optind++;
            nextchar = NULL;
        }
        return '?';
    }

    if (s[1] != ':') {
        if (*nextchar == '\0') {
            optind++;
            nextchar = NULL;
        }
        return c;
    }

    if (*nextchar != '\0') {
        optarg = nextchar;
        optind++;
    } else if (s[2] == ':') {
        optind++;
    } else if (optind + 1 < argc) {
        optarg = av[optind + 1];
        optind += 2;
    } else {
        if (opterr && shortopts[0] != ':')
            fprintf(stderr, "%s: option requires an argument -- '%c'\n",
                    av[0], c);
        optopt = c;
        optind++;
        nextchar = NULL;
        return missing;
    }
    nextchar = NULL;
    return c;
}

int
getopt(int argc, char * const argv[], const char *shortopts)
{
    return getopt_long(argc, argv, shortopts, NULL, NULL);
}
