#include "Task.h"
#include <stdexcept>

using namespace std;

static int currentTaskId = 0; // global task id counter

Task::Task(string title, Board* board) : title(title), active(true) {
    if (title.empty() || board == nullptr) {
        throw invalid_argument("Title can't be empty and board can't be null.");
    }
    this->id = ++currentTaskId;
    this->stage = Stage::Backlog;
}

Task::Task(int id, string title, Board* board) : id(id), title(title), active(true) {
    if (id <= 0 || title.empty() || board == nullptr) {
        throw invalid_argument("ID must be positive, title can't be empty, and board can't be null.");
    }
}

void Task::setTitle(string newTitle) {
    if (newTitle.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->title = newTitle;
}

void Task::setDescription(string newDesc) {
    this->description = newDesc;
}

void Task::setAssignedUser(User* user) {
    if (user == nullptr) {
        throw invalid_argument("User can't be null.");
    }
    this->assignedUser = user;
}

void Task::setStage(Stage newStage) {
    this->stage = newStage;
}

void Task::setDueDate(time_t newDueDate) {
    this->dueDate = newDueDate;
}

void Task::setDifficultyScore(int score) {
    if (score < 1 || score > 5) {
        throw invalid_argument("Score must be between 1 and 5.");
    }
    this->difficultyScore = score;
}

void Task::setActive(bool active) {
    this->active = active;
}

bool Task::isActive() {
    return this->active;
}

int Task::getId() {
    return this->id;
}

string Task::getTitle() {
    return this->title;
}

string Task::getDescription() {
    return this->description;
}

User* Task::getAssignedUser() {
    return this->assignedUser;
}

Stage Task::getStage() {
    return this->stage;
}

time_t Task::getDueDate() {
    return this->dueDate;
}

int Task::getDifficultyScore() {
    return this->difficultyScore;
}
