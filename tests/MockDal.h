#ifndef MOCK_DAL_H
#define MOCK_DAL_H

#include <map>
#include <memory>
#include <string>

#include "../src/DAL/AdminOptionsDal.h"
#include "../src/DAL/CustomerOptionsDal.h"
#include "../src/DAL/LoginDal.h"
#include "../src/model/Account.h"

/**
 * @brief In-memory mock for ILoginDal. No MySQL needed.
 */
class MockLoginDal : public ILoginDal {
   private:
    std::map<std::string, std::unique_ptr<Account>> accounts;

   public:
    void addAccount(int num, const std::string &login, const std::string &pin,
                    const std::string &name, double balance, const std::string &status,
                    bool admin) {
        std::string key = login + ":" + pin;
        accounts[key] = std::make_unique<Account>(num, login, pin, name, balance, status, admin);
    }

    std::unique_ptr<Account> findByCredentials(const std::string &login,
                                               const std::string &pin) override {
        std::string key = login + ":" + pin;
        auto it = accounts.find(key);
        if (it == accounts.end())
            return nullptr;
        const Account &a = *(it->second);
        return std::make_unique<Account>(a.getAccountNumber(), a.getLogin(), a.getPin(),
                                         a.getHolderName(), a.getBalance(), a.getStatus(),
                                         a.isAdmin());
    }
};

/**
 * @brief In-memory mock for ICustomerOptionsDal. No MySQL needed.
 */
class MockCustomerOptionsDal : public ICustomerOptionsDal {
   private:
    std::map<int, std::unique_ptr<Account>> accounts;

   public:
    void addAccount(int num, const std::string &login, const std::string &pin,
                    const std::string &name, double balance, const std::string &status,
                    bool admin) {
        accounts[num] = std::make_unique<Account>(num, login, pin, name, balance, status, admin);
    }

    bool updateBalance(int accountNumber, double newBalance) override {
        auto it = accounts.find(accountNumber);
        if (it == accounts.end())
            return false;
        const Account &old = *(it->second);
        accounts[accountNumber] = std::make_unique<Account>(
            old.getAccountNumber(), old.getLogin(), old.getPin(), old.getHolderName(), newBalance,
            old.getStatus(), old.isAdmin());
        return true;
    }

    std::unique_ptr<Account> findByNumber(int accountNumber) override {
        auto it = accounts.find(accountNumber);
        if (it == accounts.end())
            return nullptr;
        const Account &a = *(it->second);
        return std::make_unique<Account>(a.getAccountNumber(), a.getLogin(), a.getPin(),
                                         a.getHolderName(), a.getBalance(), a.getStatus(),
                                         a.isAdmin());
    }
};

/**
 * @brief In-memory mock for IAdminOptionsDal. No MySQL needed.
 */
class MockAdminOptionsDal : public IAdminOptionsDal {
   private:
    std::map<int, std::unique_ptr<Account>> accounts;
    int nextId = 100;

   public:
    void addAccount(int num, const std::string &login, const std::string &pin,
                    const std::string &name, double balance, const std::string &status,
                    bool admin) {
        accounts[num] = std::make_unique<Account>(num, login, pin, name, balance, status, admin);
        if (num >= nextId)
            nextId = num + 1;
    }

    int insertAccount(const std::string &login, const std::string &pin,
                      const std::string &holderName, double balance,
                      const std::string &status) override {
        int id = nextId++;
        accounts[id] =
            std::make_unique<Account>(id, login, pin, holderName, balance, status, false);
        return id;
    }

    bool deleteAccount(int accountNumber) override {
        auto it = accounts.find(accountNumber);
        if (it == accounts.end())
            return false;
        if (it->second->isAdmin())
            return false;
        accounts.erase(it);
        return true;
    }

    bool updateField(int accountNumber, const std::string &field,
                     const std::string &value) override {
        auto it = accounts.find(accountNumber);
        if (it == accounts.end())
            return false;
        const Account &old = *(it->second);

        std::string newLogin = old.getLogin();
        std::string newPin = old.getPin();
        std::string newName = old.getHolderName();
        double newBalance = old.getBalance();
        std::string newStatus = old.getStatus();

        if (field == "Login")
            newLogin = value;
        else if (field == "Pin")
            newPin = value;
        else if (field == "HolderName")
            newName = value;
        else if (field == "Balance")
            newBalance = std::stod(value);
        else if (field == "Status")
            newStatus = value;

        accounts[accountNumber] = std::make_unique<Account>(
            accountNumber, newLogin, newPin, newName, newBalance, newStatus, old.isAdmin());
        return true;
    }

    std::unique_ptr<Account> findCustomerByNumber(int accountNumber) override {
        auto it = accounts.find(accountNumber);
        if (it == accounts.end())
            return nullptr;
        if (it->second->isAdmin())
            return nullptr;
        const Account &a = *(it->second);
        return std::make_unique<Account>(a.getAccountNumber(), a.getLogin(), a.getPin(),
                                         a.getHolderName(), a.getBalance(), a.getStatus(),
                                         a.isAdmin());
    }
};

#endif
