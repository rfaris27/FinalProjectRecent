# FinalExam updated — ATM Console Application

I will be documenting each step during this process to keep track of my work

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
2. **Reopen in Container** (uses [`.devcontainer/docker-compose.yml`](.devcontainer/docker-compose.yml)). After load, **`postAttachCommand`** runs [`scripts/apply-schema.sh`](scripts/apply-schema.sh) so `accounts` exists.
3. **Important:** [`schema.sql`](schema.sql) is only a **text recipe**. Logins like `Adnan123` exist **after** that file is executed against MySQL—not because it sits in the repo. On a **brand-new** Docker volume, MySQL may auto-run it once (see compose `docker-entrypoint-initdb.d` mount). If the table is missing, load it yourself from the **repo root** (use your real path, e.g. `~/Documents/GitHub/recentFinal1` — not `/path/to/...`):

```bash
cd ~/Documents/GitHub/recentFinal1   # example: wherever you cloned this repo
chmod +x scripts/apply-schema.sh scripts/check-db.sh
./scripts/check-db.sh
./scripts/apply-schema.sh
```

On your **Mac** (MySQL published on port **3307**), if `mysql` is installed:

```bash
cd ~/Documents/GitHub/recentFinal1
DB_HOST=127.0.0.1 DB_PORT=3307 ./scripts/check-db.sh
DB_HOST=127.0.0.1 DB_PORT=3307 ./scripts/apply-schema.sh
```

Or with Docker only:

```bash
docker compose -f .devcontainer/docker-compose.yml up -d db
docker compose -f .devcontainer/docker-compose.yml exec -T db mysql -u root -pp ATM < schema.sql
```

### Dev Container will not build (VS Code exit code 1)
- Use **VS Code from `/Applications`** (not a translocated copy from Downloads); translocated apps often break Dev Containers.
- Ensure **Docker Desktop** is running.
- If `docker compose ... up` fails with a **container name** conflict, stop the other project’s stack or run `docker compose -f .devcontainer/docker-compose.yml down` in the other repo first.

### Build with CMake
From the **repository root** (e.g. `/workspaces` in the dev container), not inside `build/`:

```bash
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j$(nproc)
./build/debug/atm_app
```

### Run the ATM app in VS Code (common mistakes)
1. **Use a terminal inside the Dev Container** (status bar shows **Dev Container**). **`db`** as MySQL hostname only works there.
2. **On the Mac**, use `DB_HOST=127.0.0.1 DB_PORT=3307` with `./scripts/check-db.sh` — not plain `./scripts/check-db.sh`.
3. **Port 3307 in use** — add `.env` with `ATM_HOST_DB_PORT=3308` (see [`.env.example`](.env.example)), `docker compose ... up -d` again, then use `DB_PORT=3308` for host scripts.
4. **Rebuild** after code changes: `cmake --build build/debug -j$(nproc)`.

### Run Tests
```bash
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j$(nproc)
ctest --test-dir build/debug --output-on-failure
# or: ./build/debug/atm_tests
```

### CMake cache errors (`/repo` vs `/workspaces`)
If CMake complains that `CMakeCache.txt` was created in a **different directory** (often after Docker vs dev-container path changes), delete the stale build folder and reconfigure:

```bash
rm -rf build/debug build/release
cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug
```

### Full Build Script
```bash
./build.sh
```
This runs the full pipeline (same entry point as CI): **clang-format** (strict), **cppcheck** (via `compile_commands.json`), **CMake** Debug + Release builds, **CTest**, **lcov** coverage on `src/` with a **90%** line gate, **genhtml**, and **Doxygen**.

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
| cppcheck | Static analysis (uses Debug `compile_commands.json`) |
| Doxygen | API documentation generation |
| EditorConfig | Consistent editor settings |
| GitHub Actions | CI/CD pipeline |

## CI/CD Pipeline

Every push and pull request to `main` runs **`./build.sh`** in GitHub Actions (after installing dependencies): formatting, cppcheck, Debug/Release builds, CTest, coverage report & **90%** threshold, Doxygen. Generated **documentation**, **coverage HTML**, and **Google Test XML** are uploaded as workflow artifacts.

## Diagrams

All use case, system, class, component, and deployment diagrams are in [`DIAGRAMS.md`](DIAGRAMS.md). Console UI text wireframes are in the same file under **Console UI mocks** (after the use case descriptions).

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
- [x] Console UI text wireframes (`DIAGRAMS.md`)

## Command reference

Quick list of common commands from the **repository root** (where `CMakeLists.txt` lives). For Mac vs dev container and DB ports, see **Setup** and **Build with CMake** above.

| Command | What it does |
|--------|----------------|
| `cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug` | Configure a **Debug** build (app + tests). |
| `cmake --build build/debug -j$(nproc)` | Compile inside **Linux/container** (`nproc` = CPU count). On macOS: `-j$(sysctl -n hw.ncpu)` or `-j4`. |
| `./build/debug/atm_app` | Run the ATM console app (defaults: host `db`, port `3306`, user `root`, password `p`, database `ATM`; override with `DB_*` env vars). |
| `DB_HOST=127.0.0.1 DB_PORT=3307 ./build/debug/atm_app` | Run from the **host** when MySQL is mapped to **3307** (typical for this repo’s Docker compose). |
| `ctest --test-dir build/debug --output-on-failure` | Run **CTest** (includes Google Test `ATMTests`). |
| `./build/debug/atm_tests` | Run the **unit test** binary directly. |
| `cmake --build build/debug --target docs` | Generate **Doxygen** HTML (only if CMake found Doxygen and defined the `docs` target). |
| `doxygen Doxyfile` | Generate Doxygen output from the repo root (requires `doxygen` on `PATH`). |
| `./build.sh` | Full **CI-style** pipeline: format, cppcheck, Debug + Release, CTest, coverage gate, Doxygen. |
| `./scripts/check-db.sh` | Check database connectivity (optional `DB_HOST`, `DB_PORT`). |
| `./scripts/apply-schema.sh` | Apply `schema.sql` so sample accounts exist. |

### Sample logins (after `schema.sql` is applied)

| Login | PIN | Role |
|-------|-----|------|
| `Adnan123` | `12345` | Administrator |
| `John456` | `11111` | Customer |
