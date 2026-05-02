#ifndef CUSTOMER_OPTIONS_DAL_H
#define CUSTOMER_OPTIONS_DAL_H

#include <memory>

#include "../model/Account.h"

/**
 * @brief Abstract interface for customer data access operations.
 *
 * Contains only SQL-related operations — no business logic.
 */
class ICustomerOptionsDal {
   public:
    virtual ~ICustomerOptionsDal() = default;

    /**
     * @brief Update the balance for an account.
     * @param accountNumber The account to update.
     * @param newBalance The new balance value.
     * @return true if the update succeeded.
     */
    virtual bool updateBalance(int accountNumber, double newBalance) = 0;

    /**
     * @brief Fetch an account by its account number.
     * @param accountNumber The account number to look up.
     * @return A unique_ptr to Account if found, nullptr otherwise.
     */
    virtual std::unique_ptr<Account> findByNumber(int accountNumber) = 0;
};

#endif
