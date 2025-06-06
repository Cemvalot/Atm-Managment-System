# ATM Management System

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Building the Project](#building-the-project)
  - [Running the Program](#running-the-program)
- [Usage Examples](#usage-examples)
- [Login Credentials Storage](#login-credentials-storage)
- [Database Schema Overview](#database-schema-overview)
- [Data Files](#data-files)
- [Notes](#notes)
- [Contributing](#contributing)
- [License](#license)
- [Authors](#authors)
- [Contact](#contact)

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

### Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd atm-management-system
   ```
2. Ensure you have the required dependencies installed:
   - On Ubuntu/Debian:
     ```bash
     sudo apt-get install build-essential libsqlite3-dev
     ```
   - On macOS (with Homebrew):
     ```bash
     brew install sqlite3
     ```
   - On Windows: Install MinGW and SQLite3 development files.

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

## Usage Examples

- **Login:** Enter your username and password at the prompt.
- **Check Balance:** Select the "Check Balance" option from the menu.
- **Deposit Funds:** Choose "Deposit" and enter the amount to deposit.
- **Withdraw Funds:** Choose "Withdraw" and enter the amount to withdraw (subject to balance).
- **View Transaction History:** Select the corresponding menu option to see your recent transactions.

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
* For Windows users, ensure the executable is run from a terminal that supports ANSI escape codes for best display.

## Contributing

Contributions are welcome! To contribute:
1. Fork the repository
2. Create a new branch for your feature or bugfix
3. Commit your changes with clear messages
4. Open a pull request describing your changes

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Authors

* Cemvalot

## Contact

For questions, suggestions, or support, please open an issue on the repository or contact Cemvalot via GitHub.
