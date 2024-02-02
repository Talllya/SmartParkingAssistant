#include <stdlib.h>
#include <stdio.h>

struct AvailableSpots
{
    int* spots;
    int length;
    int capacity;
};

struct SpotData
{
    int number;
    int status;
    int lastUpdated;
};

struct AllSpots
{
    struct SpotData* spots;
    int length;
    int capacity;
};

struct AvailableSpots initAvailableSpots();
void addSpot(struct AvailableSpots* spots, int spot);
void printSpots(struct AvailableSpots spots);
char* theStringWithTheSpots(struct AvailableSpots spots);

struct AllSpots initAllSpots();
void addASpot(struct AllSpots* spots, struct SpotData* spot);