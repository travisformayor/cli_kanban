#include "Board.h"

using namespace std;

Board::Board(string title) : title(title) {
    if (title.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->id = 0;
}

Board::~Board() {
    // Deallocate assigned tasks list
    for (auto task : tasks) {
        delete task;
        task = nullptr;
    }
}

void Board::setId(const int id) {
    this->id = id;
}

void Board::setTitle(const string newTitle) {
    if (newTitle.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->title = newTitle;
}

void Board::setTasks(const list<Task*>& tasks) {
    this->tasks.clear();
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
