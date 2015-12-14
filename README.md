# startperf
Utility to programmatically start a command detached from its parent process. `startperf` is packaged into a simple header file that can be included directly with no other dependencies (except the C standard library).

`startperf` allows you to programmatically start a daemonized `perf` that will attach itself to the invoking process. `startperf` can be used to start an arbitrary daemonized program, but the original motivating use was for performance analysis, thus the name.

## Example

    #include "startperf.h"
    #define N (1024*1024)
    
    int main() {
        // Some initialization code we do not want to profile
        int *data = (int *)malloc(N * sizeof(int));
        assert(data);
        for (int i = 0; i < N; i++) {
            data[i] = (int)floor(sqrtf(i));
        }
    
        // %d is replaced with the parent process pid.
        startperf("perf stat -e cache-misses -p %d");
        unsigned total = 0;
        for (int iter = 0; iter < 500; iter++) {
            int result = 0;
            for (unsigned i = 0; i < N; i++) {
                result += data[i];
            }
            total += (unsigned)result;
        }
        stopperf();
    
        printf("Result: %d\n", total);
        free(data);
        return 0;
    }
