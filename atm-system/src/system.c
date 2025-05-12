#include "header.h"

const char *RECORDS = "./data/records.txt";

int getAccountFromFile(FILE *ptr, char name[50], struct Record *r)
{
    return fscanf(ptr, "%d %d %s %d %d/%d/%d %s %d %lf %s",
                  &r->id,
                  &r->userId,
                  name,
                  &r->accountNbr,
                  &r->deposit.month,
                  &r->deposit.day,
                  &r->deposit.year,
                  r->country,
                  &r->phone,
                  &r->amount,
                  r->accountType) != EOF;
}

void saveAccountToFile(FILE *ptr, struct User u, struct Record r)
{
    fprintf(ptr, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
            r.id,
            u.id,
            u.name,
            r.accountNbr,
            r.deposit.month,
            r.deposit.day,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
}

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0)
    {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            goto invalid;
        }
    }
    else
    {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1)
    {
        system("clear");
        mainMenu(u);
    }
    else
    {
        system("clear");
        exit(1);
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1)
    {
        mainMenu(u);
    }
    else if (option == 0)
    {
        exit(1);
    }
    else
    {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void createNewAcc(struct User u)
{
    struct Record r;
    struct Record cr;
    char userName[50];
    FILE *pf = fopen(RECORDS, "a+");

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");

    printf("\nEnter today's date(mm/dd/yyyy):");
    scanf("%d/%d/%d", &r.deposit.month, &r.deposit.day, &r.deposit.year);
    printf("\nEnter the account number:");
    scanf("%d", &r.accountNbr);

    while (getAccountFromFile(pf, userName, &cr))
    {
        if (strcmp(userName, u.name) == 0 && cr.accountNbr == r.accountNbr)
        {
            printf("✖ This Account already exists for this user\n\n");
            goto noAccount;
        }
    }
    printf("\nEnter the country:");
    scanf("%s", r.country);
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);

    saveAccountToFile(pf, u, r);

    fclose(pf);
    success(u);
}

void checkAllAccounts(struct User u)
{
    char userName[100];
    struct Record r;

    FILE *pf = fopen(RECORDS, "r");

    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0)
        {
            printf("_____________________\n");
            printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n",
                   r.accountNbr,
                   r.deposit.day,
                   r.deposit.month,
                   r.deposit.year,
                   r.country,
                   r.phone,
                   r.amount,
                   r.accountType);
        }
    }
    fclose(pf);
    success(u);
}
// Function to update account information (with validation)
void updateAccount(struct User u)
{
    struct Record r;
    char userName[100];
    int choice, accountNum, found = 0;
    
    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    
    system("clear");
    printf("\t\t====== Update Account Information =====\n\n");
    
    // Account number validation
    do {
        printf("Enter account number (max 5 digits): ");
        scanf("%d", &accountNum);
        
        if (accountNum > 99999) {
            printf("⚠️ Account number cannot exceed 5 digits. Please try again.\n");
        }
    } while (accountNum > 99999);
    
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum)
        {
            found = 1;
            printf("\nAccount found!\n");
            printf("\nChoose what you want to update:");
            printf("\n1. Country");
            printf("\n2. Phone");
            printf("\n3. Account Type");
            printf("\n4. Cancel\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            
            switch(choice)
            {
                case 1:
                    printf("Enter new country: ");
                    scanf("%s", r.country);
                    break;
                case 2:
                    // Phone number validation
                    do {
                        printf("Enter new phone number (max 10 digits): ");
                        scanf("%d", &r.phone);
                        
                        if (r.phone > 9999999999) {
                            printf("⚠️ Phone number cannot exceed 10 digits. Please try again.\n");
                        }
                    } while (r.phone > 9999999999);
                    break;
                case 3:
                    printf("\nChoose the new type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice: ");
                    scanf("%s", r.accountType);
                    break;
                case 4:
                    printf("Update canceled.\n");
                    break;
                default:
                    printf("Invalid option!\n");
                    break;
            }
            
            if (choice >= 1 && choice <= 3)
            {
                fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                        r.id, r.userId, userName, r.accountNbr,
                        r.deposit.month, r.deposit.day, r.deposit.year,
                        r.country, r.phone, r.amount, r.accountType);
            }
            else
            {
                fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                        r.id, r.userId, userName, r.accountNbr,
                        r.deposit.month, r.deposit.day, r.deposit.year,
                        r.country, r.phone, r.amount, r.accountType);
            }
        }
        else
        {
            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id, r.userId, userName, r.accountNbr,
                    r.deposit.month, r.deposit.day, r.deposit.year,
                    r.country, r.phone, r.amount, r.accountType);
        }
    }
    
    fclose(pf);
    fclose(tempFile);
    
    remove(RECORDS);
    rename("temp.txt", RECORDS);
    
    stayOrReturn(found, updateAccount, u);
}

// Function to check details of a specific account (with validation)
void checkAccount(struct User u)
{
    struct Record r;
    char userName[100];
    int accountNum, found = 0;
    
    FILE *pf = fopen(RECORDS, "r");
    
    system("clear");
    printf("\t\t====== Check Account Details =====\n\n");
    
    // Account number validation
    do {
        printf("Enter account number (max 5 digits): ");
        scanf("%d", &accountNum);
        
        if (accountNum > 99999) {
            printf("⚠️ Account number cannot exceed 5 digits. Please try again.\n");
        }
    } while (accountNum > 99999);
    
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum)
        {
            found = 1;
            printf("\n\t====== Account Details =====\n");
            printf("\nAccount number: %d", r.accountNbr);
            printf("\nDeposit Date: %d/%d/%d", r.deposit.day, r.deposit.month, r.deposit.year);
            printf("\nCountry: %s", r.country);
            printf("\nPhone number: %d", r.phone);
            printf("\nAmount deposited: $%.2f", r.amount);
            printf("\nType Of Account: %s\n", r.accountType);
            break;
        }
    }
    
    fclose(pf);
    stayOrReturn(found, checkAccount, u);
}

// Function to make transactions (deposit or withdraw) with validation
void makeTransaction(struct User u)
{
    struct Record r;
    char userName[100];
    int choice, accountNum, found = 0;
    double amount;
    
    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    
    system("clear");
    printf("\t\t====== Make Transaction =====\n\n");
    
    // Account number validation
    do {
        printf("Enter account number (max 5 digits): ");
        scanf("%d", &accountNum);
        
        if (accountNum > 99999) {
            printf("⚠️ Account number cannot exceed 5 digits. Please try again.\n");
        }
    } while (accountNum > 99999);
    
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum)
        {
            found = 1;
            printf("\nAccount found!\n");
            printf("\nCurrent balance: $%.2f", r.amount);
            printf("\n\nChoose transaction type:");
            printf("\n1. Deposit");
            printf("\n2. Withdraw\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            
            switch(choice)
            {
                case 1:
                    printf("Enter amount to deposit: $");
                    scanf("%lf", &amount);
                    r.amount += amount;
                    printf("\nDeposit successful. New balance: $%.2f\n", r.amount);
                    break;
                case 2:
                    printf("Enter amount to withdraw: $");
                    scanf("%lf", &amount);
                    if (amount > r.amount)
                    {
                        printf("\n⚠️ Insufficient funds!\n");
                    }
                    else
                    {
                        r.amount -= amount;
                        printf("\nWithdrawal successful. New balance: $%.2f\n", r.amount);
                    }
                    break;
                default:
                    printf("Invalid option!\n");
                    break;
            }
            
            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id, r.userId, userName, r.accountNbr,
                    r.deposit.month, r.deposit.day, r.deposit.year,
                    r.country, r.phone, r.amount, r.accountType);
        }
        else
        {
            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id, r.userId, userName, r.accountNbr,
                    r.deposit.month, r.deposit.day, r.deposit.year,
                    r.country, r.phone, r.amount, r.accountType);
        }
    }
    
    fclose(pf);
    fclose(tempFile);
    
    remove(RECORDS);
    rename("temp.txt", RECORDS);
    
    stayOrReturn(found, makeTransaction, u);
}

// Function to remove an existing account (with validation)
void removeAccount(struct User u)
{
    struct Record r;
    char userName[100];
    int accountNum, found = 0, confirm;
    
    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    
    system("clear");
    printf("\t\t====== Remove Existing Account =====\n\n");
    
    // Account number validation
    do {
        printf("Enter account number to remove (max 5 digits): ");
        scanf("%d", &accountNum);
        
        if (accountNum > 99999) {
            printf("⚠️ Account number cannot exceed 5 digits. Please try again.\n");
        }
    } while (accountNum > 99999);
    
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum)
        {
            found = 1;
            printf("\nAccount found!\n");
            printf("\nAccount Details:");
            printf("\nAccount number: %d", r.accountNbr);
            printf("\nBalance: $%.2f", r.amount);
            printf("\n\nAre you sure you want to remove this account? (1 for Yes, 0 for No): ");
            scanf("%d", &confirm);
            
            if (confirm != 1)
            {
                fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                        r.id, r.userId, userName, r.accountNbr,
                        r.deposit.month, r.deposit.day, r.deposit.year,
                        r.country, r.phone, r.amount, r.accountType);
                printf("\nAccount removal canceled.\n");
            }
            else
            {
                printf("\nAccount successfully removed.\n");
            }
        }
        else
        {
            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id, r.userId, userName, r.accountNbr,
                    r.deposit.month, r.deposit.day, r.deposit.year,
                    r.country, r.phone, r.amount, r.accountType);
        }
    }
    
    fclose(pf);
    fclose(tempFile);
    
    remove(RECORDS);
    rename("temp.txt", RECORDS);
    
    stayOrReturn(found, removeAccount, u);
}

// Function to transfer ownership of an account (with validation)
void transferOwnership(struct User u)
{
    struct Record r;
    char userName[100], newOwner[50];
    int accountNum, found = 0, userFound = 0;
    
    FILE *pf = fopen(RECORDS, "r");
    FILE *tempFile = fopen("temp.txt", "w");
    FILE *userFile = fopen("./data/users.txt", "r");
    struct User userChecker;
    
    system("clear");
    printf("\t\t====== Transfer Account Ownership =====\n\n");
    
    // Account number validation
    do {
        printf("Enter account number to transfer (max 5 digits): ");
        scanf("%d", &accountNum);
        
        if (accountNum > 99999) {
            printf("⚠️ Account number cannot exceed 5 digits. Please try again.\n");
        }
    } while (accountNum > 99999);
    
    while (getAccountFromFile(pf, userName, &r))
    {
        if (strcmp(userName, u.name) == 0 && r.accountNbr == accountNum)
        {
            found = 1;
            printf("\nAccount found!\n");
            printf("Enter username to transfer ownership to: ");
            scanf("%s", newOwner);
            
            // Check if the user exists
            rewind(userFile);
            while (fscanf(userFile, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF)
            {
                if (strcmp(userChecker.name, newOwner) == 0)
                {
                    userFound = 1;
                    break;
                }
            }
            
            if (userFound)
            {
                fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                        r.id, userChecker.id, newOwner, r.accountNbr,
                        r.deposit.month, r.deposit.day, r.deposit.year,
                        r.country, r.phone, r.amount, r.accountType);
                printf("\nOwnership successfully transferred to %s.\n", newOwner);
            }
            else
            {
                fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                        r.id, r.userId, userName, r.accountNbr,
                        r.deposit.month, r.deposit.day, r.deposit.year,
                        r.country, r.phone, r.amount, r.accountType);
                printf("\n⚠️ User not found! Transfer canceled.\n");
            }
        }
        else
        {
            fprintf(tempFile, "%d %d %s %d %d/%d/%d %s %d %.2lf %s\n\n",
                    r.id, r.userId, userName, r.accountNbr,
                    r.deposit.month, r.deposit.day, r.deposit.year,
                    r.country, r.phone, r.amount, r.accountType);
        }
    }
    
    fclose(pf);
    fclose(tempFile);
    fclose(userFile);
    
    remove(RECORDS);
    rename("temp.txt", RECORDS);
    
    stayOrReturn(found, transferOwnership, u);
}

// Function to register a new user
void registerMenu(char name[50], char password[50])
{
    struct User userChecker;
    FILE *fp;
    int id = 0;
    int userExists = 0;
    
    system("clear");
    printf("\n\n\t\t\t\tBank Management System\n\t\t\t\t User Registration\n");
    
    printf("\n\t\t\t\tEnter Username: ");
    scanf("%s", name);
    
    // Check if username already exists
    if ((fp = fopen("./data/users.txt", "r")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
    
    while (fscanf(fp, "%d %s %s", &userChecker.id, userChecker.name, userChecker.password) != EOF)
    {
        if (strcmp(userChecker.name, name) == 0)
        {
            printf("\n\t\t\t\t⚠️ Username already exists! Please choose another.\n");
            fclose(fp);
            registerMenu(name, password);
            return;
        }
        id = userChecker.id;
    }
    fclose(fp);
    
    // Get new password with hidden input
    struct termios oflags, nflags;
    
    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;
    
    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }
    
    printf("\n\t\t\t\tEnter Password: ");
    scanf("%s", password);
    
    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        exit(1);
    }
    
    // Save new user
    if ((fp = fopen("./data/users.txt", "a")) == NULL)
    {
        printf("Error! opening file");
        exit(1);
    }
    
    fprintf(fp, "%d %s %s\n", id + 1, name, password);
    fclose(fp);
    
    printf("\n\n\t\t\t\t✔ Registration Successful!\n");
}