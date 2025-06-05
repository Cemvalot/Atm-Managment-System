#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

/* External declaration for the database path */
extern const char *DB_PATH;

/* --- Data Structures --- */
struct Date {
    int month;
    int day;
    int year;
};

struct Record {
    int id;
    int userId;
    char name[100];
    char country[100];
    long long phone;
    int accountNbr;
    double amount;
    struct Date deposit;
    char accountType[10];
};

struct User {
    int id;
    char name[50];
    char password[50];
};

/* --- Function Prototypes --- */

/* System Input Functions (in system_input.c) */
void clearInputBuffer(void);
int readInt(const char *prompt);
long long readLongLong(const char *prompt);
double readDouble(const char *prompt);

/* System Database Functions (in system_db.c) */
sqlite3* open_db_system(void);
int getUserIdFromName(const char *name);

/* ------------------------ */
/*     AUTHENTICATION       */
/* ------------------------ */

/* 1) New prototype for userExists */
int userExists(const char *username);

/* 2) Updated prototype for loginMenu (only password parameter now) */
void loginMenu(char pass[50]);

/* Other authentication-related functions */
void registerUserAuth(struct User *u);
const char* getPassword(struct User u);
int verifyCredentials(struct User *u);

/* ------------------------ */
/*  OTHER SYSTEM FUNCTIONS  */
/* ------------------------ */

void handleUnexpectedError(sqlite3 *db);
void createNewAcc(struct User u);
void mainMenu(struct User u);
void initMenu(struct User *u);
void checkAllAccounts(struct User u);
void updateAccount(struct User u);
void checkAccountDetails(struct User u);
void makeTransaction(struct User u);
void removeAccount(struct User u);
void transferOwner(struct User u);
void success(struct User u);

#endif // HEADER_H
