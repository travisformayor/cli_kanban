#include "Task.h"
#include <stdexcept>
#include <sstream>
// #include <iomanip>

using namespace std;

Task::Task(string title, Board& board) : title(title), active(true) {
    if (title.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->id = 0;
    this->stage = Stage::ToDo;
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
    switch (newStage) {
        case Stage::ToDo:
            break;
        case Stage::InProgress:
            if (this->description.empty() || this->difficultyScore == 0 || this->assignedUser == nullptr) {
                throw runtime_error("Task needs assigned user, description, difficulty for 'In Progress' stage.");
            }
            break;
        case Stage::Done:
            if (this->stage != Stage::InProgress) {
                throw runtime_error("Cannot skip 'In Progress' stage.");
            }
            break;
    }

    // if all requirements pass...
    this->stage = newStage;
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

int Task::getDifficultyScore() {
    return this->difficultyScore;
}

// helper convert methods for dealing with Stages as strings
string Task::stageToString(Stage stage) {
    switch (stage) {
    case Stage::ToDo:
        return "ToDo";
    case Stage::InProgress:
        return "InProgress";
    case Stage::Done:
        return "Done";
    default:
        throw runtime_error("Invalid stage");
    }
}

Stage Task::stringToStage(const string& stageStr) {
    if (stageStr == "ToDo")
        return Stage::ToDo;
    else if (stageStr == "InProgress")
        return Stage::InProgress;
    else if (stageStr == "Done")
        return Stage::Done;
    else
        throw runtime_error("Invalid stage string: " + stageStr);
}
