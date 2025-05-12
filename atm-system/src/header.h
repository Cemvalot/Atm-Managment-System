#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

struct Date
{
    int month, day, year;
};

// all fields for each record of an account
struct Record
{
    int id;
    int userId;
    char name[100];
    char country[100];
    int phone;
    char accountType[10];
    int accountNbr;
    double amount;
    struct Date deposit;
    struct Date withdraw;
};

struct User
{
    int id;
    char name[50];
    char password[50];
};

// authentication functions
void loginMenu(char a[50], char pass[50]);
void registerMenu(char a[50], char pass[50]);
const char *getPassword(struct User u);

// system function
void createNewAcc(struct User u);
void mainMenu(struct User u);
void checkAllAccounts(struct User u);
void updateAccount(struct User u);
void checkAccount(struct User u);
void makeTransaction(struct User u);
void removeAccount(struct User u);
void transferOwnership(struct User u);

// utility functions
int getAccountFromFile(FILE *ptr, char name[50], struct Record *r);
void saveAccountToFile(FILE *ptr, struct User u, struct Record r);
void stayOrReturn(int notGood, void f(struct User u), struct User u);
void success(struct User u);