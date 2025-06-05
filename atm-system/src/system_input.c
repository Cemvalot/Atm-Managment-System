#include <stdio.h>
#include <ctype.h>
#include "header.h"

void clearInputBuffer(void) {
    while (getchar() != '\n');
}

int readInt(const char *prompt) {
    char buffer[100];
    int value;
    printf("%s", prompt);
    fflush(stdout);
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            continue;
        if (buffer[0] == '\n')
            continue;
        if (sscanf(buffer, "%d", &value) == 1)
            return value;
        printf("Invalid input. Please enter a number: ");
        fflush(stdout);
    }
}

long long readLongLong(const char *prompt) {
    char buffer[100];
    long long value;
    int valid = 0;
    while (!valid) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            continue;
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove trailing newline

        // Ensure input is not empty
        if (buffer[0] == '\0') {
            printf("Input cannot be empty. Please enter a phone number.\n");
            continue;
        }
        
        // Check if input contains only digits
        int isAllDigits = 1;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isdigit((unsigned char)buffer[i])) {
                isAllDigits = 0;
                break;
            }
        }
        if (!isAllDigits) {
            printf("Invalid phone number. Please enter only numeric digits.\n");
            continue;
        }
        if (sscanf(buffer, "%lld", &value) == 1) {
            valid = 1;
        } else {
            printf("Invalid input. Please enter only numbers.\n");
        }
    }
    return value;
}

double readDouble(const char *prompt) {
    char buffer[100];
    double value;
    int valid = 0;
    while (!valid) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            continue;
        buffer[strcspn(buffer, "\n")] = '\0';  // Remove trailing newline

        // Ensure the input is a valid double (and nothing extra)
        char extra;
        if (sscanf(buffer, " %lf %c", &value, &extra) == 1) {
            valid = 1;
        } else {
            printf("Invalid amount. Please enter a valid number.\n");
        }
    }
    return value;
}
