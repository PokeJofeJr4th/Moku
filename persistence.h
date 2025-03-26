#ifndef PERSISTENCE_H_
#define PERSISTENCE_H_

#include "food.h"

struct Pantry pantry_read(FILE *file);
int food_write(union Food *this, FILE *file);

#endif