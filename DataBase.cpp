#include "architecture.h"
#include <pqxx/pqxx>
#include <iostream>

using namespace std;

Database::Database(const string &connString) : conn(connString) {
}

void Database::execute(const vector<string>& sqlStatements) {
    try {
        if (conn.is_open()) {
            cout << "Opened Database successfully: " << conn.dbname() << endl;
        } else {
            cout << "Can't open Database" << endl;
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

Database::~Database() {
    conn.close();
}

void Database::search(const string& date) {

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
