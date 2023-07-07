#ifndef DATABASE_H
#define DATABASE_H

#include "Board.h"
#include "User.h"
#include "Task.h"
#include <sqlite3.h>
#include <string>
#include <list>
#include <map>
#include <variant>

using namespace std;

class Board;
class User;
class Task;

class Database {
public:
    Database(string dbName);
    ~Database();
    void createTables();
    void deleteTables();
    int executeQuery(const string& sql, const map<string, variant<int, string>>& dataMap);
    string queryString(const string& tableName, const map<string, variant<int, string>>& dataMap);
    // void executeSQL(const string& sql, const list<variant<int, string, bool, optional<int>>>& params);
    void saveBoardData(Board& board);
    void saveUserData(User& user);
    void saveTaskData(Task& task);
    list<Board*> loadBoardData();
    list<User*> loadUserData();
    list<Task*> loadTaskData(list<Board*> boards, list<User*> users);
    void deleteBoard(Board& board);
    void deleteUser(User& user, User& replacementUser);
    void deleteTask(Task& task);

private:
    string dbName;
    sqlite3* db;
};

#endif // DATABASE_H
