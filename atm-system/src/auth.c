#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "header.h"
#include <unistd.h>

const char *DB_PATH = "./src/data/atm.db";

sqlite3* open_db() {
    sqlite3 *db;
    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }
    
    const char *sql_create = 
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT UNIQUE NOT NULL, "
        "password TEXT NOT NULL"
        ");";

    char *errMsg = NULL;
    rc = sqlite3_exec(db, sql_create, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        exit(1);
    }
    return db;
}

int userExists(const char *username)
{
    sqlite3 *db = open_db();
    const char *sql = "SELECT 1 FROM Users WHERE name = ? LIMIT 1;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement in userExists: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    int exists = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = 1; // Found at least one row => user exists
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return exists;
}

void loginMenu(char pass[50])
{
    struct termios oflags, nflags;

    // Hide password input in the terminal
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;  // Disable echo
    nflags.c_lflag |= ECHONL; // Still print a newline after user hits Enter
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        exit(1);
    }

    printf("Enter your password: ");
    if (scanf("%49s", pass) != 1) {
        printf("Password input error.\n");
        pass[0] = '\0'; // Clear password on error
    }

    // Restore original terminal settings
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        exit(1);
    }
    // Flush leftover input (just in case)
    while(getchar() != '\n');
}

const char *getPassword(struct User u)
{
    sqlite3 *db = open_db();
    const char *sql = "SELECT password FROM Users WHERE name = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_STATIC);

    const char *result = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *passText = sqlite3_column_text(stmt, 0);
        result = strdup((const char*) passText);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}

int verifyCredentials(struct User *u) {
    const char *stored = getPassword(*u);
    if (stored == NULL) {
        // No such user or no password found
        // But we now check user existence before calling this.
        printf("User not found in verifyCredentials.\n");
        return 0;
    }
    char *enteredHash = crypt(u->password, stored);
    int valid = (strcmp(enteredHash, stored) == 0);
    if (valid) {
        printf("\nPassword Match!\n");
    } else {
        printf("\nWrong password.\n");
    }
    free((void *)stored);
    return valid;
}

void registerUserAuth(struct User *u)
{
    char name[50];
    char password[50];
    int usernameExistsFlag = 0;
    sqlite3 *db = open_db();
    sqlite3_stmt *stmt;
    int rc;

    /* Loop until a unique username is provided */
    do {
        usernameExistsFlag = 0;
        printf("Enter username to register: ");
        if (scanf("%49s", name) != 1) {
            printf("Invalid username input.\n");
            // Flush leftover input
            while(getchar() != '\n');
            continue;
        }
        // Flush leftover input
        while(getchar() != '\n');

        const char *sql_check = "SELECT id FROM Users WHERE name = ?;";
        rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            exit(1);
        }
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("The username '%s' already exists. Please choose a different username.\n\n", name);
            usernameExistsFlag = 1;
        }
        sqlite3_finalize(stmt);
    } while (usernameExistsFlag);

    printf("Enter password: ");
    if (scanf("%49s", password) != 1) {
        printf("Invalid password input.\n");
        // Flush leftover input
        while(getchar() != '\n');
    }
    while(getchar() != '\n'); // flush leftover input

    /* Hash the password using a fixed salt (for demonstration purposes) */
    char *hashed = crypt(password, "$1$ab$");

    const char *sql_insert = "INSERT INTO Users (name, password) VALUES (?, ?);";
    rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashed, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Insert failed: %s\n", sqlite3_errmsg(db));
    } else {
        printf("User '%s' registered successfully.\n", name);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    /* Store the credentials in the provided User structure */
    strncpy(u->name, name, sizeof(u->name));
    strncpy(u->password, password, sizeof(u->password));

    /* Prompt the new user with choices */
    int choice;
    printf("\nPlease choose:\n");
    printf("1 - Proceed to log in\n");
    printf("2 - Log off (Return to first screen)\n");
    printf("3 - Exit\n");
    printf("Enter your choice: ");
    if (scanf("%d", &choice) != 1) {
         printf("Invalid input. Defaulting to log off.\n");
         choice = 2;
    }
    while(getchar() != '\n'); // Clear input buffer

    switch(choice) {
         case 1:
              printf("\nPlease log in with your new credentials.\n");
              // We already have the username and password stored in *u, but let's
              // prompt again so user sees the new flow:
              loginMenu(u->password);
              if (verifyCredentials(u)) {
                  return;
              } else {
                  exit(1);
              }
              break;
         case 2:
              /* Return to the first screen */
              initMenu(u);
              break;
         case 3:
              printf("Thank you for choosing our bank for your transactions. See you soon!\n");
              exit(0);
              break;
         default:
              printf("Invalid option. Returning to first screen.\n");
              initMenu(u);
    }
}