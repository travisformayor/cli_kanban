#include "Task.h"

using namespace std;

Task::Task(string title, Board& board) : title(title) {
    if (title.empty()) {
        throw invalid_argument("Title can't be empty.");
    }
    this->id = 0;
    this->stage = Stage::ToDo;
    this->difficultyRating = 1;
    this->boardId = board.getId();
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
    else if (newTitle.length() > 50) { // char limit on title
        throw invalid_argument("Title can't exceed 50 characters.");
    }
    this->title = newTitle;
}

void Task::setDescription(string newDesc) {
    this->description = newDesc;
}

void Task::setStage(Stage newStage, bool loading) {
    switch (newStage) {
    case Stage::ToDo:
        break;
    case Stage::InProgress:
        if (this->description.empty() || this->difficultyRating == 0) {
            throw invalid_argument("Task needs description and difficulty for 'In Progress' stage.");
        }
        break;
    case Stage::Done:
        // check if task update or loading task from db
        // skip previous stage check if loading is true
        if (!loading && (this->stage == Stage::ToDo)) {
            throw invalid_argument("Cannot skip 'In Progress' stage.");
        }
        break;
    }

    // if all requirements pass...
    this->stage = newStage;
}

void Task::setDifficulty(int rating) {
    if (rating < 1 || rating > 5) {
        throw invalid_argument("Enter a number 1 - 5.");
    }
    this->difficultyRating = rating;
}

void Task::setBoardId(int boardId) {
    this->boardId = boardId;
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

int Task::getDifficultyRating() {
    return this->difficultyRating;
}

int Task::getBoardId() {
    return this->boardId;
}


list<Task*> Task::searchTasks(list<Task*> tasks, const string& query) {
    list<Task*> foundTasks;
    for (Task* task : tasks) {
        if (task->getTitle().find(query) != string::npos || task->getDescription().find(query) != string::npos) {
            foundTasks.push_back(task);
        }
    }
    return foundTasks;
}

// helper convert methods for dealing with Stages as strings
string Task::stageToString(Stage stage) {
    switch (stage) {
    case Stage::ToDo: return "To Do";
    case Stage::InProgress: return "In Progress";
    case Stage::Done: return "Done";
    default: throw invalid_argument("Invalid stage");
    }
}

Stage Task::stringToStage(const string& stageStr) {
    if (stageStr == "To Do")
        return Stage::ToDo;
    else if (stageStr == "In Progress")
        return Stage::InProgress;
    else if (stageStr == "Done")
        return Stage::Done;
    else
        throw invalid_argument("Invalid stage. Enter To Do, In Progress, or Done.");
}
