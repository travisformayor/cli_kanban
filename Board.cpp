#include "Board.h"
#include <stdexcept>

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

void Board::setId(int id) {
    this->id = id;
}

void Board::setTitle(string newTitle) {
    if (newTitle.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->title = newTitle;
}

void Board::addTask(Task& task) {
    tasks.push_back(&task);
}

void Board::removeTask(Task& task) {
    tasks.remove(&task);
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