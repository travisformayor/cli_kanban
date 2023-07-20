#ifndef TASK_H
#define TASK_H

#include "User.h"
#include "Board.h"
#include <string>

using namespace std;

class User;
class Board;

enum class Stage {
    ToDo,
    InProgress,
    Done
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
    void setDifficultyScore(int score);
    void setActive(bool active);
    bool isActive();
    int getId();
    string getTitle();
    string getDescription();
    User* getAssignedUser();
    Stage getStage();
    int getDifficultyScore();
    static string stageToString(Stage stage);
    static Stage stringToStage(const string& stageStr);

private:
    int id;
    string title;
    string description;
    User* assignedUser;
    Stage stage;
    int difficultyScore;
    bool active;
};

#endif // TASK_H
