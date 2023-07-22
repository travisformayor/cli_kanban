#ifndef DATABASE_H
#define DATABASE_H

#include "Board.h"
#include "Task.h"
#include <sqlite3.h>
#include <string>
#include <list>
#include <map>
#include <variant>

using namespace std;

class Board;
class Task;

class Database {
public:
    Database(string dbName);
    ~Database();
    void createTables();
    void deleteTables();
    int executeQuery(const string& sql, const map<string, variant<int, string>>& dataMap);
    string queryString(const string& tableName, const map<string, variant<int, string>>& dataMap);
    void saveBoardData(Board& board);
    void saveTaskData(Task& task);
    list<Board*> loadBoardData();
    list<Task*> loadTaskData(Board& board);
    void deleteBoard(Board& board);
    void deleteTask(Task& task);

private:
    string dbName;
    sqlite3* db;
};

#endif // DATABASE_H
