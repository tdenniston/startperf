#include <stdio.h>
#include "startperf.h"

// Simple test program which compiles itself using startperf.
int main(int argc, char **argv) {
    const char *testcmd_fmt = "gcc %s -o %s.testexe";
    size_t len = snprintf(NULL, 0, testcmd_fmt, __FILE__, argv[0]);
    char *cmd = (char *)malloc((len+1) * sizeof(char));
    snprintf(cmd, len+1, testcmd_fmt, __FILE__, argv[0]);

    startperf(cmd);

    return 0;
}
