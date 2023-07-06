#include "Database.h"
#include <sqlite3.h> 
#include <iostream>
#include <variant>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <ctime>

using namespace std;

// Constructor 
Database::Database(string dbName) : dbName(dbName) {
    int resultCode = sqlite3_open(dbName.c_str(), &db);
    if (resultCode != 0) { // 0 means success
        sqlite3_close(db);
        string errorMsg = "Error opening database: " + string(sqlite3_errmsg(db));
        throw runtime_error(errorMsg);
    }
}

// Destructor
Database::~Database() {
    sqlite3_close(db);
}

// Setup the DB Tables
void Database::createTables() {
    string sql;

    // Open the SQLite database
    int result = sqlite3_open("kanban_database.db", &db);
    if (result) {
        throw runtime_error(string("Can't open database: ") + sqlite3_errmsg(db));
    }
    else {
        cout << "Opened database successfully\n";
    }

    // Tables relationship info:
    // - A Task belongs to one User (or none)
    // - A User can have many Tasks (or none)
    // - A Task belongs to one Board
    // - A Board can have many Tasks (or none)

    // Create the Users table
    sql = "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "active INTEGER NOT NULL"
        ");";

    executeSQL(sql, {});
    cout << "Users table created successfully\n";

    // Create the Boards table
    sql = "CREATE TABLE IF NOT EXISTS Boards ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL,"
        "active INTEGER NOT NULL"
        ");";

    executeSQL(sql, {});
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
        "board_id INTEGER NOT NULL,"
        "FOREIGN KEY(assigned_user) REFERENCES Users(id),"
        "FOREIGN KEY(board_id) REFERENCES Boards(id)"
        ");";

    executeSQL(sql, {});
    cout << "Tasks table created successfully\n";

    // Close the SQLite database
    sqlite3_close(db);
}

// Clear the DB.
void Database::deleteTables() {
    try {
        executeSQL("DROP TABLE IF EXISTS Boards;", {});
        executeSQL("DROP TABLE IF EXISTS Users;", {});
        executeSQL("DROP TABLE IF EXISTS Tasks;", {});
    }
    catch (const runtime_error& e) {
        cerr << "Caught exception: " << e.what() << endl;
    }
}

// helper method to safely execute sql queries
void Database::executeSQL(const string& sql, const list<variant<int, string, bool, optional<int>>>& params) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Failed to prepare statement: " + string(sqlite3_errmsg(db)));
    }

    int index = 1;
    for (const auto& param : params) {
        if (holds_alternative<int>(param)) {
            sqlite3_bind_int(stmt, index, get<int>(param));
        }
        else if (holds_alternative<string>(param)) {
            sqlite3_bind_text(stmt, index, get<string>(param).c_str(), -1, SQLITE_STATIC);
        }
        else if (holds_alternative<bool>(param)) {
            sqlite3_bind_int(stmt, index, get<bool>(param) ? 1 : 0);  // Convert boolean to int
        }
        else if (holds_alternative<optional<int>>(param)) {
            if (get<optional<int>>(param).has_value()) {
                sqlite3_bind_int(stmt, index, get<optional<int>>(param).value());
            }
            else {
                sqlite3_bind_null(stmt, index);
            }
        }
        ++index;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        throw runtime_error("Failed to execute statement: " + string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
}

// methods to save data
void Database::saveBoardData(Board& board) {
    string sql = "INSERT OR REPLACE INTO Boards(id, name, active) VALUES(?, ?, ?)";
    list<variant<int, string, bool, optional<int>>> params = {
        board.getId(),
        board.getName(),
        board.isActive() ? 1 : 0
    };
    executeSQL(sql, params);

    // If this was a new record, get the db id and include it
    if (board.getId() == 0) {
        board.setId(sqlite3_last_insert_rowid(db));
    }
}

void Database::saveTaskData(Task& task) {
    string sql = "INSERT OR REPLACE INTO Tasks(id, title, description, difficulty_score, active, due_date, stage, assigned_user) VALUES(?, ?, ?, ?, ?, ?, ?, ?)";
    list<variant<int, string, bool, optional<int>>> params = {
        task.getId(),
        task.getTitle(),
        task.getDescription(),
        task.getDifficultyScore(),
        task.isActive(),
        task.getDueDate(),
        task.stageToString(task.getStage()),
        (task.getAssignedUser() == nullptr) ? optional<int>{} : task.getAssignedUser()->getId()
    };

    executeSQL(sql, params);

    // If this was a new record, get the db id and include it
    if (task.getId() == 0) {
        task.setId(sqlite3_last_insert_rowid(db));
    }
}

void Database::saveUserData(User& user) {
    string sql = "INSERT OR REPLACE INTO Users(id, name, active) VALUES(?, ?, ?)";
    list<variant<int, string, bool, optional<int>>> params = {
        user.getId(),
        user.getName(),
        user.isActive() ? 1 : 0
    };
    executeSQL(sql, params);

    // If this was a new record, get the db id and include it
    if (user.getId() == 0) {
        user.setId(sqlite3_last_insert_rowid(db));
    }
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

        Board* board = new Board(name);
        board->setId(id);
        board->setActive(active);
        boards.push_back(board);
    }

    sqlite3_finalize(stmt);
    return boards;
}

list<Task*> Database::loadTaskData(list<Board*> boards, list<User*> users) {
    list<Task*> tasks;
    string sql = "SELECT * FROM Tasks WHERE Active = 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Failed to prepare statement: " + string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        int difficultyScore = sqlite3_column_int(stmt, 3);
        bool active = sqlite3_column_int(stmt, 4) == 1;
        int boardId = sqlite3_column_int(stmt, 5);
        Board* board = Board::findById(boards, boardId);
        string stageStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));

        // Parse date
        string dueDateStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        tm dueDateTm = {};
        strptime(dueDateStr.c_str(), "%Y-%m-%d %H:%M:%S", &dueDateTm);
        time_t dueDate = mktime(&dueDateTm);

        int assignedUserId = sqlite3_column_int(stmt, 8);
        User* user = User::findById(users, assignedUserId);

        Task* task = new Task(title, *board);
        task->setId(id);
        task->setDescription(description);
        task->setDifficultyScore(difficultyScore);
        task->setActive(active);
        task->setDueDate(dueDate);
        task->setStage(task->stringToStage(stageStr));
        if (user != nullptr) {
            task->setAssignedUser(user);
        }

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

        User* user = new User(name);
        user->setId(id);
        user->setActive(active);
        users.push_back(user);
    }

    sqlite3_finalize(stmt);
    return users;
}

// delete entities
void Database::deleteTask(Task& task) {
    string sql = "UPDATE Tasks SET active = ? WHERE id = ?";
    list<variant<int, string, bool, optional<int>>> params = {
        0,  // Active
        task.getId()
    };
    executeSQL(sql, params);

    task.setActive(false);
}

void Database::deleteBoard(Board& board) {
    string sql = "UPDATE Boards SET active = ? WHERE id = ?";
    list<variant<int, string, bool, optional<int>>> params = {
        0,  // Active
        board.getId()
    };
    executeSQL(sql, params);

    board.setActive(false);
}

void Database::deleteUser(User& user, User& replacementUser) {
    // Update all the tasks assigned to this user
    string sql = "UPDATE Tasks SET assigned_user = ? WHERE assigned_user = ?";
    list<variant<int, string, bool, optional<int>>> params = {
        replacementUser.getId(),
        user.getId()
    };
    executeSQL(sql, params);

    // Update the user's active status
    sql = "UPDATE Users SET active = ? WHERE id = ?";
    params = {
        0,  // Active
        user.getId()
    };
    executeSQL(sql, params);

    user.setActive(false);
}
