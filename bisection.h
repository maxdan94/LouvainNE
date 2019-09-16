#ifndef BISECTION_H
#define BISECTION_H

#include "struct.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned (*bisection)(adjlist*,unsigned*);
bisection choosebisection(char*);

#endif
