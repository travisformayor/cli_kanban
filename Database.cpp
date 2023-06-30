#include "Database.h"
#include <stdexcept>
#include <sstream>
#include <map>

using namespace std;

// Constructor
Database::Database(string dbName) : dbName(dbName) {
    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc) {
        sqlite3_close(db);
        throw runtime_error(sqlite3_errmsg(db));
    }
}

// Destructor
Database::~Database() {
    sqlite3_close(db);
}

// Query to access records in the db
void Database::query(const string& sql) {
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &zErrMsg);
    if (rc != SQLITE_OK) {
        string errorMsg = string("SQL error: ") + zErrMsg;
        sqlite3_free(zErrMsg);
        throw runtime_error(errorMsg);
    }
}

int Database::callback(void* notUsed, int argc, char** argv, char** azColName) {
    // This callback function will be used to process the result of sqlite3_exec
    // In this minimalist implementation, we're not actually using the results, but you'd typically
    // process the results of your query here.
    return 0;
}

// Methods to save data
void Database::saveBoardData(Board* board) {
    stringstream sql;
    sql << "INSERT OR REPLACE INTO Boards VALUES("
        << board->getId() << ", "
        << "'" << board->getName() << "', "
        << (board->isActive() ? 1 : 0) << ", "
        << board->getOwner()->getId() << ");";
    query(sql.str());
    // Save tasks related to this board
    for (Task* task : board->getTasks()) {
        saveTaskData(task);
    }
}

void Database::saveTaskData(Task* task) {
    stringstream sql;
    sql << "INSERT OR REPLACE INTO Tasks VALUES("
        << task->getId() << ", "
        << "'" << task->getTitle() << "', "
        << "'" << task->getDescription() << "', "
        << task->getDifficultyScore() << ", "
        << (task->isActive() ? 1 : 0) << ", "
        << "'" << task->getDueDate() << "', " 
        << "'" << task->getStage() << "', "
        << task->getAssignedUser()->getId() << ");";
    query(sql.str());
}

void Database::saveUserData(User* user) {
    stringstream sql;
    sql << "INSERT OR REPLACE INTO Users VALUES("
        << user->getId() << ", "
        << "'" << user->getName() << "', "
        << (user->isActive() ? 1 : 0) << ", "
        << user->getCurrentBoard()->getId() << ");";
    query(sql.str());
}

// Methods to load data
list<Board*> Database::loadBoardData() {
    list<Board*> boards;
    string sql = "SELECT * FROM Boards;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Failed to prepare statement: " + string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        bool active = sqlite3_column_int(stmt, 2);
        int ownerId = sqlite3_column_int(stmt, 3);

        Board* board = new Board(id, name, tempUsers[ownerId]);
        board->setActive(active);
        tempBoards[id] = board;
        boards.push_back(board);
    }

    sqlite3_finalize(stmt);
    return boards;
}

list<Task*> Database::loadTaskData() {
    list<Task*> tasks;
    string sql = "SELECT * FROM Tasks;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Failed to prepare statement: " + string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int difficultyScore = sqlite3_column_int(stmt, 3);
        bool active = sqlite3_column_int(stmt, 4);
        string dueDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        string stageStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        int assignedUserId = sqlite3_column_int(stmt, 7);
        
        Task* task = new Task(id, title);
        task->setDescription(description);
        task->setDifficultyScore(difficultyScore);
        task->setActive(active);
        task->setDueDate(dueDate);
        task->setStage(stageStr);
        task->setAssignedUser(tempUsers[assignedUserId]);
        
        tempTasks[id] = task;
        tasks.push_back(task);
    }

    sqlite3_finalize(stmt);
    return tasks;
}

list<User*> Database::loadUserData() {
    list<User*> users;
    string sql = "SELECT * FROM Users;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Failed to prepare statement: " + string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        bool active = sqlite3_column_int(stmt, 2);
        int currentBoardId = sqlite3_column_int(stmt, 3);

        User* user = new User(id, name);
        user->setActive(active);
        user->setCurrentBoard(tempBoards[currentBoardId]);

        tempUsers[id] = user;
        users.push_back(user);
    }

    sqlite3_finalize(stmt);
    return users;
}
