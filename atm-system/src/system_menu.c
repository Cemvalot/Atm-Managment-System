#include <stdio.h>
#include <stdlib.h>
#include "header.h"

void mainMenu(struct User u) {
    system("clear");
    printf("\n\n\t\t======= ATM =======\n\n");
    printf("\n\t\t-->> Choose an option <<--\n");
    printf("\n\t\t[1]- Create a new account\n");
    printf("\n\t\t[2]- Update account information\n");
    printf("\n\t\t[3]- Check details of an account\n");
    printf("\n\t\t[4]- Check list of owned accounts\n");
    printf("\n\t\t[5]- Make Transaction\n");
    printf("\n\t\t[6]- Remove existing account\n");
    printf("\n\t\t[7]- Transfer ownership\n");
    printf("\n\t\t[8]- Exit\n");
    printf("\n\t\t[9]- Log Out\n\n");

    int option = readInt("Enter your choice: ");
    switch (option) {
        case 1: createNewAcc(u); break;
        case 2: updateAccount(u); break;
        case 3: checkAccountDetails(u); break;
        case 4: checkAllAccounts(u); break;
        case 5: makeTransaction(u); break;
        case 6: removeAccount(u); break;
        case 7: transferOwner(u); break;
        case 8: exit(0); break;
        case 9: {
                    struct User newUser;
                    initMenu(&newUser);
                    mainMenu(newUser);
                    break;
                }
        default:
            printf("Invalid operation! Please try again.\n");
            mainMenu(u);
    }
}

void success(struct User u) {
    int option = readInt("\n✔ Success!\nEnter 1 to go to the main menu, 2 to log out, or 0 to exit: ");
    if (option == 1) {
        mainMenu(u);
    } else if (option == 2) {
        struct User newUser;
        initMenu(&newUser);
        mainMenu(newUser);
    } else if (option == 0) {
        exit(0);
    } else {
        printf("Insert a valid operation!\n");
        success(u);
    }
}
