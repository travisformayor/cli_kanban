#ifndef USER_H
#define USER_H

#include "Board.h"
#include <string>
#include <list>

class Board;

using namespace std;

class User {
public:
    User(string name);
    ~User();
    void setId(int id);
    void setName(string newName);
    void setCurrentBoard(Board* board);
    void setActive(bool active);
    bool isActive();
    int getId();
    string getName();
    Board* getCurrentBoard();
    static User* findById(list<User*> users, int id);

private:
    int id;
    string name;
    Board* currentBoard;
    bool active;
};

#endif // USER_H
