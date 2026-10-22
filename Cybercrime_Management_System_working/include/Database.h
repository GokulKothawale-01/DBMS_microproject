#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

#if __has_include(<mysql.h>)
#include <mysql.h>
#define MYSQL_AVAILABLE 1
#else
#define MYSQL_AVAILABLE 0
#endif

class Database {
public:
    Database();
    ~Database();

    bool connect(
        const std::string& host = "localhost",
        const std::string& user = "root",
        const std::string& password = "",
        const std::string& dbName = "cybercrime_db"
    );

    void disconnect();
    bool isConnected() const;

    bool createCase(
        const std::string& caseID,
        const std::string& type,
        const std::string& severity,
        const std::string& status,
        const std::string& description,
        const std::string& extraData = ""
    );

    bool addEvidence(
        const std::string& evidenceID,
        const std::string& caseID,
        const std::string& type,
        const std::string& custodyLog
    );

    bool addInvestigator(
        const std::string& investigatorID,
        const std::string& name,
        const std::string& contact
    );

    std::vector<std::vector<std::string>> query(const std::string& sql);

private:
#if MYSQL_AVAILABLE
    MYSQL* connection;
#else
    void* connection;
#endif
    bool connected;

    bool execute(const std::string& sql);
};

#endif
