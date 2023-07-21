#ifndef TASK_H
#define TASK_H

#include "Board.h"
#include <string>

using namespace std;

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
    void setStage(Stage newStage);
    void setDifficultyScore(int score);
    void setActive(bool active);
    bool isActive();
    int getId();
    string getTitle();
    string getDescription();
    Stage getStage();
    int getDifficultyScore();
    static string stageToString(Stage stage);
    static Stage stringToStage(const string& stageStr);

private:
    int id;
    string title;
    string description;
    Stage stage;
    int difficultyScore;
    bool active;
};

#endif // TASK_H
