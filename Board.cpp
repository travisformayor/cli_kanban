#include "Board.h"

using namespace std;

Board::Board(string name) : name(name), active(true) {
    if (name.empty()) {
        throw invalid_argument("Name can't be empty.");
    }
}

Board::~Board() {
    // Deallocate assigned tasks list
    for (auto task : tasks) {
        delete task;
        task = nullptr;
    }
}

void Board::setId(int id) {
    this->id = id;
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
    }
    else {
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

Board* Board::findById(list<Board*> boards, int id) {
    for (list<Board*>::iterator it = boards.begin(); it != boards.end(); ++it) {
        if ((*it)->getId() == id) {
            return *it;
        }
    }
    return nullptr; // if no board with the given id was found
}