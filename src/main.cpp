#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>

#include "DAL/AdminOptionsDal.cpp"
#include "DAL/CustomerOptionsDal.cpp"
#include "DAL/LoginDal.cpp"
#include "model/AdminOptions.h"
#include "model/CustomerOptions.h"
#include "model/Login.h"
#include "ui/ConsoleUI.h"

/**
 * @brief Application entry point.
 *
 * Wires together the three layers:
 * DAL (data access) -> model (business logic) -> UI (console).
 */
int main() {
    try {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        sql::Connection *con = driver->connect("tcp://db:3306", "root", "p");
        con->setSchema("ATM");

        // DAL layer — SQL only
        LoginDal loginDal(con);
        CustomerOptionsDal customerDal(con);
        AdminOptionsDal adminDal(con);

        // Model layer — business logic, injected with DAL
        Login loginService(&loginDal);
        CustomerOptions customerService(&customerDal);
        AdminOptions adminService(&adminDal);

        // UI layer — console I/O, injected with model services
        ConsoleUI ui(&loginService, &customerService, &adminService);
        ui.run();

        delete con;
    } catch (sql::SQLException &e) {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        std::cerr << "Error Code: " << e.getErrorCode() << std::endl;
        return 1;
    }

    return 0;
}
