#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "header.h"

extern const char *DB_PATH;

sqlite3* open_db_system(void) {
    sqlite3 *db;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    const char *sql_create = "CREATE TABLE IF NOT EXISTS Accounts ("
                             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                             "userId INTEGER NOT NULL, "
                             "accountNbr INTEGER NOT NULL, "
                             "depositMonth INTEGER, "
                             "depositDay INTEGER, "
                             "depositYear INTEGER, "
                             "country TEXT, "
                             "phone INTEGER, "
                             "amount REAL, "
                             "accountType TEXT);";
    char *errMsg = NULL;
    int rc2 = sqlite3_exec(db, sql_create, 0, 0, &errMsg);
    if (rc2 != SQLITE_OK) {
        fprintf(stderr, "SQL error (Accounts): %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        exit(1);
    }
    return db;
}

int getUserIdFromName(const char *name) {
    sqlite3 *db = open_db_system();
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id FROM Users WHERE name = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    int userId = -1;
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            userId = sqlite3_column_int(stmt, 0);
        }
    } else {
        fprintf(stderr, "Failed to fetch user id: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return userId;
}
