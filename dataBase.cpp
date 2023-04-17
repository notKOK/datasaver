#include "architecture.h"
#include <pqxx/pqxx>
#include <iostream>

using namespace std;

database::database(const string &connString) :conn(connString) {
}

void database::execute(const vector<string>& sqlStatements) {
    try {
        if (conn.is_open()) {
            cout << "Opened database successfully: " << conn.dbname() << endl;
        } else {
            cout << "Can't open database" << endl;
            return;
        }
        pqxx::work txn(conn);

        for (const auto& sql : sqlStatements) {
            txn.exec(sql);
        }

        txn.commit();
        cout << "Transaction completed successfully" << endl;

    } catch (const exception& e) {
        cerr << "Transaction failed: " << e.what() << endl;
    }
}

database::~database() {
    conn.close();
}

void database::search(const string& date) {

    std::string query = "SELECT series_of_holograms.Path_Rgg\n"
                        "FROM series_of_holograms\n"
                        "JOIN context \n"
                        " ON context.KeyContext = series_of_holograms.Key1FileName\n"
                        "WHERE "+ date +" >= context.ContextBeginDate \n"
                        "  AND " + date + " <= context.ContextEndDate;";

    // выполнение запроса
    pqxx::work txn(conn);

    pqxx::result result = txn.exec(query);

    // вывод результатов запроса
    for (auto row : result) {
        std::cout << row["Path_Rgg"].as<std::string>() << std::endl;
    }
}
