// init database +
// close database +
// get available parking spots +
// get lot data
// update lot status +

#include <sqlite3.h>
#include "utils.h"
#include "baza.h"

static sqlite3 *db;
static int dt;
static char *sql;
static char *errMsg;
static int dbRequiresSeeding = 1;

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    for (int i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    return 0;
}

static int setDbRequiresSeeding(void *_, int argc, char **argv, char **azColName)
{
    dbRequiresSeeding = 0;
    return 0;
}

static int callbackForSpots(struct AvailableSpots *result, int argc, char **argv, char **azColName)
{
    addSpot(result, atoi(argv[0]));
    return 0;
}

static int callbackForAllSpots(struct AllSpots *result, int argc, char **argv, char **azColName)
{
    struct SpotData *spot = (struct SpotData*)malloc(sizeof(struct SpotData));
    spot->number = atoi(argv[0]);
    spot->status = atoi(argv[1]);
    spot->lastUpdated = atoi(argv[2]);
    addASpot(result, spot);
    return 0;
}

static int callbackForData(struct SpotData *result, int argc, char **argv, char **azColName)
{
    result->number = atoi(argv[0]);
    result->status = atoi(argv[1]);
    result->lastUpdated = atoi(argv[2]);
    return 0;
}

static char *getSeedSql(int parkingLotsCount)
{
    char *seedSql = (char *)malloc(1000 * sizeof(char));
    strcpy(seedSql, "INSERT INTO Lots VALUES ");

    for (int i = 1; i <= parkingLotsCount; i++)
    {
        char endOfTheLineCharacter = i == parkingLotsCount ? ';' : ',';
        char *rowValueSql = (char *)malloc(1000 * sizeof(char));
        sprintf(rowValueSql, "(%d, %d, %d)%c", i, getRandomLotStatus(), getUnixTimeNow(), endOfTheLineCharacter);
        strcat(seedSql, rowValueSql);
    }

    return seedSql;
}

static void openDb()
{
    // Open db (if not exists create a new one)
    dt = sqlite3_open("parking_lots.db", &db);

    if (dt != SQLITE_OK)
    {
        fprintf(stderr, "Can't open the database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    else
    {
        fprintf(stdout, "Database connection established\n");
    }
}

static void createTable()
{
    sql = "CREATE TABLE IF NOT EXISTS Lots("
          "Number INTEGER PRIMARY KEY NOT NULL,"
          "Status INTEGER NOT NULL,"
          "Last_Updated INTEGER NOT NULL);";

    dt = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (dt != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
    }
}

static void checkIfSeedingRequired()
{
    sql = "SELECT * FROM Lots;";

    dt = sqlite3_exec(db, sql, setDbRequiresSeeding, 0, &errMsg);
    if (dt != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
    }
}

static void seed()
{
    dt = sqlite3_exec(db, getSeedSql(30), callback, 0, &errMsg);

    if (dt != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
    }
    else
    {
        printf("Seeding completed\n");
    }
}

void initDb(int seedParam)
{
    srand(time(NULL));
    openDb();
    createTable();
    if (seedParam)
    {
    checkIfSeedingRequired();
    if (dbRequiresSeeding)
    {
        seed();
    }
    else
    {
        printf("Seeding not required\n");
    }
    }
}

void closeDb()
{
    sqlite3_close(db);
}

struct AvailableSpots getAvailableParkingSpots()
{
    struct AvailableSpots result = initAvailableSpots();

    char *sql = (char *)malloc(100 * sizeof(char));
    sprintf(sql, "SELECT Number FROM Lots WHERE Status = %d;", Available);

    dt = sqlite3_exec(db, sql, callbackForSpots, &result, &errMsg);
    if (dt != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
    }

    return result;
}

void updateLotStatus(int number, int status)
{
    char *sql = (char *)malloc(100 * sizeof(char));
    sprintf(sql, "UPDATE Lots "
                 "SET Status = %d, Last_Updated = %d "
                 "WHERE Number = %d;",
            status, getUnixTimeNow(), number);

    dt = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (dt != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
    }
    else
    {
        printf("Table updated successfully\n");
    }
}

struct SpotData getSpotData(int number)
{
    struct SpotData result;
    
    char *sql = (char *)malloc(100 * sizeof(char));
    sprintf(sql, "SELECT * FROM Lots WHERE Number = %d;", number);

    dt = sqlite3_exec(db, sql, callbackForData, &result, &errMsg);
    if (dt != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
    }

    return result;
}

struct AllSpots getAllSpotsData()
{
    struct AllSpots result = initAllSpots();

    char *sql = (char *)malloc(100 * sizeof(char));
    sprintf(sql, "SELECT * FROM Lots;");

    dt = sqlite3_exec(db, sql, callbackForAllSpots, &result, &errMsg);
    if (dt != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
    }

    return result;
}