#include "User.h"
#include <stdexcept>

using namespace std;

User::User(string name) : name(name), active(true) {
    if (name.empty()) {
        throw invalid_argument("Name can't be empty.");
    }
    
    // to do: save to db and get id
}

User::User(int id, string name) : id(id), name(name), active(true) {
    // Only used by the load method to fetch existing users's that already have db ids
    if (id <= 0 || name.empty()) {
        throw invalid_argument("ID must be positive and name can't be empty.");
    }
}

User::~User() {
    currentBoard = nullptr;
}

void User::setName(string newName) {
    if (newName.empty()) {
        throw invalid_argument("New name can't be empty.");
    }
    this->name = newName;
}

void User::setActive(bool active) {
    this->active = active;
}

bool User::isActive() {
    return this->active;
}

void User::setCurrentBoard(Board* board) {
    if (board == nullptr) {
        throw invalid_argument("Board can't be null.");
    }
    this->currentBoard = board;
}

int User::getId() {
    return this->id;
}

string User::getName() {
    return this->name;
}

Board* User::getCurrentBoard() {
    return this->currentBoard;
}

User* User::findById(list<User*> users, int id) {
    for (list<User*>::iterator it = users.begin(); it != users.end(); ++it) {
        if ((*it)->getId() == id) {
            return *it;
        }
    }
    return nullptr; // if no user with the given id was found
}