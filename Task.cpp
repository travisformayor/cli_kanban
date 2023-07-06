#include "Task.h"
#include <stdexcept>
#include <ctime>

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
    switch (newStage) {
        case Stage::ToDo:
            if (this->description.empty() || this->difficultyScore == 0) {
                throw runtime_error("Task needs description and difficulty score to enter 'To Do' stage.");
            }
            if (this->stage != Stage::Backlog) {
                throw runtime_error("Cannot skip stages. Backlog before To Do.");
            }
            break;
        case Stage::InProgress:
            if (this->description.empty() || this->difficultyScore == 0 || this->assignedUser == nullptr || this->dueDate == 0) {
                throw runtime_error("Task needs description, difficulty score, assigned user, and due date to enter 'In Progress' stage.");
            }
            if (this->stage != Stage::ToDo) {
                throw runtime_error("Cannot skip stages. 'To Do' before 'In Progress'.");
            }
            break;
        case Stage::Done:
            if (this->stage != Stage::InProgress) {
                throw runtime_error("Cannot skip stages. 'In Progress' before 'Done'.");
            }
            break;
        case Stage::Archive:
            if (this->stage != Stage::Done) {
                throw runtime_error("Cannot skip stages. 'Done' before 'Archive', or delete the Task.");
            }
            break;
    }

    // if all requirements pass...
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
