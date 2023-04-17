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