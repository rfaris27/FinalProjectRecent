#include "CustomerOptionsDal.h"

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

/**
 * @brief MySQL implementation of ICustomerOptionsDal.
 */
class CustomerOptionsDal : public ICustomerOptionsDal {
   private:
    sql::Connection *con;

   public:
    explicit CustomerOptionsDal(sql::Connection *con) : con(con) {}

    bool updateBalance(int accountNumber, double newBalance) override {
        sql::PreparedStatement *pstmt =
            con->prepareStatement("UPDATE accounts SET Balance = ? WHERE AccountNumber = ?");
        pstmt->setDouble(1, newBalance);
        pstmt->setInt(2, accountNumber);
        int rows = pstmt->executeUpdate();
        delete pstmt;
        return rows > 0;
    }

    std::unique_ptr<Account> findByNumber(int accountNumber) override {
        sql::PreparedStatement *pstmt =
            con->prepareStatement("SELECT * FROM accounts WHERE AccountNumber = ?");
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
