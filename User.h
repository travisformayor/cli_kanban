#ifndef USER_H
#define USER_H

#include <string>
#include "Board.h"

using namespace std;

class User {
public:
    User(string name);
    User(int id, string name);
    void setName(string newName);
    void setCurrentBoard(Board* board);
    void setActive(bool active);
    bool isActive();
    int getId();
    string getName();
    Board* getCurrentBoard();

private:
    int id;
    string name;
    Board* currentBoard;
    bool active;
};

#endif // USER_H
