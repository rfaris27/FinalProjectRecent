# MidTermExam — ATM Console Application

I will be documenting each steps during this process to keep track of my work

## Overview
A C++ console-based ATM system connected to a MySQL database running in Docker. Supports two user types: **Customers** and **Administrators**.

## Software Architecture

**3-Layer Architecture** with dependency injection:

| Layer | Folder | Purpose |
|-------|--------|---------|
| Presentation | `src/ui/` | Console I/O only. No SQL, no business logic. |
| Business Logic | `src/model/` | Validation, rules, data classes. Calls DAL for data. |
| Data Access (DAL) | `src/DAL/` | SQL only. No validation, no cout, no business rules. |

The model layer depends on abstract DAL interfaces (e.g., `ILoginDal`), not concrete MySQL implementations. This enables unit testing with mock DALs (no database needed).

The `Account` class is **immutable** — all fields set via constructor, all members `private const`, zero setters. To "change" a balance, create a new Account object.

## Project Structure

```
src/
├── main.cpp                        # Wires DAL -> model -> UI
├── model/
│   ├── Account.h                   # Immutable Account class
│   ├── Login.h                     # Authentication logic
│   ├── CustomerOptions.h           # Withdraw, Deposit, Balance logic
│   └── AdminOptions.h              # Account CRUD logic
├── DAL/
│   ├── LoginDal.h / .cpp           # SQL: credential queries
│   ├── CustomerOptionsDal.h / .cpp # SQL: balance updates
│   └── AdminOptionsDal.h / .cpp    # SQL: account CRUD
└── ui/
    └── ConsoleUI.h                 # All console I/O and menus

tests/
├── MockDal.h                       # In-memory mock DALs
├── test_account.cpp
├── test_login.cpp
├── test_customer_options.cpp
└── test_admin_options.cpp
```

## Setup
1. Open the project in VS Code with the Dev Containers extension
2. Reopen in container (it uses `docker-compose.yml`)
3. Run `schema.sql` against the MySQL database to create the `accounts` table

### Build with CMake
```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./atm_app
```

### Run Tests
```bash
cd build
./atm_tests
```

### Full Build Script
```bash
./build.sh
```
This runs: format check, debug build, release build, Doxygen docs, unit tests, and coverage report.

## Database Connection
- **Host:** `db` (Docker service name, used inside the dev container)
- **Port:** `3306`
- **User:** `root`
- **Password:** `p`
- **Database:** `ATM`

For the VS Code SQL extension (connecting from host machine), use `127.0.0.1:3307`.

## Recreating the Database
To recreate the database from the dump file, run:
```bash
mysql -u root -p ATM < db_dump.sql
```

## Features

### Login System
- Single login screen for both customers and admins
- Validates login and 5-digit pin against the database
- After login, user selects role (Customer or Administrator)
- Admin access is restricted to accounts with `IsAdmin = TRUE`

### Customer Options
- **1----Withdraw Cash** — Validates amount (positive, sufficient funds), updates DB, shows receipt
- **3----Deposit Cash** — Validates amount, updates DB, shows receipt
- **4----Display Balance** — Shows account #, date, and current balance
- **5----Exit** — Returns to login screen

### Admin Options
- **1----Create New Account** — Validates pin (5-digit integer), checks for empty fields
- **2----Delete Existing Account** — Shows holder name, asks for re-confirmation
- **3----Update Account Information** — Displays current info, allows updating any field
- **4----Search for Account** — Displays full account details
- **6----Exit** — Returns to login screen

## Tooling

| Tool | Purpose |
|------|---------|
| CMake | Build system (debug, release, tests, docs) |
| Google Test | Unit testing framework |
| gcov + lcov | Code coverage (target: 90%+) |
| clang-format | Code formatting (Google style, 4-space indent) |
| Doxygen | API documentation generation |
| EditorConfig | Consistent editor settings |
| GitHub Actions | CI/CD pipeline |

## CI/CD Pipeline

Every push to `main` triggers GitHub Actions which:
1. Checks code formatting with clang-format
2. Builds in Debug mode
3. Builds in Release mode
4. Generates Doxygen documentation
5. Runs unit tests
6. Generates coverage report (fails if below 90%)

## Diagrams
All use case, system, class, component, and deployment diagrams are in [`DIAGRAMS.md`](DIAGRAMS.md).

## Progress

- [x] Docker environment setup (app + MySQL containers)
- [x] Database schema (`accounts` table)
- [x] Immutable Account class (constructor, const members, no setters)
- [x] 3-layer architecture (UI / model / DAL)
- [x] Login system with dependency injection
- [x] Customer menu (Withdraw, Deposit, Display Balance)
- [x] Admin menu (Create, Delete, Update, Search)
- [x] Unit tests with MockDal (Google Test)
- [x] CMake build system
- [x] Doxygen documentation
- [x] clang-format linter
- [x] EditorConfig
- [x] build.sh script
- [x] GitHub Actions CI/CD
- [x] Class, component, and deployment diagrams
