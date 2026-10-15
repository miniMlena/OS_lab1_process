#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

static int is_prime(long n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(void) {
    int value;

    while (scanf("%d", &value) == 1) {
        if (value < 0) {
            return 0;
        }

        if (is_prime((long)value)) {
            return 0;
        }

        printf("%d\n", value);

        fflush(stdout);
    }

    return 0;
}