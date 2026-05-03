#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "../model/Account.h"
#include "../model/AdminOptions.h"
#include "../model/CustomerOptions.h"
#include "../model/Login.h"

/**
 * @brief Handles all console input/output for the ATM application.
 *
 * Contains no SQL or business logic. Delegates to model layer services.
 */
class ConsoleUI {
   private:
    Login *loginService;
    CustomerOptions *customerService;
    AdminOptions *adminService;

    /**
     * @brief Get a validated integer from the user.
     */
    int getValidInt(const std::string &prompt) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value)
                return value;
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    /**
     * @brief Get a validated double from the user.
     */
    double getValidDouble(const std::string &prompt) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value)
                return value;
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    /**
     * @brief Get the current date formatted as MM/DD/YYYY.
     */
    std::string getCurrentDate() {
        time_t now = time(0);
        struct tm *t = localtime(&now);
        char buf[11];
        strftime(buf, sizeof(buf), "%m/%d/%Y", t);
        return std::string(buf);
    }

    /**
     * @brief Display the customer options menu and handle choices.
     */
    void showCustomerMenu(const Account &initialAccount) {
        auto currentAccount = std::make_unique<Account>(
            initialAccount.getAccountNumber(), initialAccount.getLogin(), initialAccount.getPin(),
            initialAccount.getHolderName(), initialAccount.getBalance(), initialAccount.getStatus(),
            initialAccount.isAdmin());

        while (true) {
            std::cout << "\n--- Customer Menu ---" << std::endl;
            std::cout << "1----Withdraw Cash" << std::endl;
            std::cout << "3----Deposit Cash" << std::endl;
            std::cout << "4----Display Balance" << std::endl;
            std::cout << "5----Exit" << std::endl;
            int choice = getValidInt("Enter your choice: ");

            switch (choice) {
                case 1: {
                    double amount = getValidDouble("Enter the withdrawal amount: ");
                    TransactionResult result = customerService->withdraw(*currentAccount, amount);
                    std::cout << result.message << std::endl;
                    if (result.success) {
                        std::cout << "Account #" << currentAccount->getAccountNumber() << std::endl;
                        std::cout << "Date: " << getCurrentDate() << std::endl;
                        std::cout << std::fixed << std::setprecision(2);
                        std::cout << "Withdrawn: " << amount << std::endl;
                        std::cout << "Balance: " << result.newBalance << std::endl;
                        currentAccount = std::make_unique<Account>(
                            currentAccount->getAccountNumber(), currentAccount->getLogin(),
                            currentAccount->getPin(), currentAccount->getHolderName(),
                            result.newBalance, currentAccount->getStatus(),
                            currentAccount->isAdmin());
                    }
                    break;
                }
                case 3: {
                    double amount = getValidDouble("Enter the cash amount to deposit: ");
                    TransactionResult result = customerService->deposit(*currentAccount, amount);
                    std::cout << result.message << std::endl;
                    if (result.success) {
                        std::cout << "Account #" << currentAccount->getAccountNumber() << std::endl;
                        std::cout << "Date: " << getCurrentDate() << std::endl;
                        std::cout << std::fixed << std::setprecision(2);
                        std::cout << "Deposited: " << amount << std::endl;
                        std::cout << "Balance: " << result.newBalance << std::endl;
                        currentAccount = std::make_unique<Account>(
                            currentAccount->getAccountNumber(), currentAccount->getLogin(),
                            currentAccount->getPin(), currentAccount->getHolderName(),
                            result.newBalance, currentAccount->getStatus(),
                            currentAccount->isAdmin());
                    }
                    break;
                }
                case 4: {
                    std::cout << "Account #" << currentAccount->getAccountNumber() << std::endl;
                    std::cout << "Date: " << getCurrentDate() << std::endl;
                    std::cout << std::fixed << std::setprecision(2);
                    std::cout << "Balance: " << currentAccount->getBalance() << std::endl;
                    break;
                }
                case 5:
                    std::cout << "Logged out." << std::endl;
                    return;
                default:
                    std::cout << "Invalid choice." << std::endl;
            }
        }
    }

    /**
     * @brief Display the admin options menu and handle choices.
     */
    void showAdminMenu(const Account &account) {
        while (true) {
            std::cout << "\n--- Admin Options ---" << std::endl;
            std::cout << "Welcome, " << account.getHolderName() << std::endl;
            std::cout << "1----Create New Account" << std::endl;
            std::cout << "2----Delete Existing Account" << std::endl;
            std::cout << "3----Update Account Information" << std::endl;
            std::cout << "4----Search for Account" << std::endl;
            std::cout << "6----Exit" << std::endl;
            int choice = getValidInt("Enter your choice: ");

            switch (choice) {
                case 1:
                    handleCreateAccount();
                    break;
                case 2:
                    handleDeleteAccount();
                    break;
                case 3:
                    handleUpdateAccount();
                    break;
                case 4:
                    handleSearchAccount();
                    break;
                case 6:
                    std::cout << "Logged out." << std::endl;
                    return;
                default:
                    std::cout << "Invalid choice." << std::endl;
            }
        }
    }

    void handleCreateAccount() {
        std::string login, pin, name, status;
        double balance;

        std::cin.ignore();
        std::cout << "The following information must be entered for the new account holder"
                  << std::endl;
        std::cout << "Login: ";
        std::getline(std::cin, login);
        std::cout << "Pin Code: ";
        std::getline(std::cin, pin);
        std::cout << "Holders Name: ";
        std::getline(std::cin, name);
        std::cout << "Starting Balance: ";
        std::cin >> balance;
        std::cin.ignore();
        std::cout << "Status: ";
        std::getline(std::cin, status);

        AdminResult result = adminService->createAccount(login, pin, name, balance, status);
        std::cout << result.message << std::endl;
    }

    void handleDeleteAccount() {
        int accountNumber;
        std::cout << "Enter the account number to which you want to delete: ";
        std::cin >> accountNumber;

        std::string holderName = adminService->getHolderName(accountNumber);
        if (holderName.empty()) {
            std::cout << "Account not found." << std::endl;
            return;
        }

        std::cout << "You wish to delete the account held by " << holderName
                  << ". If this information is correct, please re-enter the account number: ";
        int confirmNumber;
        std::cin >> confirmNumber;

        if (confirmNumber != accountNumber) {
            std::cout << "Account numbers do not match. Delete cancelled." << std::endl;
            return;
        }

        AdminResult result = adminService->deleteAccount(accountNumber);
        std::cout << result.message << std::endl;
    }

    void handleUpdateAccount() {
        int accountNumber;
        std::cout << "Enter account number to update: ";
        std::cin >> accountNumber;

        auto account = adminService->searchAccount(accountNumber);
        if (!account) {
            std::cout << "Account not found or cannot update admin." << std::endl;
            return;
        }

        std::cout << "\nAccount # " << account->getAccountNumber() << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Holder: " << account->getHolderName() << std::endl;
        std::cout << "Balance: " << account->getBalance() << std::endl;
        std::cout << "Status: " << account->getStatus() << std::endl;
        std::cout << "Login: " << account->getLogin() << std::endl;
        std::cout << "Pin Code: " << account->getPin() << std::endl;

        std::cout << "\nEnter new values (press Enter to keep current):" << std::endl;
        std::string holder, pin, status, login, balanceStr;
        std::cin.ignore();

        std::cout << "Holder: ";
        std::getline(std::cin, holder);
        std::cout << "Balance: ";
        std::getline(std::cin, balanceStr);
        std::cout << "Status: ";
        std::getline(std::cin, status);
        std::cout << "Login: ";
        std::getline(std::cin, login);
        std::cout << "Pin Code: ";
        std::getline(std::cin, pin);

        if (!holder.empty()) {
            auto r = adminService->updateAccountField(accountNumber, "HolderName", holder);
            if (!r.success)
                std::cout << r.message << std::endl;
        }
        if (!balanceStr.empty()) {
            auto r = adminService->updateAccountField(accountNumber, "Balance", balanceStr);
            if (!r.success)
                std::cout << r.message << std::endl;
        }
        if (!status.empty()) {
            auto r = adminService->updateAccountField(accountNumber, "Status", status);
            if (!r.success)
                std::cout << r.message << std::endl;
        }
        if (!login.empty()) {
            auto r = adminService->updateAccountField(accountNumber, "Login", login);
            if (!r.success)
                std::cout << r.message << std::endl;
        }
        if (!pin.empty()) {
            auto r = adminService->updateAccountField(accountNumber, "Pin", pin);
            if (!r.success) {
                std::cout << r.message << std::endl;
                return;
            }
        }
        std::cout << "Account updated successfully." << std::endl;
    }

    void handleSearchAccount() {
        int accountNumber;
        std::cout << "Enter Account number: ";
        std::cin >> accountNumber;

        auto account = adminService->searchAccount(accountNumber);
        if (!account) {
            std::cout << "Account not found." << std::endl;
            return;
        }

        std::cout << "\nThe account information is:" << std::endl;
        std::cout << "Account # " << account->getAccountNumber() << std::endl;
        std::cout << "Holder: " << account->getHolderName() << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Balance: " << account->getBalance() << std::endl;
        std::cout << "Status: " << account->getStatus() << std::endl;
        std::cout << "Login: " << account->getLogin() << std::endl;
        std::cout << "Pin Code: " << account->getPin() << std::endl;
    }

   public:
    /**
     * @brief Construct ConsoleUI with injected service dependencies.
     * @param loginService Pointer to Login service.
     * @param customerService Pointer to CustomerOptions service.
     * @param adminService Pointer to AdminOptions service.
     */
    ConsoleUI(Login *loginService, CustomerOptions *customerService, AdminOptions *adminService)
        : loginService(loginService),
          customerService(customerService),
          adminService(adminService) {}

    /**
     * @brief Run the main application loop.
     *
     * Displays login screen, role selection, and routes to the appropriate menu.
     */
    void run() {
        std::cout << "--- ATM System ---" << std::endl;

        while (true) {
            std::string login, pin;
            std::cout << "Enter login: ";
            std::cin >> login;
            std::cout << "Enter Pin code: ";
            std::cin >> pin;

            auto account = loginService->authenticate(login, pin);

            if (account) {
                std::cout << "\nLogin as:" << std::endl;
                std::cout << "1----Customer" << std::endl;
                std::cout << "2----Administrator" << std::endl;
                int role = getValidInt("Enter your choice: ");

                if (role == 2 && account->isAdmin()) {
                    showAdminMenu(*account);
                } else if (role == 1) {
                    showCustomerMenu(*account);
                } else {
                    std::cout << "Error: Invalid choice or access denied." << std::endl;
                }
            } else {
                std::cout << "Error: Invalid login or pin." << std::endl;
            }
        }
    }
};

#endif
