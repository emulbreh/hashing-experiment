#include <time.h>

void get_monotonic_time(struct timespec *ts);
double get_elapsed_time(struct timespec *before, struct timespec *after);
