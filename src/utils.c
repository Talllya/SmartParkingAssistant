#include "utils.h"

int getRandomLotStatus()
{
    return rand() % 2;
}

int getUnixTimeNow()
{
    return (int)time(NULL);
}

int getRandomPercent() {
    return rand() % 101;
}