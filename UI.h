#ifndef UI_H
#define UI_H

#include "Database.h"
#include "Board.h"
#include "Task.h"
#include <windows.h>
#include <conio.h>
#include <variant>
#include <string>
#include <list>

using namespace std;

class UI {
public:
    UI(Database& db);
    void setTextColor(WORD color);
    void setSelectIndex(int index);
    string getScreen();
    bool screenChanged();
    Board* loadSelectedBoard();
    Task* getSelectedTask();
    void displayScreen();
    void displayTitles(list<string>& titles);
    void topMenu();
    void loadBoards();
    void addBoard();
    void removeBoard();
    void addTask();
    void removeTask();
    string getUserInput(const string& prompt);
    void boardListControls();
    void boardViewControls();
    void taskViewControls();

private:
    const WORD TEXT_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    const WORD TEXT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;

    Database db;
    list<Board*> loadedBoards;
    list<Task*> loadedTasks;
    Board selectedBoard;
    Task selectedTask;
    int selectedIndex;
    string currScreen;
    string prevScreen;
    map<string, string> screenMenus;
};

#endif // UI_H