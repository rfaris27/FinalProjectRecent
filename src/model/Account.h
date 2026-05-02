#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

/**
 * @brief Immutable Account model class.
 *
 * All fields are set via the constructor and cannot be changed afterwards.
 * To "update" an account (e.g. change balance), create a new Account object.
 * This enforces true encapsulation — data is hidden and immutable.
 */
class Account {
   private:
    const int accountNumber;
    const std::string login;
    const std::string pin;
    const std::string holderName;
    const double balance;
    const std::string status;
    const bool admin;

   public:
    /**
     * @brief Construct a new Account object with all fields.
     */
    Account(int accountNumber, const std::string &login, const std::string &pin,
            const std::string &holderName, double balance, const std::string &status, bool admin)
        : accountNumber(accountNumber),
          login(login),
          pin(pin),
          holderName(holderName),
          balance(balance),
          status(status),
          admin(admin) {}

    /** @brief Get the account number. */
    int getAccountNumber() const { return accountNumber; }
    /** @brief Get the login username. */
    std::string getLogin() const { return login; }
    /** @brief Get the pin code. */
    std::string getPin() const { return pin; }
    /** @brief Get the account holder's name. */
    std::string getHolderName() const { return holderName; }
    /** @brief Get the current balance. */
    double getBalance() const { return balance; }
    /** @brief Get the account status (Active/Inactive). */
    std::string getStatus() const { return status; }
    /** @brief Check if this account has admin privileges. */
    bool isAdmin() const { return admin; }
};

#endif
