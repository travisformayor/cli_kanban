#include "Board.h"
#include "User.h"
#include "Task.h"

using namespace std;

static int currentBoardId = 0; // global board id counter

Board::Board(string name, User* owner) : name(name), owner(owner), active(true) {
    if (name.empty() || owner == nullptr) {
        throw invalid_argument("Name can't be empty and owner can't be null.");
    }
    this->id = ++currentBoardId;
}

Board::Board(int id, string name, User* owner) : id(id), name(name), owner(owner), active(true) {
    if (id <= 0 || name.empty() || owner == nullptr) {
        throw invalid_argument("ID must be positive, name can't be empty, and owner can't be null.");
    }
}

void Board::setName(string newName) {
    if (newName.empty()) {
        throw invalid_argument("Name can't be empty.");
    }
    this->name = newName;
}

void Board::setOwner(User* newOwner) {
    if (newOwner == nullptr) {
        throw invalid_argument("Owner can't be null.");
    }
    this->owner = newOwner;
}

void Board::addTask(Task* task) {
    if (task == nullptr) {
        throw invalid_argument("Task can't be null.");
    }
    this->tasks.push_back(task);
}

void Board::removeTask(Task* task) {
    if (task == nullptr) {
        throw invalid_argument("Task can't be null.");
    }
    tasks.remove(task);
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

User* Board::getOwner() {
    return this->owner;
}

list<Task*> Board::getTasks() {
    return this->tasks;
}
