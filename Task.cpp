#include "Task.h"
#include <stdexcept>

using namespace std;

Task::Task(string title, Board& board) : title(title), active(true) {
    if (title.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->stage = Stage::Backlog;
    this->dueDate = 0;
    this->difficultyScore = 0;
}

Task::~Task() {
    // Nothing to deallocate
}

void Task::setId(int id) {
    this->id = id;
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
    this->assignedUser = user;
}

void Task::setStage(Stage newStage) {
    // to do: check stage requirements or throw error
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
    if (this->assignedUser != nullptr) {
        return this->assignedUser;
    }
    else {
        return nullptr;
    }
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

// helper convert methods for dealing with Stages as strings
string Task::stageToString(Stage stage) {
    switch (stage) {
    case Stage::Backlog:
        return "Backlog";
    case Stage::ToDo:
        return "ToDo";
    case Stage::InProgress:
        return "InProgress";
    case Stage::Done:
        return "Done";
    case Stage::Archive:
        return "Archive";
    default:
        throw runtime_error("Invalid stage");
    }
}

Stage Task::stringToStage(const string& stageStr) {
    if (stageStr == "Backlog")
        return Stage::Backlog;
    else if (stageStr == "ToDo")
        return Stage::ToDo;
    else if (stageStr == "InProgress")
        return Stage::InProgress;
    else if (stageStr == "Done")
        return Stage::Done;
    else if (stageStr == "Archive")
        return Stage::Archive;
    else
        throw runtime_error("Invalid stage string: " + stageStr);
}
