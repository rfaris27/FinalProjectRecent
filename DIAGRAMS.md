# ATM System Diagrams

## 1. Use Case Diagram

### Actors
- **Customer** — A user with a bank account who can perform transactions
- **Administrator** — A privileged user who manages customer accounts

```mermaid
graph TD
    UC1[Login]

    Customer((Customer))
    Admin((Administrator))

    UC1 --> Customer
    UC1 --> Admin

    subgraph Customer Options
        UC2[Withdraw Cash]
        UC3[Deposit Cash]
        UC4[Display Balance]
        UC5[Exit]
    end

    subgraph Admin Options
        UC6[Create New Account]
        UC7[Delete Existing Account]
        UC8[Update Account Information]
        UC9[Search for Account]
        UC10[Exit]
    end

    Customer --> UC2
    Customer --> UC3
    Customer --> UC4
    Customer --> UC5

    Admin --> UC6
    Admin --> UC7
    Admin --> UC8
    Admin --> UC9
    Admin --> UC10

    DB[(MySQL Database)]

    UC1 -- "Reads credentials" --> DB
    UC2 -- "Updates balance" --> DB
    UC3 -- "Updates balance" --> DB
    UC4 -- "Reads balance" --> DB
    UC6 -- "Inserts new account" --> DB
    UC7 -- "Deletes account" --> DB
    UC8 -- "Updates account info" --> DB
    UC9 -- "Reads account info" --> DB
```

### Use Case Descriptions

| Use Case | Actor(s) | Description |
|----------|----------|-------------|
| Login | Customer, Admin | User enters login and 5-digit pin. System verifies credentials against the database. |
| Select Role | Customer, Admin | After login, user selects Customer or Administrator role. Access is validated. |
| Withdraw Cash | Customer | Customer enters amount to withdraw. System validates (positive, sufficient funds), updates balance in DB, displays receipt. |
| Deposit Cash | Customer | Customer enters amount to deposit. System validates (positive), updates balance in DB, displays receipt. |
| Display Balance | Customer | System displays account number, current date, and balance. |
| Create New Account | Admin | Admin enters login, pin, name, balance, status. System validates pin (5 digits), checks for duplicate login, creates account. |
| Delete Existing Account | Admin | Admin enters account number. System shows holder name, asks for re-confirmation, deletes account. |
| Update Account Information | Admin | Admin enters account number. System shows current info. Admin can update holder, balance, status, login, or pin. |
| Search for Account | Admin | Admin enters account number. System displays full account details. |
| Logout / Exit | Customer, Admin | User exits their menu and returns to the login screen. |

---

## 2. System Diagram for entire application

```mermaid
flowchart TD
    A[Start Application] --> B[Connect to MySQL Database]
    B --> C[Display Login Screen - Any User]
    C --> D[Enter Login & Pin]

    subgraph "Model / DAL"
        M1[Login.h - Verify Credentials]
        M2[CustomerOptions.h - Process Transaction]
        M3[AdminOptions.h - Manage Accounts]
        M4[Account.h - Account Object]
    end

    subgraph "MySQL Database"
        DB[(accounts table)]
    end

    D --> M1
    M1 -- "SELECT WHERE Login & Pin" --> DB
    DB -- "Return account data" --> M4
    M1 -- Valid --> G[Select Role]
    M1 -- Invalid --> F[Display Error]
    F --> C

    G --> H{Role Choice}

    H -- Customer --> I[Customer Menu]
    I --> I1{Choice}
    I1 -- 1 --> I2[Withdraw Cash]
    I1 -- 3 --> I3[Deposit Cash]
    I1 -- 4 --> I4[Display Balance]
    I1 -- 5 --> LOGOUT1[Logout]
    LOGOUT1 --> C

    I2 -- "Enter Amount" --> M2
    M2 -- "UPDATE Balance" --> DB
    M2 -- "Show Receipt" --> I
    I3 -- "Enter Amount" --> M2
    I4 -- "Read from Account object" --> M4
    M4 -- "Display Info" --> I

    H -- Administrator --> J{Is Admin?}
    J -- No --> K[Access Denied]
    K --> C
    J -- Yes --> L[Admin Menu]
    L --> L1{Choice}

    L1 -- 1 --> L2[Create Account]
    L2 --> L2a[Enter New Customer Login, Pin, Name, Balance, Status]
    L2a --> M3
    M3 -- "INSERT new account" --> DB
    M3 -- "Account Created with Assigned Number" --> L

    L1 -- 2 --> L3[Delete Account]
    L3 --> L3a[Enter Account Number]
    L3a -- "SELECT holder name" --> DB
    DB -- "Show Holder Name" --> L3b[Re-enter Account Number to Confirm]
    L3b --> M3
    M3 -- "DELETE account" --> DB

    L1 -- 3 --> L4[Update Account]
    L4 --> L4a[Enter Account Number]
    L4a -- "SELECT account info" --> DB
    DB -- "Display Current Info" --> L4b[Enter New Values]
    L4b --> M3
    M3 -- "UPDATE account" --> DB

    L1 -- 4 --> L5[Search Account]
    L5 --> L5a[Enter Account Number]
    L5a -- "SELECT account info" --> DB
    DB -- "Display Account Info" --> L

    L1 -- 6 --> LOGOUT2[Logout]
    LOGOUT2 --> C
```

---

## 3. Class Diagram

```mermaid
classDiagram
    class Account {
        -const int accountNumber
        -const string login
        -const string pin
        -const string holderName
        -const double balance
        -const string status
        -const bool admin
        +Account(int, string, string, string, double, string, bool)
        +getAccountNumber() int
        +getLogin() string
        +getPin() string
        +getHolderName() string
        +getBalance() double
        +getStatus() string
        +isAdmin() bool
    }

    class ILoginDal {
        <<interface>>
        +findByCredentials(string, string) Account*
    }

    class ICustomerOptionsDal {
        <<interface>>
        +updateBalance(int, double) bool
        +findByNumber(int) Account*
    }

    class IAdminOptionsDal {
        <<interface>>
        +insertAccount(string, string, string, double, string) int
        +deleteAccount(int) bool
        +updateField(int, string, string) bool
        +findCustomerByNumber(int) Account*
    }

    class LoginDal {
        -Connection* con
        +findByCredentials(string, string) Account*
    }

    class CustomerOptionsDal {
        -Connection* con
        +updateBalance(int, double) bool
        +findByNumber(int) Account*
    }

    class AdminOptionsDal {
        -Connection* con
        +insertAccount(string, string, string, double, string) int
        +deleteAccount(int) bool
        +updateField(int, string, string) bool
        +findCustomerByNumber(int) Account*
    }

    class Login {
        -ILoginDal* dal
        +Login(ILoginDal*)
        +authenticate(string, string) Account*
    }

    class CustomerOptions {
        -ICustomerOptionsDal* dal
        +CustomerOptions(ICustomerOptionsDal*)
        +withdraw(Account, double) TransactionResult
        +deposit(Account, double) TransactionResult
        +getBalance(Account) double
    }

    class AdminOptions {
        -IAdminOptionsDal* dal
        +AdminOptions(IAdminOptionsDal*)
        +createAccount(string, string, string, double, string) AdminResult
        +deleteAccount(int) AdminResult
        +getHolderName(int) string
        +updateAccountField(int, string, string) AdminResult
        +searchAccount(int) Account*
    }

    class ConsoleUI {
        -Login* loginService
        -CustomerOptions* customerService
        -AdminOptions* adminService
        +ConsoleUI(Login*, CustomerOptions*, AdminOptions*)
        +run() void
    }

    ILoginDal <|.. LoginDal
    ICustomerOptionsDal <|.. CustomerOptionsDal
    IAdminOptionsDal <|.. AdminOptionsDal
    Login --> ILoginDal
    CustomerOptions --> ICustomerOptionsDal
    AdminOptions --> IAdminOptionsDal
    ConsoleUI --> Login
    ConsoleUI --> CustomerOptions
    ConsoleUI --> AdminOptions
    LoginDal ..> Account
    CustomerOptionsDal ..> Account
    AdminOptionsDal ..> Account
```

---

## 4. Component Diagram (Layered Architecture)

```mermaid
graph TB
    subgraph UILayer["Presentation Layer (ui/)"]
        CONSOLEUI["ConsoleUI.h<br/>All console I/O, menus"]
    end

    subgraph ModelLayer["Business Logic Layer (model/)"]
        ACC["Account.h<br/>Immutable data class, no setters"]
        LOGIN_M["Login.h<br/>Authentication logic"]
        CUST_M["CustomerOptions.h<br/>Withdraw, Deposit, Balance"]
        ADMIN_M["AdminOptions.h<br/>Create, Delete, Update, Search"]
    end

    subgraph DALLayer["Data Access Layer (DAL/)"]
        LOGIN_D["LoginDal.h/.cpp<br/>SQL: SELECT credentials"]
        CUST_D["CustomerOptionsDal.h/.cpp<br/>SQL: UPDATE balance"]
        ADMIN_D["AdminOptionsDal.h/.cpp<br/>SQL: INSERT, DELETE, UPDATE, SELECT"]
    end

    subgraph DataStore["MySQL Database"]
        DB[(accounts table)]
    end

    CONSOLEUI --> LOGIN_M
    CONSOLEUI --> CUST_M
    CONSOLEUI --> ADMIN_M
    LOGIN_M --> LOGIN_D
    CUST_M --> CUST_D
    ADMIN_M --> ADMIN_D
    LOGIN_D --> DB
    CUST_D --> DB
    ADMIN_D --> DB
    LOGIN_M --> ACC
    CUST_M --> ACC
    ADMIN_M --> ACC
```

### Component Responsibilities

| Layer | Component | File | Responsibility |
|-------|-----------|------|----------------|
| UI | ConsoleUI | `src/ui/ConsoleUI.h` | All std::cin/std::cout, menus, user input |
| Model | Account | `src/model/Account.h` | Immutable data class (constructor, const members, no setters) |
| Model | Login | `src/model/Login.h` | Authentication business logic |
| Model | CustomerOptions | `src/model/CustomerOptions.h` | Withdraw/deposit validation and logic |
| Model | AdminOptions | `src/model/AdminOptions.h` | Account CRUD validation and logic |
| DAL | LoginDal | `src/DAL/LoginDal.h/.cpp` | SQL only: credential queries |
| DAL | CustomerOptionsDal | `src/DAL/CustomerOptionsDal.h/.cpp` | SQL only: balance updates |
| DAL | AdminOptionsDal | `src/DAL/AdminOptionsDal.h/.cpp` | SQL only: account CRUD queries |
| Entry | main.cpp | `src/main.cpp` | Wires DAL -> model -> UI together |

---

## 4. Deployment Diagram

```mermaid
graph TB
    subgraph "Docker Environment"
        subgraph "App Container (devcontainer)"
            APP["C++ ATM Application<br/>(atm_app binary)"]
            LIBS["libmysqlcppconn<br/>MySQL C++ Connector"]
            GCC["g++ Compiler<br/>Build Tools"]
        end

        subgraph "DB Container (MidTermProject-db)"
            MYSQL["MySQL 8.0 Server"]
            DATA["db-data Volume<br/>(Persistent Storage)"]
        end

        NET["Docker Network<br/>(devcontainer_default)"]
    end

    DEV["Developer Machine<br/>(VS Code + Dev Containers Extension)"]
    SQL_EXT["VS Code SQL Extension<br/>(127.0.0.1:3307)"]

    DEV --> APP
    SQL_EXT --> MYSQL
    APP -- "tcp://db:3306" --> MYSQL
    MYSQL --> DATA
    APP --- NET
    MYSQL --- NET
```

### Deployment Details

| Component | Container | Port | Notes |
|-----------|-----------|------|-------|
| C++ App | `devcontainer-app` | N/A | Runs inside dev container, connects to DB via service name `db` |
| MySQL 8.0 | `MidTermProject-db` | 3306 (internal), 3307 (host) | Persistent data via `db-data` Docker volume |
| Docker Network | `devcontainer_default` | — | Enables container-to-container communication via DNS |

