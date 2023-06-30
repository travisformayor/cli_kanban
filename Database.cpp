#include "Database.h"
#include "Task.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <map>
#include <chrono>
#include <ctime>

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

// Clear the DB
void Database::deleteTables() {
    try {
        query("DROP TABLE IF EXISTS Boards;");
        query("DROP TABLE IF EXISTS Users;");
        query("DROP TABLE IF EXISTS Tasks;");
    } catch (const runtime_error& e) {
        cerr << "Caught exception: " << e.what() << '\n';
        // Handle the error appropriately...
    }
}

// Setup the DB Tables
void Database::initDb() {
    string sql;

    // Open the SQLite database
    int result = sqlite3_open("kanban_database.db", &db);
    if (result) {
        throw runtime_error(string("Can't open database: ") + sqlite3_errmsg(db));
    } else {
        cout << "Opened database successfully\n";
    }

    // Create the Users table
    sql = "CREATE TABLE IF NOT EXISTS Users ("  
          "id INTEGER PRIMARY KEY,"
          "name TEXT NOT NULL,"
          "current_board INTEGER,"
          "active INTEGER NOT NULL,"
          "FOREIGN KEY(current_board) REFERENCES Boards(id)"
          ");";

    query(sql);
    cout << "Users table created successfully\n";

    // Create the Boards table
    sql = "CREATE TABLE IF NOT EXISTS Boards ("  
          "id INTEGER PRIMARY KEY,"
          "name TEXT NOT NULL,"
          "active INTEGER NOT NULL"
          ");";

    query(sql);
    cout << "Boards table created successfully\n";

    // Create the Tasks table
    sql = "CREATE TABLE IF NOT EXISTS Tasks ("  
          "id INTEGER PRIMARY KEY,"
          "title TEXT NOT NULL,"
          "description TEXT,"
          "assigned_user INTEGER,"
          "stage TEXT NOT NULL,"
          "due_date TEXT,"
          "difficulty_score INTEGER,"
          "active INTEGER NOT NULL,"
          "board_id INTEGER,"
          "FOREIGN KEY(assigned_user) REFERENCES Users(id),"
          "FOREIGN KEY(board_id) REFERENCES Boards(id)"
          ");";

    query(sql);
    cout << "Tasks table created successfully\n";

    // Close the SQLite database
    sqlite3_close(db);
}

// Methods to save data
void Database::saveBoardData(Board& board) {
    stringstream sql;
    sql << "INSERT OR REPLACE INTO Boards VALUES("
        << board.getId() << ", "
        << "'" << board.getName() << "', "
        << (board.isActive() ? 1 : 0) << ");";
    query(sql.str());
    // Save tasks related to this board
    for (Task* task : board.getTasks()) {
        saveTaskData(*task);
    }
}

void Database::saveTaskData(Task& task) {
    stringstream sql;
    sql << "INSERT OR REPLACE INTO Tasks VALUES("
        << task.getId() << ", "
        << "'" << task.getTitle() << "', "
        << "'" << task.getDescription() << "', "
        << task.getDifficultyScore() << ", "
        << (task.isActive() ? 1 : 0) << ", "
        << "'" << task.getDueDate() << "', " 
        << "'" << task.stageToString(task.getStage()) << "', "
        << task.getAssignedUser()->getId() << ");";
    query(sql.str());
}

void Database::saveUserData(User& user) {
    Board* board = user.getCurrentBoard();
    int boardId = board ? board->getId() : -1;  // -1 means user has no active board

    stringstream sql;
    sql << "INSERT OR REPLACE INTO Users VALUES("
        << user.getId() << ", "
        << "'" << user.getName() << "', "
        << (user.isActive() ? 1 : 0) << ", "
        << boardId << ");";
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

        Board* board = new Board(id, name);
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
        string stageStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        int assignedUserId = sqlite3_column_int(stmt, 7);

        // load and convert time
        string dueDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        tm timeStruct = {};
        strptime(dueDate.c_str(), "%Y-%m-%d %H:%M:%S", &timeStruct); // Convert string to time structure
        time_t dueDateTime = std::mktime(&timeStruct); // Convert structure to time_t

        Task* task = new Task(id, title);
        task->setDescription(description);
        task->setDifficultyScore(difficultyScore);
        task->setActive(active);
        task->setDueDate(dueDateTime);
        task->setStage(task->stringToStage(stageStr));
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

// Delete entities
void Database::deleteTask(Task& task) {
    string sql = "UPDATE Task SET active = 0 WHERE id = " + to_string(task.getId()) + ";";
    query(sql);
    task.setActive(false);
}

void Database::deleteBoard(Board& board) {
    string sql = "UPDATE Board SET active = 0 WHERE id = " + to_string(board.getId()) + ";";
    query(sql);
    board.setActive(false);
}

void Database::deleteUser(User& user, User& replacementUser) {
    // Update all the tasks assigned to this user
    string sql = "UPDATE Task SET assignedUser = " + to_string(replacementUser.getId()) 
               + " WHERE assignedUser = " + to_string(user.getId()) + ";";
    query(sql);

    // Update the user's active status
    sql = "UPDATE User SET active = 0 WHERE id = " + to_string(user.getId()) + ";";
    query(sql);

    user.setActive(false);
}
