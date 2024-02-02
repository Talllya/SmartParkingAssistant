#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "baza.h"

static int checkCycle = 60;
static int minPercent = 5;
static int maxPercent = 95;
static int minBusySeconds = 120;
static int minBronedToGetAvailableSeconds = 40;

int adjustChanceToBoundary(int chance)
{
    if (chance < minPercent)
        return minPercent;
    if (chance > maxPercent)
        return maxPercent;
    return chance;
}

int camDetect(struct SpotData spot)
{
    if (spot.status == Available)
        return spot.status;

    int passed = getUnixTimeNow() - spot.lastUpdated;
    if (spot.status == Broned)
    {
        int requiredChance = adjustChanceToBoundary(passed * minPercent / minBronedToGetAvailableSeconds);
        int chance = getRandomPercent();
        if (chance <= requiredChance)
        {
            return Available;
        }
        return Taken;
    }
    if (spot.status == Taken)
    {
        int requiredChance = adjustChanceToBoundary(passed * minPercent / minBusySeconds);
        int chance = getRandomPercent();
        if (chance <= requiredChance)
        {
            return Available;
        }
    }

    return spot.status;
}

int main()
{
    int seedParamOn = 1;

    initDb(seedParamOn);
    printf("\nTurning on cameras\n");
    while (1)
    {
        sleep(checkCycle);

        struct AllSpots spots = getAllSpotsData();
        for (int i = 0; i < spots.length; i++)
        {
            int updatedStatus = camDetect(spots.spots[i]);
            if (updatedStatus != spots.spots[i].status)
            {
                updateLotStatus(spots.spots[i].number, updatedStatus);
                printf("Updated spot %d: from %d to %d\n", spots.spots[i].number, spots.spots[i].status, updatedStatus);
            }
        }
    }
    printf("\nTurning off cameras\n");
    closeDb();
}
