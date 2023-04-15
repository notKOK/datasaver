#include "architecture.h"
#include <pqxx/pqxx>
#include <iostream>


database::database(const std::string &connString) :conn(connString) {
}

void database::execute(const std::vector<std::string>& sqlStatements) {
    try {
        if (conn.is_open()) {
            std::cout << "Opened database successfully: " << conn.dbname() << std::endl;
        } else {
            std::cout << "Can't open database" << std::endl;
            return;
        }
        pqxx::work txn(conn);

        for (const auto& sql : sqlStatements) {
            txn.exec(sql);
        }

        txn.commit();
        std::cout << "Transaction completed successfully" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Transaction failed: " << e.what() << std::endl;
    }
}

database::~database() {
    conn.close();
}