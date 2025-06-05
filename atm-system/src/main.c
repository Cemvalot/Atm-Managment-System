#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * initMenu - Initializes the system by prompting the user to log in or register.
 *
 * Updated to:
 *   1) Check if username exists. If not, show error and let the user retry.
 *   2) Allow up to 3 attempts for password if the username is valid.
 *   3) If all 3 fail, block and exit (or do something else).
 */
void initMenu(struct User *u)
{
    int option;
    int loggedIn = 0;

    system("clear");
    printf("\n\n\t\t======= ATM =======\n");
    printf("\n\t\t-->> Login / Register :\n");
    printf("\n\t\t[1]- Login\n");
    printf("\n\t\t[2]- Register\n");
    printf("\n\t\t[3]- Exit\n");

    while (!loggedIn) {
        printf("\nChoose an option: ");
        if (scanf("%d", &option) != 1) {
            printf("Invalid input. Please enter a valid number.\n");
            while(getchar() != '\n'); // Clear input buffer
            continue;
        }
        while(getchar() != '\n'); // Clear leftover input for safety

        switch (option) {
            case 1: {
                // Prompt for username
                system("clear");
                printf("=== LOGIN ===\n");
                printf("Enter Username: ");
                if (scanf("%49s", u->name) != 1) {
                    printf("Invalid username input.\n");
                    while(getchar() != '\n');
                    break; // Return to menu
                }
                while(getchar() != '\n'); // Flush leftover input

                // Check if user exists
                if (!userExists(u->name)) {
                    printf("\nUsername '%s' doesn't exist.\n", u->name);
                    printf("Please register or contact the bank if you believe this is an error.\n\n");
                    break; // Return to main menu loop
                }

                // If user exists, allow up to 3 attempts for the correct password
                int attempts = 0;
                int authenticated = 0;
                while (attempts < 3) {
                    loginMenu(u->password);  // Asks for password with echo off
                    if (verifyCredentials(u)) {
                        authenticated = 1;
                        break;
                    } else {
                        attempts++;
                        if (attempts < 3) {
                            printf("Attempts left: %d\n", 3 - attempts);
                        }
                    }
                }
                if (!authenticated) {
                    // All 3 attempts failed
                    printf("\nToo many failed attempts.\n");
                    printf("Please contact your bank. Your account is temporarily blocked.\n");
                    exit(1);  // or you could break to show the menu again, your choice
                }

                // If authenticated
                loggedIn = 1;
                break;
            }
            case 2:
                registerUserAuth(u);
                // If registration + immediate login was successful, we’re logged in
                loggedIn = 1;
                break;
            case 3:
                exit(0);
                break;
            default:
                printf("Insert a valid operation!\n");
        }
    }
}

/**
 * main - The entry point of the ATM program.
 *
 * It initializes the system (login/registration) and then calls the main menu
 * for further account operations.
 */
int main()
{
    struct User u;
    initMenu(&u);
    mainMenu(u);  // Calls mainMenu from your system module(s)
    return 0;
}
