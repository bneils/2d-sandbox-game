#ifndef TESTING_H
#define TESTING_H

#include <stdio.h>
#include <stdlib.h>

#define assert(cond) do { if (!(cond)) { puts("failed"); exit(1); }} while (0);

#endif // TESTING_H
