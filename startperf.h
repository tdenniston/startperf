/*
Copyright (c) 2015 Tyler Denniston

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __STARTPERF_H__
#define __STARTPERF_H__

#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static pid_t startperf_child_pid = -1;
static char **startperf_getcmd(const char *cmdstr, pid_t parent_pid);

/*
 * Starts a child process and detaches it from the parent so that it
 * continues running after the parent terminates. Any occurrence of
 * '%d' in the command is replaced with the parent pid. The given
 * command string should be null-terminated.
 *
 * Example:
 *
 *    startperf("perf stat -e cache-misses -p %d -o perf-stat.dat");
 *
 */
static void startperf(const char *cmdstr) {
    assert(cmdstr != NULL);
    pid_t parent_pid = getpid();
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Failed to fork process.\n");
    }

    if (pid == 0) {
        // Preprocess command.
        char **cmd = startperf_getcmd(cmdstr, parent_pid);
        assert(cmd != NULL);
        // Detach from parent process.
        pid_t sid = setsid();
        if (sid < 0) {
            fprintf(stderr, "Failed to set session id.\n");
            exit(1);
        }
        // Exec command.
        int retval = execvp(cmd[0], cmd);
        perror("Failed to exec");
        exit(1);
    } else {
        startperf_child_pid = pid;
    }
}

/*
 * Sends SIGINT to the child process and waits for it to terminate.
 */
static void stopperf() {
    if (startperf_child_pid == -1) return;
    int retval = kill(startperf_child_pid, SIGINT);
    if (retval == 0) {
        pid_t p = waitpid(startperf_child_pid, NULL, 0);
        if (p == -1) {
            perror("waitpid");
        }
    } else {
        perror("Failed to send SIGTERM");
    }
}

/*
 * Helper function for startperf. Replaces all occurrences of '%d'
 * with the parent process pid, and then splits the string on spaces,
 * returning an array of strings. The last element of the resulting
 * array is always NULL.
 */
static char **startperf_getcmd(const char *cmdstr, pid_t parent_pid) {
    assert(cmdstr);
    char **cmd = NULL, *copy = NULL, *tok = NULL;
    char c = '\0';
    int nelts = 1, i = 0;
    const char *delim = " ";
    const char *p = cmdstr;

    while (c = *p++) {
        if (c == ' ') nelts++;
    }
    cmd = (char **)calloc(nelts+1, sizeof(char *));
    assert(cmd != NULL);
    cmd[nelts] = (char *)NULL;

    copy = (char *)calloc(strlen(cmdstr) + 1, sizeof(char));
    assert(copy != NULL);
    strcpy(copy, cmdstr);

    tok = strtok(copy, delim);
    for (i = 0; tok != NULL; i++) {
        if (strncmp(tok, "%d", 2) == 0) {
            char pid_s[16];
            snprintf(pid_s, sizeof(pid_s), "%d", parent_pid);
            cmd[i] = (char *)calloc(strlen(pid_s) + 1, sizeof(char));
            assert(cmd[i] != NULL);
            strcpy(cmd[i], pid_s);
        } else {
            cmd[i] = (char *)calloc(strlen(tok) + 1, sizeof(char));
            assert(cmd[i] != NULL);
            strcpy(cmd[i], tok);
        }
        tok = strtok(NULL, delim);
    }
    free(copy);
    return cmd;
}

#endif
