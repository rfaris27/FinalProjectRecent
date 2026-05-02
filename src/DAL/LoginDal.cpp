#include "LoginDal.h"

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

/**
 * @brief MySQL implementation of ILoginDal.
 */
class LoginDal : public ILoginDal {
   private:
    sql::Connection *con;

   public:
    /**
     * @brief Construct LoginDal with a database connection.
     * @param con Pointer to an active MySQL connection.
     */
    explicit LoginDal(sql::Connection *con) : con(con) {}

    std::unique_ptr<Account> findByCredentials(const std::string &login,
                                               const std::string &pin) override {
        sql::PreparedStatement *pstmt =
            con->prepareStatement("SELECT * FROM accounts WHERE Login = ? AND Pin = ?");
        pstmt->setString(1, login);
        pstmt->setString(2, pin);

        sql::ResultSet *res = pstmt->executeQuery();

        std::unique_ptr<Account> account = nullptr;
        if (res->next()) {
            account = std::make_unique<Account>(
                res->getInt("AccountNumber"), res->getString("Login"), res->getString("Pin"),
                res->getString("HolderName"), res->getDouble("Balance"),
                res->getString("Status"), res->getBoolean("IsAdmin"));
        }

        delete res;
        delete pstmt;
        return account;
    }
};
