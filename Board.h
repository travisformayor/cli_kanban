#ifndef BOARD_H
#define BOARD_H

#include "Task.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <list>

using namespace std;

class Task;

class Board {
public:
    Board(string title);
    ~Board();
    void setId(const int id);
    void setTitle(const string newTitle);
    void setTasks(const list<Task*>& tasks);
    void addTask(Task& task);
    void removeTask(Task& task);
    int getId();
    string getTitle();
    list<Task*>& getTasks();
    static Board* findById(list<Board*> boards, int id);

private:
    int id;
    string title;
    list<Task*> tasks;
};

#endif // BOARD_H
