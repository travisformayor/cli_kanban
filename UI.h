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
    void reloadBoards();
    void reloadBoardTasks();
    void findSelectedBoard();
    void findSelectedTask();
    void displayScreen();
    void displayTitles(list<string>& titles);
    void displayTaskCard(string taskDetails);
    void addNewBoard();
    void addNewTask();
    void deleteSelectedBoard();
    void deleteSelectedTask();
    string getUserInput(const string& prompt);
    void keyboardListen();
    void moveSelector(int direction);
    void changeScreen(string command);

    // methods to update boards and tasks
    void editBoardTitle();
    void editTaskTitle();
    void editTaskDescription();
    void editTaskStage();
    void editTaskRating();

private:
    const WORD TEXT_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    const WORD TEXT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;

    Database db;
    list<Board*> loadedBoards;
    Board* selectedBoardPtr;
    Task* selectedTaskPtr;
    int selectedIndex;
    string currScreen;
    map<string, string> screenMenus;
};

#endif // UI_H