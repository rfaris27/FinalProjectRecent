#ifndef LOGIN_H
#define LOGIN_H

#include <memory>
#include <string>

#include "../DAL/LoginDal.h"
#include "Account.h"

/**
 * @brief Business logic for user authentication.
 *
 * Delegates database queries to ILoginDal. Contains no SQL or I/O.
 */
class Login {
   private:
    ILoginDal *dal;

   public:
    /**
     * @brief Construct Login with a DAL dependency.
     * @param dal Pointer to a login data access object (injected).
     */
    explicit Login(ILoginDal *dal) : dal(dal) {}

    /**
     * @brief Authenticate a user by login and pin.
     * @param login The login username.
     * @param pin The 5-digit pin code.
     * @return A unique_ptr to Account if credentials are valid, nullptr otherwise.
     */
    std::unique_ptr<Account> authenticate(const std::string &login, const std::string &pin) {
        if (login.empty() || pin.empty()) {
            return nullptr;
        }
        return dal->findByCredentials(login, pin);
    }
};

#endif
