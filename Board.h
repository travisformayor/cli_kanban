#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <list>
#include <stdexcept>

using namespace std;

class Task; // Forward declaration
class User; // Forward declaration

class Board {
public:
    Board(string name);
    Board(int id, string name);
    void setName(string newName);
    void addTask(Task& task);
    void removeTask(Task& task);
    void setActive(bool active);
    bool isActive();
    int getId();
    string getName();
    list<Task*> getTasks();

private:
    int id;
    string name;
    list<Task*> tasks;
    bool active;
};

#endif // BOARD_H
