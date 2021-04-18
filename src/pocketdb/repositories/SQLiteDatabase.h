#ifndef POCKETDB_SQLITEDATABASE_H
#define POCKETDB_SQLITEDATABASE_H

#include "sqlite3.h"
#include "logging.h"
#include <iostream>
#include <filesystem>

class SQLiteDatabase {
private:
    const std::string m_dir_path;

    const std::string m_file_path;

    void Cleanup() noexcept;
public:
    SQLiteDatabase() = delete;

    SQLiteDatabase(const std::filesystem::path& dir_path, const std::filesystem::path& file_path);

    void Open();
    void Close();

    bool BeginTransaction();
    bool CommitTransaction();
    bool AbortTransaction();

    sqlite3* m_db{nullptr};
};


#endif // POCKETDB_SQLITEDATABASE_H