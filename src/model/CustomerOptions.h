#ifndef CUSTOMER_OPTIONS_H
#define CUSTOMER_OPTIONS_H

#include <memory>
#include <string>

#include "../DAL/CustomerOptionsDal.h"
#include "Account.h"

/**
 * @brief Result of a customer transaction (withdraw or deposit).
 *
 * Immutable — constructed once and read via public const members.
 */
struct TransactionResult {
    const bool success;
    const std::string message;
    const double newBalance;

    TransactionResult(bool success, const std::string &message, double newBalance)
        : success(success), message(message), newBalance(newBalance) {}
};

/**
 * @brief Business logic for customer operations.
 *
 * Handles validation (positive amounts, sufficient funds).
 * Delegates database operations to ICustomerOptionsDal.
 * Contains no SQL or I/O.
 */
class CustomerOptions {
   private:
    ICustomerOptionsDal *dal;

   public:
    /**
     * @brief Construct CustomerOptions with a DAL dependency.
     * @param dal Pointer to customer data access object (injected).
     */
    explicit CustomerOptions(ICustomerOptionsDal *dal) : dal(dal) {}

    /**
     * @brief Withdraw cash from an account.
     * @param account The account to withdraw from.
     * @param amount The amount to withdraw.
     * @return TransactionResult with success status, message, and new balance.
     */
    TransactionResult withdraw(const Account &account, double amount) {
        if (amount <= 0) {
            return TransactionResult(false, "Invalid amount.", account.getBalance());
        }
        if (amount > account.getBalance()) {
            return TransactionResult(false, "Insufficient funds.", account.getBalance());
        }

        double newBalance = account.getBalance() - amount;
        bool ok = dal->updateBalance(account.getAccountNumber(), newBalance);
        if (!ok) {
            return TransactionResult(false, "Database update failed.", account.getBalance());
        }
        return TransactionResult(true, "Cash Successfully Withdrawn", newBalance);
    }

    /**
     * @brief Deposit cash into an account.
     * @param account The account to deposit into.
     * @param amount The amount to deposit.
     * @return TransactionResult with success status, message, and new balance.
     */
    TransactionResult deposit(const Account &account, double amount) {
        if (amount <= 0) {
            return TransactionResult(false, "Invalid amount.", account.getBalance());
        }

        double newBalance = account.getBalance() + amount;
        bool ok = dal->updateBalance(account.getAccountNumber(), newBalance);
        if (!ok) {
            return TransactionResult(false, "Database update failed.", account.getBalance());
        }
        return TransactionResult(true, "Cash Deposited Successfully.", newBalance);
    }

    /**
     * @brief Get the current balance for an account.
     * @param account The account to query.
     * @return The current balance.
     */
    double getBalance(const Account &account) { return account.getBalance(); }
};

#endif
