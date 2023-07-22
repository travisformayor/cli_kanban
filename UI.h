#ifndef UI_H
#define UI_H

#include "Board.h"
#include "Task.h"
#include "Database.h"
#include <windows.h>
#include <conio.h>
#include <list>
#include <string>

using namespace std;

class UI {
public:
    void setTextColor(WORD color);
    void topMenu(const string& currentScreen);
    void displayScreen(const string& currentScreen, int& selectedIndex, const list<string>& items);
    void displayList(const list<string>& items, int& selectedIndex);
    void addBoard(Database* db, list<Board*>& boards);
    void removeBoard(Database* db, list<Board*>& boards, int& selectedIndex);
    void addTask(Database* db, list<Task*>& tasks);
    void removeTask(Database* db, list<Task*>& tasks, int& selectedIndex);
    string getUserInput(const string& prompt);
    void navControls(string& screen, int& selectedIndex, Database* db, variant<list<Board*>, list<Task*>>& items);
    void taskEditControls(string& screen, int& selectedIndex, Database* db, Task* task);

private:
    const WORD TEXT_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    const WORD TEXT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;

    map<string, string> screenMenus = {
        {"Boards", "    | up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | esc: Quit |   "},
        {"BoardView", "| up/down: Navigate | enter: Select | c: Create Task | d: Delete Task | b: Back | esc: Quit | "},
        {"Task", " Select to Edit | t: Title | d: Description | s: Stage | r: Rated Difficulty | b: Back/Save "}
    };
};

#endif // UI_H