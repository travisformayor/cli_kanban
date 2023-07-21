#include "Database.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace std;

// Constructor 
Database::Database(string dbName) : dbName(dbName) {
    int resultCode = sqlite3_open(dbName.c_str(), &db);
    if (resultCode != SQLITE_OK) {
        sqlite3_close(db);
        string errorMsg = "Error opening database: " + string(sqlite3_errmsg(db));
        throw runtime_error(errorMsg);
    }

    // Setup tables if not already created
    createTables();
}

// Destructor
Database::~Database() {
    sqlite3_close(db);
}

// Setup the DB Tables
void Database::createTables() {
    string sql;

    // Tables relationship info:
    // - A Board can have many Tasks (or none)
    // - A Task belongs to one Board

    // Create the Boards table
    sql = "CREATE TABLE IF NOT EXISTS Boards ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "active INTEGER NOT NULL"
        ");";

    executeQuery(sql, {});

    // Create the Tasks table
    sql = "CREATE TABLE IF NOT EXISTS Tasks ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "title TEXT NOT NULL,"
        "description TEXT,"
        "stage TEXT NOT NULL,"
        "difficulty_score INTEGER,"
        "active INTEGER NOT NULL,"
        "board_id INTEGER NOT NULL,"
        "FOREIGN KEY(board_id) REFERENCES Boards(id)"
        ");";

    executeQuery(sql, {});
}

// Clear the DB.
void Database::deleteTables() {
    try {
        executeQuery("DROP TABLE IF EXISTS Boards;", {});
        executeQuery("DROP TABLE IF EXISTS Tasks;", {});
    }
    catch (const runtime_error& e) {
        cerr << "Caught exception: " << e.what() << endl;
    }
}

// helper method to safely execute sql queries
int Database::executeQuery(const string& sql, const map<string, variant<int, string>>& dataMap) {
    sqlite3_stmt* stmt;

    // prepare the db action
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Error preparing query statement: " + string(sqlite3_errmsg(db)));
    }

    // bind values to sql datatype
    int index = 1;
    for (const auto& param : dataMap) {
        if (holds_alternative<int>(param.second)) {
            sqlite3_bind_int(stmt, index, get<int>(param.second));
        }
        else if (holds_alternative<string>(param.second)) {
            sqlite3_bind_text(stmt, index, get<string>(param.second).c_str(), -1, SQLITE_STATIC);
        }
        index++;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        throw runtime_error("Failed to execute statement: " + string(sqlite3_errmsg(db)));
    }
    sqlite3_finalize(stmt);

    // return record db id
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

// helper method to construct the sql query string
string Database::queryString(const string& tableName, const map<string, variant<int, string>>& dataMap) {
    list<string> columns;
    for (const auto& pair : dataMap) {
        columns.push_back(pair.first);
    }

    bool hasId = (!columns.empty() && columns.front() == "id");
    string query = hasId ? "REPLACE INTO " : "INSERT INTO ";
    query += tableName + "(";

    // Add column names
    for (auto i = columns.begin(); i != columns.end(); ++i) {
        query += *i;
        if (next(i) != columns.end()) {
            query += ", ";
        }
    }

    // Add placeholder values
    query += ") VALUES(";
    for (auto i = columns.begin(); i != columns.end(); ++i) {
        query += "?";
        if (next(i) != columns.end()) {
            query += ", ";
        }
    }
    query += ")";

    return query;
}

// methods to save data
void Database::saveBoardData(Board& board) {
    string tableName = "Boards";


    map<string, variant<int, string>> dataMap = {
        { "name", board.getName() },
        { "active", board.isActive() ? 1 : 0 }
    };

    // include id only if not new
    bool isNew = (board.getId() == 0);
    if (!isNew) {
        dataMap.insert({"id", board.getId()});
    }

    // create sql statement and execute
    string sql = queryString(tableName, dataMap);
    int returnId = executeQuery(sql, dataMap);

    // If new record, fetch the created db id
    if (isNew) {
        board.setId(returnId);
    }
}

void Database::saveTaskData(Task& task) {
    string tableName = "Tasks";
    map<string, variant<int, string>> dataMap = {
        { "title", variant<int, string>{task.getTitle()} },
        { "description", variant<int, string>{task.getDescription()} },
        { "difficulty_score", variant<int, string>{task.getDifficultyScore()} },
        { "active", variant<int, string>{task.isActive() ? 1 : 0} },
        { "stage", variant<int, string>{task.stageToString(task.getStage())} }
    };

    // include id only if not new
    bool isNew = (task.getId() == 0);
    if (!isNew) {
        dataMap.insert({"id", task.getId()});
    }

    // create sql statement and execute
    string sql = queryString(tableName, dataMap);
    int returnId = executeQuery(sql, dataMap);

    // If new record, fetch the created db id
    if (isNew) {
        task.setId(returnId);
    }
}

// delete entities
void Database::deleteBoard(Board& board) {
    string sql = "UPDATE Boards SET active = ? WHERE id = ?";

    map<string, variant<int, string>> dataMap = {
        { "active", variant<int, string>{0} },
        { "id", variant<int, string>{board.getId()} }
    };

    executeQuery(sql, dataMap);

    board.setActive(false);
}

void Database::deleteTask(Task& task) {
    string sql = "UPDATE Tasks SET active = ? WHERE id = ?";

    map<string, variant<int, string>> dataMap = {
        { "active", variant<int, string>{0} },
        { "id", variant<int, string>{task.getId()} }
    };

    executeQuery(sql, dataMap);

    task.setActive(false);
}

// Methods to load data
list<Board*> Database::loadBoardData() {
    list<Board*> boards;
    string sql = "SELECT * FROM Boards WHERE Active = 1;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Error preparing load board statement: " + string(sqlite3_errmsg(db)));
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

list<Task*> Database::loadTaskData(list<Board*> boards) {
    list<Task*> tasks;
    string sql = "SELECT * FROM Tasks WHERE Active = 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        throw runtime_error("Error preparing load task statement: " + string(sqlite3_errmsg(db)));
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // get id
        int id = sqlite3_column_int(stmt, 0);
        // Parse title and description as string or empty string
        const char* titleRaw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string title = titleRaw ? titleRaw : "";
        const char* descriptionRaw = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string description = descriptionRaw ? descriptionRaw : "";
        // get attributes
        int difficultyScore = sqlite3_column_int(stmt, 3);
        bool active = sqlite3_column_int(stmt, 4) == 1;
        int boardId = sqlite3_column_int(stmt, 5);
        Board* board = Board::findById(boards, boardId);
        string stageStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
    
        // create task object, save fetched info
        Task* task = new Task(title, *board);
        task->setId(id);
        task->setDescription(description);
        task->setDifficultyScore(difficultyScore);
        task->setActive(active);
        task->setStage(task->stringToStage(stageStr));

        tasks.push_back(task);
    }

    sqlite3_finalize(stmt);
    return tasks;
}
