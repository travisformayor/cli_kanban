#include "Board.h"
#include "User.h"
#include "Task.h"

using namespace std;

static int currentBoardId = 0; // global board id counter

Board::Board(string name) : name(name), active(true) {
    if (name.empty()) {
        throw invalid_argument("Name can't be empty.");
    }
    this->id = ++currentBoardId;
}

Board::Board(int id, string name) : id(id), name(name), active(true) {
    if (id <= 0 || name.empty()) {
        throw invalid_argument("ID must be positive and name can't be empty.");
    }
}

Board::~Board() {
    // Deallocate assigned tasks list
    for(auto task : tasks) {
        delete task;
        task = nullptr;
    }
}

void Board::setName(string newName) {
    if (newName.empty()) {
        throw invalid_argument("Name can't be empty.");
    }
    this->name = newName;
}

void Board::addTask(Task& task) {
    if (task.isActive()) {
        tasks.push_back(&task);
    } else {
        throw invalid_argument("Task must be active");
    }
}

void Board::removeTask(Task& task) {
    tasks.remove(&task);
}

void Board::setActive(bool active) {
    this->active = active;
}

bool Board::isActive() {
    return this->active;
}

int Board::getId() {
    return this->id;
}

string Board::getName() {
    return this->name;
}

list<Task*> Board::getTasks() {
    return this->tasks;
}
