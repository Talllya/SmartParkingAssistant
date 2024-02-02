#include <stdio.h>
#include <string.h>
#include "availableSpots.h"

enum spotStatus
{
    Available = 0,
    Taken = 1,
    Broned = 2
};

void initDb(int seedParam);   
void closeDb();
struct AvailableSpots getAvailableParkingSpots();
void updateLotStatus(int number, int status);
struct SpotData getSpotData(int number);
struct AllSpots getAllSpotsData();
