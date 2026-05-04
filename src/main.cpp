#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <cstdlib>
#include <iostream>
#include <string>

#include "DAL/AdminOptionsDal.cpp"
#include "DAL/CustomerOptionsDal.cpp"
#include "DAL/LoginDal.cpp"
#include "model/AdminOptions.h"
#include "model/CustomerOptions.h"
#include "model/Login.h"
#include "ui/ConsoleUI.h"

namespace {

const char *env_or(const char *name, const char *fallback) {
    const char *v = std::getenv(name);
    return (v && v[0]) ? v : fallback;
}

/** tcp://host:port — defaults match docker-compose (service name `db` inside the dev network). */
std::string mysql_tcp_url() {
    const char *host = env_or("DB_HOST", "db");
    const char *port = env_or("DB_PORT", "3306");
    return std::string("tcp://") + host + ":" + port;
}

}  // namespace

/**
 * @brief Application entry point.
 *
 * Wires together the three layers:
 * DAL (data access) -> model (business logic) -> UI (console).
 *
 * Connection uses DB_HOST, DB_PORT, DB_USER, DB_PASS, DB_NAME from the environment when set
 * (see .devcontainer/docker-compose.yml). From the host machine you can run, for example:
 * DB_HOST=127.0.0.1 DB_PORT=3307 ./build/debug/atm_app
 */
int main() {
    try {
        sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
        const std::string url = mysql_tcp_url();
        const char *user = env_or("DB_USER", "root");
        const char *pass = env_or("DB_PASS", "p");
        const char *schema = env_or("DB_NAME", "ATM");
        sql::Connection *con = driver->connect(url.c_str(), user, pass);
        con->setSchema(schema);

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
