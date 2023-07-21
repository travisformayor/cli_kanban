#ifndef BOARD_H
#define BOARD_H

#include "Task.h"
#include <string>
#include <list>

using namespace std;

class Task;

class Board {
public:
    Board(string name);
    ~Board();
    void setId(int id);
    void setName(string newName);
    void addTask(Task& task);
    void removeTask(Task& task);
    int getId();
    string getName();
    list<Task*> getTasks();
    static Board* findById(list<Board*> boards, int id);

private:
    int id;
    string name;
    list<Task*> tasks;
};

#endif // BOARD_H
