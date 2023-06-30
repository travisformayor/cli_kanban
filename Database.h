#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <map>
#include <sqlite3.h> 
#include "Board.h"
#include "Task.h"
#include "User.h"

using namespace std;

class Database {
public:
    Database(string dbName);
    ~Database();
    void query(const string& sql);
    void deleteTables();
    void initDb();
    void saveBoardData(Board* board);
    void saveTaskData(Task* task);
    void saveUserData(User* user);
    list<Board*> loadBoardData();
    list<Task*> loadTaskData();
    list<User*> loadUserData();
    void deleteTask(Task& task);
    void deleteBoard(Board& board);
    void deleteUser(User& user, User& replacementUser);

private:
    string dbName;
    sqlite3* db;

    map<int, User*> tempUsers;
    map<int, Board*> tempBoards;
    map<int, Task*> tempTasks;

    static int callback(void* notUsed, int argc, char** argv, char** azColName);
};

#endif // DATABASE_H
