#include "Board.h"

using namespace std;

Board::Board(string title) : title(title) {
    if (title.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->id = 0;
}

Board::~Board() {
    // Deallocate and clear assigned tasks list
    for (auto task : this->tasks) {
        delete task;
    }
    this->tasks.clear();
}

void Board::setId(const int id) {
    this->id = id;
}

void Board::setTitle(const string newTitle) {
    if (newTitle.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    else if (newTitle.length() > 50) { // char limit on title
        throw invalid_argument("Title can't exceed 50 characters.");
    }
    this->title = newTitle;
}

void Board::setTasks(const list<Task*>& tasks) {
    // deallocate and clear current list
    for (auto task : this->tasks) {
        delete task;
    }
    this->tasks.clear();
    // set new list
    this->tasks = tasks;
}

void Board::addTask(Task& task) {
    this->tasks.push_back(&task);
}

void Board::removeTask(Task& task) {
    this->tasks.remove(&task);
}

int Board::getId() {
    return this->id;
}

string Board::getTitle() {
    return this->title;
}

list<Task*>& Board::getTasks() {
    return this->tasks;
}

Task* Board::getTaskById(int id) {
    for (Task* task : tasks) {
        if (task->getId() == id) {
            return task;
        }
    }
    throw runtime_error("Task with id " + to_string(id) + " not found.");
    return nullptr;
}
