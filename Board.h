#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <list>
#include "Task.h"

using namespace std;

class Board {
public:
    Board(string name);
    ~Board();
    void setId(int id);
    void setName(string newName);
    void addTask(Task& task);
    void removeTask(Task& task);
    void setActive(bool active);
    bool isActive();
    int getId();
    string getName();
    list<Task*> getTasks();
    static Board* findById(list<Board*> boards, int id);

private:
    int id;
    string name;
    list<Task*> tasks;
    bool active;
};

#endif // BOARD_H
