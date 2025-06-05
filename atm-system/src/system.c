#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>      // <-- for date/time functions if you want partial maturity checks
#include <sqlite3.h>
#include "header.h"

/**
 * handleUnexpectedError - General fallback for DB errors or unexpected failures.
 * Closes the DB connection (if open) and displays an error message, then exits.
 */
void handleUnexpectedError(sqlite3 *db) {
    if (db != NULL) {
        sqlite3_close(db);
    }
    printf("\n⚠️  Unexpected error occurred. Please contact your bank directly.\n");
    exit(EXIT_FAILURE);
}

/**
 * chooseAccountType - Prompts user to choose and confirm the account type.
 * 'fixed01', 'fixed02', 'fixed03' are special accounts with restricted transactions.
 */
void chooseAccountType(char *accountType, size_t size) {
    int choice;
    char confirm;
    while (1) {
        printf("\nChoose the type of account:\n");
        printf("1 - Saving\n   (Monthly interest; approximate annual rate 7%%, prorated monthly)\n");
        printf("2 - Current\n   (No interest applied)\n");
        printf("3 - Fixed01\n   (1-year fixed deposit with 4%% interest)\n");
        printf("4 - Fixed02\n   (2-year fixed deposit with 5%% interest)\n");
        printf("5 - Fixed03\n   (3-year fixed deposit with 8%% interest)\n");
        choice = readInt("Enter your choice (1-5): ");
        
        switch(choice) {
            case 1:
                strncpy(accountType, "saving", size);
                printf("\nYou have chosen Saving account. It earns monthly interest (7%% per annum, prorated monthly).\n");
                break;
            case 2:
                strncpy(accountType, "current", size);
                printf("\nYou have chosen Current account. It does not earn any interest.\n");
                break;
            case 3:
                strncpy(accountType, "fixed01", size);
                printf("\nYou have chosen Fixed01 account. It is a 1-year fixed deposit with 4%% interest.\n");
                printf("WARNING: Fixed accounts do not allow deposits or withdrawals until the maturity date (1 year from the deposit date).\n");
                break;
            case 4:
                strncpy(accountType, "fixed02", size);
                printf("\nYou have chosen Fixed02 account. It is a 2-year fixed deposit with 5%% interest.\n");
                printf("WARNING: Fixed accounts do not allow deposits or withdrawals until the maturity date (2 years from the deposit date).\n");
                break;
            case 5:
                strncpy(accountType, "fixed03", size);
                printf("\nYou have chosen Fixed03 account. It is a 3-year fixed deposit with 8%% interest.\n");
                printf("WARNING: Fixed accounts do not allow deposits or withdrawals until the maturity date (3 years from the deposit date).\n");
                break;
            default:
                printf("Invalid option. Please choose a number between 1 and 5.\n");
                continue;
        }
        
        printf("Confirm this account type? (Y/N): ");
        scanf(" %c", &confirm);
        clearInputBuffer();
        if (confirm == 'Y' || confirm == 'y') {
            break;
        } else {
            printf("Let's choose again.\n");
        }
    }
}

/**
 * createNewAcc - Creates a new account for the given user.
 * Checks for duplicates, sets deposit date, phone, country, etc.
 */
void createNewAcc(struct User u) {
    sqlite3 *db = open_db_system();
    if (!db) handleUnexpectedError(db);

    int userId = getUserIdFromName(u.name);
    if (userId == -1) {
        printf("User not found in database. Cannot create account.\n");
        sqlite3_close(db);
        return;
    }
    
    int month, day, year, accountNbr;
    char country[100], accountType[20];
    long long phone;
    double amount;
    char inputBuffer[100];
    
    while (1) {
        system("clear");
        printf("\t\t\t===== New Account Creation =====\n");
        
        printf("\nEnter today's date (mm/dd/yyyy): ");
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL ||
            sscanf(inputBuffer, "%d/%d/%d", &month, &day, &year) != 3) {
            printf("Invalid date format. Please use mm/dd/yyyy.\n");
            continue;
        }
        
        accountNbr = readInt("\nEnter the account number: ");
        
        sqlite3_stmt *stmt;
        const char *sql_check = "SELECT id FROM Accounts WHERE userId = ? AND accountNbr = ?;";
        int rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, 0);
        if (rc != SQLITE_OK) handleUnexpectedError(db);

        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_int(stmt, 2, accountNbr);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            printf("✖ This account already exists for this user.\n\n");
            sqlite3_finalize(stmt);
            continue;
        }
        sqlite3_finalize(stmt);
        
        printf("\nEnter the country: ");
        if (fgets(country, sizeof(country), stdin) == NULL) {
            printf("Error reading country.\n");
            continue;
        }
        country[strcspn(country, "\n")] = '\0';
        
        phone = readLongLong("\nEnter the phone number: ");
        amount = readDouble("\nEnter amount to deposit: $");
        chooseAccountType(accountType, sizeof(accountType));
        
        const char *sql_insert = "INSERT INTO Accounts (userId, accountNbr, depositMonth, depositDay, depositYear, country, phone, amount, accountType) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
        rc = sqlite3_prepare_v2(db, sql_insert, -1, &stmt, 0);
        if (rc != SQLITE_OK) handleUnexpectedError(db);

        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_int(stmt, 2, accountNbr);
        sqlite3_bind_int(stmt, 3, month);
        sqlite3_bind_int(stmt, 4, day);
        sqlite3_bind_int(stmt, 5, year);
        sqlite3_bind_text(stmt, 6, country, -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 7, phone);
        sqlite3_bind_double(stmt, 8, amount);
        sqlite3_bind_text(stmt, 9, accountType, -1, SQLITE_STATIC);
        
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "Could not insert record: %s\n", sqlite3_errmsg(db));
            // Optionally: handleUnexpectedError(db); if you want a full exit
        } else {
            printf("New account created successfully.\n");
        }
        sqlite3_finalize(stmt);
        break;
    }
    sqlite3_close(db);
    success(u);
}

/**
 * checkAllAccounts - Lists every account owned by the current user.
 */
void checkAllAccounts(struct User u) {
    sqlite3 *db = open_db_system();
    if (!db) handleUnexpectedError(db);

    int userId = getUserIdFromName(u.name);
    if (userId == -1) {
        printf("User not found.\n");
        sqlite3_close(db);
        return;
    }
    system("clear");
    printf("\t\t====== All accounts for user %s =====\n\n", u.name);
    
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, accountNbr, depositMonth, depositDay, depositYear, country, phone, amount, accountType FROM Accounts WHERE userId = ? ORDER BY id;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int accNbr = sqlite3_column_int(stmt, 1);
        int depositMonth = sqlite3_column_int(stmt, 2);
        int depositDay = sqlite3_column_int(stmt, 3);
        int depositYear = sqlite3_column_int(stmt, 4);
        const unsigned char *country = sqlite3_column_text(stmt, 5);
        long long phone = sqlite3_column_int64(stmt, 6);
        double amount = sqlite3_column_double(stmt, 7);
        const unsigned char *accountType = sqlite3_column_text(stmt, 8);
        
        printf("_____________________\n");
        printf("\nAccount Record ID: %d\n", id);
        printf("Account Number: %d\n", accNbr);
        printf("Deposit Date: %d/%d/%d\n", depositMonth, depositDay, depositYear);
        printf("Country: %s\n", country);
        printf("Phone: %lld\n", phone);
        printf("Amount: $%.2lf\n", amount);
        printf("Account Type: %s\n", accountType);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

/**
 * updateAccount - Updates the country or phone field of a specific account.
 */
void updateAccount(struct User u) {
    int accNbr = readInt("Enter the account number to update: ");
    
    sqlite3 *db = open_db_system();
    if (!db) handleUnexpectedError(db);

    int userId = getUserIdFromName(u.name);
    if (userId == -1) handleUnexpectedError(db);

    sqlite3_stmt *stmt;
    const char *sql_check = "SELECT id FROM Accounts WHERE accountNbr = ? AND userId = ?;";
    int rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, accNbr);
    sqlite3_bind_int(stmt, 2, userId);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Account not found or does not belong to you.\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    sqlite3_finalize(stmt);
    
    int option = readInt("Which field do you want to update? [1] Country, [2] Phone number: ");
    if (option == 1) {
        char newCountry[100];
        printf("Enter new country: ");
        if (fgets(newCountry, sizeof(newCountry), stdin) == NULL) {
            printf("Error reading input.\n");
            sqlite3_close(db);
            return;
        }
        newCountry[strcspn(newCountry, "\n")] = '\0';
        const char *sql_update = "UPDATE Accounts SET country = ? WHERE accountNbr = ? AND userId = ?;";
        rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
        if (rc != SQLITE_OK) handleUnexpectedError(db);

        sqlite3_bind_text(stmt, 1, newCountry, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, accNbr);
        sqlite3_bind_int(stmt, 3, userId);
    } else if (option == 2) {
        long long newPhone = readLongLong("Enter new phone number: ");
        const char *sql_update = "UPDATE Accounts SET phone = ? WHERE accountNbr = ? AND userId = ?;";
        rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
        if (rc != SQLITE_OK) handleUnexpectedError(db);

        sqlite3_bind_int64(stmt, 1, newPhone);
        sqlite3_bind_int(stmt, 2, accNbr);
        sqlite3_bind_int(stmt, 3, userId);
    } else {
        printf("Invalid option.\n");
        sqlite3_close(db);
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Update failed: %s\n", sqlite3_errmsg(db));
        // Optionally: handleUnexpectedError(db);
    } else {
        printf("Field updated successfully.\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

/**
 * checkAccountDetails - Displays details of a single account for the current user.
 */
void checkAccountDetails(struct User u) {
    int accNbr = readInt("Enter the account number to view details: ");
    
    sqlite3 *db = open_db_system();
    if (!db) handleUnexpectedError(db);

    int userId = getUserIdFromName(u.name);
    if (userId == -1) {
        printf("User not found.\n");
        sqlite3_close(db);
        return;
    }
    
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, accountNbr, depositMonth, depositDay, depositYear, country, phone, amount, accountType FROM Accounts WHERE accountNbr = ? AND userId = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, accNbr);
    sqlite3_bind_int(stmt, 2, userId);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int accNbrDb = sqlite3_column_int(stmt, 1);
        int depositMonth = sqlite3_column_int(stmt, 2);
        int depositDay = sqlite3_column_int(stmt, 3);
        int depositYear = sqlite3_column_int(stmt, 4);
        const unsigned char *country = sqlite3_column_text(stmt, 5);
        long long phone = sqlite3_column_int64(stmt, 6);
        double amount = sqlite3_column_double(stmt, 7);
        const unsigned char *accountType = sqlite3_column_text(stmt, 8);
        
        printf("Account Record ID: %d\n", id);
        printf("Account Number: %d\n", accNbrDb);
        printf("Deposit Date: %d/%d/%d\n", depositMonth, depositDay, depositYear);
        printf("Country: %s\n", country);
        printf("Phone: %lld\n", phone);
        printf("Amount: $%.2lf\n", amount);
        printf("Account Type: %s\n", accountType);
        
        // Example of interest logic
        double interest = 0.0;
        if (strcmp((const char*)accountType, "saving") == 0) {
            interest = (amount * 7.0 / 100.0) / 12.0;
            printf("You will get $%.2lf as interest on day %d of every month.\n", interest, depositDay);
        } else if (strcmp((const char*)accountType, "fixed01") == 0) {
            interest = amount * 4.0 / 100.0;
            printf("You will get $%.2lf as interest on %d/%d/%d.\n", interest, depositMonth, depositDay, depositYear + 1);
        } else if (strcmp((const char*)accountType, "fixed02") == 0) {
            interest = amount * 5.0 / 100.0 * 2;
            printf("You will get $%.2lf as interest on %d/%d/%d.\n", interest, depositMonth, depositDay, depositYear + 2);
        } else if (strcmp((const char*)accountType, "fixed03") == 0) {
            interest = amount * 8.0 / 100.0 * 3;
            printf("You will get $%.2lf as interest on %d/%d/%d.\n", interest, depositMonth, depositDay, depositYear + 3);
        } else {
            printf("No interest for current accounts.\n");
        }
    } else {
        printf("Account not found or does not belong to you.\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

/**
 * makeTransaction - Handles deposits or withdrawals, with special handling for fixed accounts.
 * (Optional) To allow after maturity, see maturity-check sample code in previous suggestions.
 */
void makeTransaction(struct User u) {
    int accNbr = readInt("Enter the account number for transaction: ");
    sqlite3 *db = open_db_system();
    if (!db) handleUnexpectedError(db);

    int userId = getUserIdFromName(u.name);
    if (userId == -1) {
        printf("User not found.\n");
        sqlite3_close(db);
        return;
    }
    
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, amount, accountType FROM Accounts WHERE accountNbr = ? AND userId = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, accNbr);
    sqlite3_bind_int(stmt, 2, userId);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Account not found or does not belong to you.\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        success(u);
        return;
    }
    
    double currentAmount = sqlite3_column_double(stmt, 1);
const unsigned char *dbAccountType = sqlite3_column_text(stmt, 2);

// Copy the text to a local buffer
char accountType[50];
strncpy(accountType, (const char*)dbAccountType, sizeof(accountType));
accountType[sizeof(accountType) - 1] = '\0';

sqlite3_finalize(stmt);

//printf("Current type: %s\n", accountType);


    // Basic approach: block all transactions on fixed accounts
    // If you want partial maturity checks, see older suggestions
    printf("Current type: %s\n", accountType);
    if (strcmp((const char*)accountType, "fixed01") == 0 ||
        strcmp((const char*)accountType, "fixed02") == 0 ||
        strcmp((const char*)accountType, "fixed03") == 0) {
        printf("Transactions are not allowed on fixed accounts.\n");
        sqlite3_close(db);
        success(u);
        return;
    }
    
    int transOption = readInt("Enter 1 for deposit, 2 for withdrawal: ");
    double amount;
    char inputBuffer[100];
    if (transOption == 1) {
        printf("Enter deposit amount: $");
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL || sscanf(inputBuffer, "%lf", &amount) != 1) {
            printf("Invalid deposit amount.\n");
            sqlite3_close(db);
            success(u);
            return;
        }
        currentAmount += amount;
        printf("Deposit successful. New balance: $%.2lf\n", currentAmount);
    } else if (transOption == 2) {
        printf("Enter withdrawal amount: $");
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL || sscanf(inputBuffer, "%lf", &amount) != 1) {
            printf("Invalid withdrawal amount.\n");
            sqlite3_close(db);
            success(u);
            return;
        }
        if (amount > currentAmount) {
            printf("Insufficient funds.\n");
            sqlite3_close(db);
            success(u);
            return;
        }
        currentAmount -= amount;
        printf("Withdrawal successful. New balance: $%.2lf\n", currentAmount);
    } else {
        printf("Invalid transaction option.\n");
        sqlite3_close(db);
        success(u);
        return;
    }
    
    const char *sql_update = "UPDATE Accounts SET amount = ? WHERE accountNbr = ? AND userId = ?;";
    rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_double(stmt, 1, currentAmount);
    sqlite3_bind_int(stmt, 2, accNbr);
    sqlite3_bind_int(stmt, 3, userId);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Update failed: %s\n", sqlite3_errmsg(db));
        // Optionally: handleUnexpectedError(db);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

/**
 * removeAccount - Removes an existing account, verifying it belongs to the current user.
 */
void removeAccount(struct User u) {
    int accNbr = readInt("Enter the account number to remove: ");
    
    sqlite3 *db = open_db_system();
    if (!db) handleUnexpectedError(db);

    int userId = getUserIdFromName(u.name);
    if (userId == -1) handleUnexpectedError(db);

    sqlite3_stmt *stmt;
    const char *sql_check = "SELECT id FROM Accounts WHERE accountNbr = ? AND userId = ?;";
    int rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, accNbr);
    sqlite3_bind_int(stmt, 2, userId);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Account not found or does not belong to you.\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        success(u);
        return;
    }
    sqlite3_finalize(stmt);
    
    const char *sql_delete = "DELETE FROM Accounts WHERE accountNbr = ? AND userId = ?;";
    rc = sqlite3_prepare_v2(db, sql_delete, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, accNbr);
    sqlite3_bind_int(stmt, 2, userId);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Delete failed: %s\n", sqlite3_errmsg(db));
        // Optionally: handleUnexpectedError(db);
    } else {
        printf("Account removed successfully.\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

/**
 * transferOwner - Transfers an account from the current user to another user (must exist in DB).
 */
void transferOwner(struct User u) {
    int accNbr = readInt("Enter the account number to transfer: ");
    char newOwnerName[50];
    printf("Enter the username of the new owner: ");
    if (fgets(newOwnerName, sizeof(newOwnerName), stdin) == NULL) {
        printf("Error reading username.\n");
        return;
    }
    newOwnerName[strcspn(newOwnerName, "\n")] = '\0';
    
    sqlite3 *db = open_db_system();
    if (!db) handleUnexpectedError(db);

    sqlite3_stmt *stmt;
    const char *sql_user = "SELECT id FROM Users WHERE name = ?;";
    int rc = sqlite3_prepare_v2(db, sql_user, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_text(stmt, 1, newOwnerName, -1, SQLITE_STATIC);
    int newUserId = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        newUserId = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    
    if (newUserId == -1) {
        printf("User %s not found.\n", newOwnerName);
        sqlite3_close(db);
        success(u);
        return;
    }
    
    int userId = getUserIdFromName(u.name);
    if (userId == -1) handleUnexpectedError(db);

    const char *sql_check2 = "SELECT id FROM Accounts WHERE accountNbr = ? AND userId = ?;";
    rc = sqlite3_prepare_v2(db, sql_check2, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, accNbr);
    sqlite3_bind_int(stmt, 2, userId);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        printf("Account not found or does not belong to you.\n");
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        success(u);
        return;
    }
    sqlite3_finalize(stmt);
    
    const char *sql_update = "UPDATE Accounts SET userId = ? WHERE accountNbr = ? AND userId = ?;";
    rc = sqlite3_prepare_v2(db, sql_update, -1, &stmt, 0);
    if (rc != SQLITE_OK) handleUnexpectedError(db);

    sqlite3_bind_int(stmt, 1, newUserId);
    sqlite3_bind_int(stmt, 2, accNbr);
    sqlite3_bind_int(stmt, 3, userId);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Update failed: %s\n", sqlite3_errmsg(db));
        // Optionally: handleUnexpectedError(db);
    } else {
        printf("Account transferred successfully to %s.\n", newOwnerName);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}
