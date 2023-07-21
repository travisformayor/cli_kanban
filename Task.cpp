#include "Task.h"
#include <stdexcept>
#include <sstream>

using namespace std;

Task::Task(string title, Board& board) : title(title) {
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

void Task::setStage(Stage newStage) {
    switch (newStage) {
        case Stage::ToDo:
            break;
        case Stage::InProgress:
            if (this->description.empty() || this->difficultyScore == 0) {
                throw runtime_error("Task needs description and difficulty for 'In Progress' stage.");
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

int Task::getId() {
    return this->id;
}

string Task::getTitle() {
    return this->title;
}

string Task::getDescription() {
    return this->description;
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
