#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h> 
#include <string>
#include <list>
#include "Board.h"
#include "Task.h"
#include "User.h"

using namespace std;

class Database {
public:
    Database(string dbName);
    ~Database();
    void createTables();
    void deleteTables();
    void executeSQL(const string& sql, const list<variant<int, string, bool, optional<int>>>& params);
    void saveBoardData(Board& board);
    void saveTaskData(Task& task);
    void saveUserData(User& user);
    list<Board*> loadBoardData();
    list<User*> loadUserData();
    list<Task*> loadTaskData(list<Board*> boards, list<User*> users);
    void deleteTask(Task& task);
    void deleteBoard(Board& board);
    void deleteUser(User& user, User& replacementUser);

private:
    string dbName;
    sqlite3* db;
};

#endif // DATABASE_H
