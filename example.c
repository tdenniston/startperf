#include <math.h>
#include <stdlib.h>
#include "startperf.h"

#define N (1024*1024)

int main() {
    // Some initialization code we do not want to profile
    int *data = (int *)malloc(N * sizeof(int));
    assert(data);
    for (int i = 0; i < N; i++) {
        data[i] = (int)floor(sqrtf(i));
    }

    startperf("perf stat -e cache-misses -p %d");
    // Give perf a chance to attach.
    sleep(1);

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
