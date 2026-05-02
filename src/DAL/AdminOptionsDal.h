#ifndef ADMIN_OPTIONS_DAL_H
#define ADMIN_OPTIONS_DAL_H

#include <memory>
#include <string>

#include "../model/Account.h"

/**
 * @brief Abstract interface for admin data access operations.
 *
 * Contains only SQL-related operations — no business logic, no I/O.
 */
class IAdminOptionsDal {
   public:
    virtual ~IAdminOptionsDal() = default;

    /**
     * @brief Insert a new customer account into the database.
     * @return The auto-generated account number, or -1 on failure.
     */
    virtual int insertAccount(const std::string &login, const std::string &pin,
                              const std::string &holderName, double balance,
                              const std::string &status) = 0;

    /**
     * @brief Delete a customer account by account number.
     * @return true if a row was deleted.
     */
    virtual bool deleteAccount(int accountNumber) = 0;

    /**
     * @brief Update a single field on an account.
     * @param accountNumber The account to update.
     * @param field The column name (HolderName, Pin, Balance, Status, Login).
     * @param value The new value as a string.
     * @return true if the update succeeded.
     */
    virtual bool updateField(int accountNumber, const std::string &field,
                             const std::string &value) = 0;

    /**
     * @brief Find a non-admin account by its account number.
     * @return A unique_ptr to Account if found, nullptr otherwise.
     */
    virtual std::unique_ptr<Account> findCustomerByNumber(int accountNumber) = 0;
};

#endif
