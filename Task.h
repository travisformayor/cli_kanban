#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>
#include "User.h"
#include "Board.h"

using namespace std;

enum class Stage {
    Backlog,
    ToDo,
    InProgress,
    Done,
    Archive
};

class Task {
public:
    Task(string title, Board& board);
    ~Task();
    void setId(int id);
    void setTitle(string newTitle);
    void setDescription(string newDesc);
    void setAssignedUser(User* user);
    void setStage(Stage newStage);
    void setDueDate(time_t newDueDate);
    void setDifficultyScore(int score);
    void setActive(bool active);
    bool isActive();
    int getId();
    string getTitle();
    string getDescription();
    User* getAssignedUser();
    Stage getStage();
    time_t getDueDate();
    int getDifficultyScore();
    string stageToString(Stage stage);
    Stage stringToStage(const string& stageStr);

private:
    int id;
    string title;
    string description;
    User* assignedUser;
    Stage stage;
    time_t dueDate;
    int difficultyScore;
    bool active;
};

#endif // TASK_H
