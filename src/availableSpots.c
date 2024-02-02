#include "availableSpots.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int DEFAULT_AVAILABLE_SPOTS_CAPACITY = 10;

struct AvailableSpots initAvailableSpots()
{
    struct AvailableSpots as;
    as.capacity = DEFAULT_AVAILABLE_SPOTS_CAPACITY;
    as.length = 0;
    as.spots = malloc(as.capacity * sizeof(int));
    return as;
}

void addSpot(struct AvailableSpots *spots, int spot)
{
    if (spots->capacity == spots->length)
    {
        struct AvailableSpots *tmp = realloc(spots->spots, (spots->capacity + DEFAULT_AVAILABLE_SPOTS_CAPACITY) * sizeof(int));
        if (tmp == NULL)
        {
            // handle failed realloc, struct is unchanged
        }
        else
        {
            // everything went ok, update the original pointer (struct)
            spots->spots = tmp;
            spots->capacity = spots->capacity + DEFAULT_AVAILABLE_SPOTS_CAPACITY;
        }
    }
    spots->spots[spots->length] = spot;
    spots->length++;
}

struct AllSpots initAllSpots()
{
    struct AllSpots as;
    as.capacity = DEFAULT_AVAILABLE_SPOTS_CAPACITY;
    as.length = 0;
    as.spots = malloc(as.capacity * sizeof(struct SpotData));
    return as;
}

void addASpot(struct AllSpots* spots, struct SpotData* spot)
{
    if (spots->capacity == spots->length)
    {
        struct AllSpots *tmp = realloc(spots->spots, (spots->capacity + DEFAULT_AVAILABLE_SPOTS_CAPACITY) * sizeof(struct SpotData));
        if (tmp == NULL)
        {
            // handle failed realloc, struct is unchanged
        }
        else
        {
            // everything went ok, update the original pointer (struct)
            spots->spots = tmp;
            spots->capacity = spots->capacity + DEFAULT_AVAILABLE_SPOTS_CAPACITY;
        }
    }
    spots->spots[spots->length] = *spot;
    spots->length++;
}

void printSpots(struct AvailableSpots spots)
{
    for (int i = 0; i < spots.length; i++)
    {
        printf("%d ", spots.spots[i]);
    }
    printf("\n");
}

char* theStringWithTheSpots(struct AvailableSpots spots)
{
    if (spots.length == 0)
        return "";

    char *result = malloc(200 * sizeof(char));
    result[0]= '\0';
    char buf[10] = "";

    for (int i = 0; i < spots.length; i++)
    {
        sprintf(buf, "%d ", spots.spots[i]);
        strncat(result, buf, strlen(buf));
    }

    return result;
}