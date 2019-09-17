#ifndef PARTITION_H
#define PARTITION_H

#include "struct.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned (*partition)(adjlist*,unsigned*);
partition choose_partition(char*);

#endif
