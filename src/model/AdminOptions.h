#ifndef ADMIN_OPTIONS_H
#define ADMIN_OPTIONS_H

#include <cctype>
#include <memory>
#include <string>

#include "../DAL/AdminOptionsDal.h"
#include "Account.h"

/**
 * @brief Result of an admin operation.
 *
 * Immutable — constructed once and read via public const members.
 */
struct AdminResult {
    const bool success;
    const std::string message;
    const int accountNumber;

    AdminResult(bool success, const std::string &message, int accountNumber = -1)
        : success(success), message(message), accountNumber(accountNumber) {}
};

/**
 * @brief Business logic for admin account management.
 *
 * Handles validation (pin format, duplicate detection).
 * Delegates database operations to IAdminOptionsDal.
 * Contains no SQL or I/O.
 */
class AdminOptions {
   private:
    IAdminOptionsDal *dal;

    /**
     * @brief Validate that a pin is exactly 5 digits.
     */
    bool isValidPin(const std::string &pin) const {
        if (pin.length() != 5)
            return false;
        for (char c : pin) {
            if (!std::isdigit(c))
                return false;
        }
        return true;
    }

   public:
    /**
     * @brief Construct AdminOptions with a DAL dependency.
     * @param dal Pointer to admin data access object (injected).
     */
    explicit AdminOptions(IAdminOptionsDal *dal) : dal(dal) {}

    /**
     * @brief Create a new customer account.
     * @return AdminResult with success status and assigned account number.
     */
    AdminResult createAccount(const std::string &login, const std::string &pin,
                              const std::string &holderName, double balance,
                              const std::string &status) {
        if (login.empty()) {
            return AdminResult(false, "Login cannot be empty.");
        }
        if (!isValidPin(pin)) {
            return AdminResult(false, "Error: Pin Code must be an integer of length 5.");
        }
        if (holderName.empty()) {
            return AdminResult(false, "Holder name cannot be empty.");
        }

        int newId = dal->insertAccount(login, pin, holderName, balance, status);
        if (newId < 0) {
            return AdminResult(false, "Error creating account.");
        }
        return AdminResult(true,
                           "Account Successfully Created – the account number assigned is: " +
                               std::to_string(newId),
                           newId);
    }

    /**
     * @brief Delete a customer account after looking it up.
     * @param accountNumber The account to delete.
     * @return AdminResult with success status.
     */
    AdminResult deleteAccount(int accountNumber) {
        auto account = dal->findCustomerByNumber(accountNumber);
        if (!account) {
            return AdminResult(false, "Account not found.");
        }
        bool ok = dal->deleteAccount(accountNumber);
        if (!ok) {
            return AdminResult(false, "Failed to delete account.");
        }
        return AdminResult(true, "Account Deleted Successfully", accountNumber);
    }

    /**
     * @brief Look up a customer account's holder name for confirmation.
     * @param accountNumber The account number.
     * @return The holder name, or empty string if not found.
     */
    std::string getHolderName(int accountNumber) {
        auto account = dal->findCustomerByNumber(accountNumber);
        if (!account)
            return "";
        return account->getHolderName();
    }

    /**
     * @brief Update a field on a customer account.
     * @param accountNumber The account to update.
     * @param field The field name (HolderName, Pin, Balance, Status, Login).
     * @param value The new value.
     * @return AdminResult with success status.
     */
    AdminResult updateAccountField(int accountNumber, const std::string &field,
                                   const std::string &value) {
        if (field == "Pin" && !isValidPin(value)) {
            return AdminResult(false,
                               "Error: Pin Code must be an integer of length 5. Update cancelled.");
        }

        auto account = dal->findCustomerByNumber(accountNumber);
        if (!account) {
            return AdminResult(false, "Account not found or cannot update admin.");
        }

        bool ok = dal->updateField(accountNumber, field, value);
        if (!ok) {
            return AdminResult(false, "Failed to update account.");
        }
        return AdminResult(true, "Account updated successfully.", accountNumber);
    }

    /**
     * @brief Search for a customer account by number.
     * @param accountNumber The account number to search.
     * @return A unique_ptr to Account if found, nullptr otherwise.
     */
    std::unique_ptr<Account> searchAccount(int accountNumber) {
        return dal->findCustomerByNumber(accountNumber);
    }
};

#endif
