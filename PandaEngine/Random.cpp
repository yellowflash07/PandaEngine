#include "Random.h"
#include <stdlib.h>

Random::Random()
{
}

Random::~Random()
{
}

float Random::GetRandomFloat(float min, float max)
{
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = max - min;
    float r = random * diff;
    return min + r;
}
