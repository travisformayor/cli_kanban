#ifndef TASK_H
#define TASK_H

#include "Board.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <list>

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
    void setId(const int id);
    void setTitle(const string newTitle);
    void setDescription(const string newDesc);
    void setStage(const Stage newStage, const bool loading);
    void setDifficulty(const int rating);
    void setBoardId(const int boardId);
    int getId();
    string getTitle();
    string getDescription();
    Stage getStage();
    int getDifficultyRating();
    int getBoardId();
    list<Task*> searchTasks(list<Task*> tasks, const string& query);
    static string stageToString(Stage stage);
    static Stage stringToStage(const string& stageStr);

private:
    int id;
    string title;
    string description;
    Stage stage;
    int difficultyRating;
    int boardId;
};

#endif // TASK_H
