# ATM Management System

## Overview

The **ATM Management System** is a terminal-based project written in C that simulates basic ATM operations. It allows users to log in and perform actions such as checking balance, withdrawing money, depositing funds, and viewing transaction history.

## Features

* User authentication via stored credentials
* Secure password hashing (e.g., SHA-256 or equivalent custom hashing logic)
* Deposit and withdrawal functionalities
* Transaction history logging
* Data persistence using a lightweight database file (`atm.db`)
* Input validation and error handling
* Modular code structure for maintainability and scalability
* Clear terminal-based menu system for navigation

## Project Structure

```
atm-system/
├── Makefile               # Build automation
├── .gitignore             # Git ignore rules
├── src/                   # Source code directory
│   ├── main.c             # Entry point
│   ├── auth.c             # Handles user authentication
│   ├── header.h           # Header declarations
│   ├── system.c           # Core ATM logic and program flow
│   ├── system_db.c        # Handles database interactions and queries
│   ├── system_input.c     # Input validation and user interaction
│   ├── system_menu.c      # Terminal menu system
│   └── data/              # Data storage files
│       └── atm.db         # SQLite-style DB file storing users and records
```

## Getting Started

### Prerequisites

* GCC compiler (Linux, macOS, or Windows with MinGW)
* SQLite3 development library (if using SQLite features)

### Building the Project

1. Open a terminal in the `atm-system` directory.
2. Run the following command:

   ```bash
   make
   ```

   This compiles all source files and creates an executable binary.

### Running the Program

Run the compiled binary:

```bash
./atm
```

You will be prompted with a secure login screen followed by an interactive menu system.

## Login Credentials Storage

* Usernames and hashed passwords are securely stored in the `atm.db` file.
* During login, input passwords are hashed and compared against the stored hash.
* This approach ensures credentials are not stored in plain text and improves security.

## Database Schema Overview

While `atm.db` is a simple file-based database, it may include tables such as:

* `users`: stores user IDs, usernames, hashed passwords, account balances
* `transactions`: logs withdrawals, deposits, timestamps, and amounts

## Data Files

* **atm.db**: Core database file that holds persistent user and transaction data.

## Notes

* Ensure the `data/` folder has read/write permissions.
* The hashing logic should be reviewed and tested for robustness.
* Additional features like password recovery, multi-user admin access, or account locking could be implemented.

## Authors

* Cemvalot

---