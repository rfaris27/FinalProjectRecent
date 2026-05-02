#ifndef LOGIN_DAL_H
#define LOGIN_DAL_H

#include <memory>
#include <string>

#include "../model/Account.h"

/**
 * @brief Abstract interface for login data access.
 *
 * Concrete implementations provide the actual database queries.
 * This enables dependency injection and testability.
 */
class ILoginDal {
   public:
    virtual ~ILoginDal() = default;

    /**
     * @brief Find an account by login and pin credentials.
     * @param login The login username.
     * @param pin The 5-digit pin code.
     * @return A unique_ptr to Account if found, nullptr otherwise.
     */
    virtual std::unique_ptr<Account> findByCredentials(const std::string &login,
                                                       const std::string &pin) = 0;
};

#endif
