#include "AdminOptionsDal.h"

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

/**
 * @brief MySQL implementation of IAdminOptionsDal.
 */
class AdminOptionsDal : public IAdminOptionsDal {
   private:
    sql::Connection *con;

   public:
    explicit AdminOptionsDal(sql::Connection *con) : con(con) {}

    int insertAccount(const std::string &login, const std::string &pin,
                      const std::string &holderName, double balance,
                      const std::string &status) override {
        sql::PreparedStatement *pstmt = con->prepareStatement(
            "INSERT INTO accounts (Login, Pin, HolderName, Balance, Status, IsAdmin) "
            "VALUES (?, ?, ?, ?, ?, FALSE)");
        pstmt->setString(1, login);
        pstmt->setString(2, pin);
        pstmt->setString(3, holderName);
        pstmt->setDouble(4, balance);
        pstmt->setString(5, status);
        pstmt->executeUpdate();
        delete pstmt;

        sql::Statement *stmt = con->createStatement();
        sql::ResultSet *res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS id");
        res->next();
        int newId = res->getInt("id");
        delete res;
        delete stmt;
        return newId;
    }

    bool deleteAccount(int accountNumber) override {
        sql::PreparedStatement *pstmt = con->prepareStatement(
            "DELETE FROM accounts WHERE AccountNumber = ? AND IsAdmin = FALSE");
        pstmt->setInt(1, accountNumber);
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    }

    bool updateField(int accountNumber, const std::string &field,
                     const std::string &value) override {
        std::string query = "UPDATE accounts SET " + field + " = ? WHERE AccountNumber = ?";
        sql::PreparedStatement *pstmt = con->prepareStatement(query);
        pstmt->setString(1, value);
        pstmt->setInt(2, accountNumber);
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    }

    std::unique_ptr<Account> findCustomerByNumber(int accountNumber) override {
        sql::PreparedStatement *pstmt = con->prepareStatement(
            "SELECT * FROM accounts WHERE AccountNumber = ? AND IsAdmin = FALSE");
        pstmt->setInt(1, accountNumber);
        sql::ResultSet *res = pstmt->executeQuery();

        std::unique_ptr<Account> account = nullptr;
        if (res->next()) {
            account = std::make_unique<Account>(
                res->getInt("AccountNumber"), res->getString("Login"), res->getString("Pin"),
                res->getString("HolderName"), res->getDouble("Balance"), res->getString("Status"),
                res->getBoolean("IsAdmin"));
        }

        delete res;
        delete pstmt;
        return account;
    }
};
