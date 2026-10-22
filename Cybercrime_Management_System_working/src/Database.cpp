#include "Database.h"

#include <iostream>
#include <sstream>

Database::Database() : connection(nullptr), connected(false) {}

Database::~Database() {
    disconnect();
}

bool Database::connect(
    const std::string& host,
    const std::string& user,
    const std::string& password,
    const std::string& dbName
) {
#if MYSQL_AVAILABLE
    connection = mysql_init(nullptr);
    if (!connection) {
        std::cerr << "MySQL init failed.\n";
        return false;
    }

    if (mysql_real_connect(connection, host.c_str(), user.c_str(), password.c_str(), dbName.c_str(), 3306, nullptr, 0) == nullptr) {
        std::cerr << "MySQL connection failed: " << mysql_error(connection) << "\n";
        mysql_close(connection);
        connection = nullptr;
        connected = false;
        return false;
    }

    connected = true;
    return true;
#else
    (void)host;
    (void)user;
    (void)password;
    (void)dbName;
    connected = false;
    connection = nullptr;
    std::cerr << "MySQL support is not enabled in this build. Install the MySQL dev library and rebuild.\n";
    return false;
#endif
}

void Database::disconnect() {
#if MYSQL_AVAILABLE
    if (connection) {
        mysql_close(connection);
        connection = nullptr;
    }
#endif
    connected = false;
}

bool Database::isConnected() const {
    return connected;
}

bool Database::execute(const std::string& sql) {
#if MYSQL_AVAILABLE
    if (!connection) {
        return false;
    }

    if (mysql_query(connection, sql.c_str()) != 0) {
        std::cerr << "SQL error: " << mysql_error(connection) << "\n";
        return false;
    }

    return true;
#else
    (void)sql;
    return false;
#endif
}

bool Database::createCase(
    const std::string& caseID,
    const std::string& type,
    const std::string& severity,
    const std::string& status,
    const std::string& description,
    const std::string& extraData
) {
#if MYSQL_AVAILABLE
    std::ostringstream sql;
    sql << "INSERT INTO incidents (case_id, type, severity, status, description, extra_data) "
        << "VALUES ('" << caseID << "', '" << type << "', '" << severity << "', '" << status << "', '"
        << description << "', '" << extraData << "');";
    return execute(sql.str());
#else
    (void)caseID; (void)type; (void)severity; (void)status; (void)description; (void)extraData;
    return false;
#endif
}

bool Database::addEvidence(
    const std::string& evidenceID,
    const std::string& caseID,
    const std::string& type,
    const std::string& custodyLog
) {
#if MYSQL_AVAILABLE
    std::ostringstream sql;
    sql << "INSERT INTO evidence (evidence_id, case_id, type, custody_log) VALUES ('"
        << evidenceID << "', '" << caseID << "', '" << type << "', '" << custodyLog << "');";
    return execute(sql.str());
#else
    (void)evidenceID; (void)caseID; (void)type; (void)custodyLog;
    return false;
#endif
}

bool Database::addInvestigator(
    const std::string& investigatorID,
    const std::string& name,
    const std::string& contact
) {
#if MYSQL_AVAILABLE
    std::ostringstream sql;
    sql << "INSERT INTO investigators (investigator_id, name, contact) VALUES ('"
        << investigatorID << "', '" << name << "', '" << contact << "');";
    return execute(sql.str());
#else
    (void)investigatorID; (void)name; (void)contact;
    return false;
#endif
}

std::vector<std::vector<std::string>> Database::query(const std::string& sql) {
    std::vector<std::vector<std::string>> rows;
#if MYSQL_AVAILABLE
    if (!connection) {
        return rows;
    }

    if (mysql_query(connection, sql.c_str()) != 0) {
        std::cerr << "Query failed: " << mysql_error(connection) << "\n";
        return rows;
    }

    MYSQL_RES* result = mysql_store_result(connection);
    if (!result) {
        return rows;
    }

    MYSQL_ROW row;
    unsigned int numFields = mysql_num_fields(result);
    while ((row = mysql_fetch_row(result)) != nullptr) {
        std::vector<std::string> values;
        unsigned long* lengths = mysql_fetch_lengths(result);
        for (unsigned int i = 0; i < numFields; ++i) {
            values.emplace_back(row[i] ? std::string(row[i], lengths[i]) : std::string());
        }
        rows.push_back(values);
    }

    mysql_free_result(result);
#endif
    (void)sql;
    return rows;
}
