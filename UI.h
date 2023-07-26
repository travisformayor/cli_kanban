#ifndef UI_H
#define UI_H

// exclude parts of <windows.h> causing build errors
#define WIN32_LEAN_AND_MEAN
#define RPC_NO_WINDOWS_H
#include <windows.h>

#include "Database.h"
#include "Board.h"
#include "Task.h"
#include <iostream>
#include <conio.h>
#include <variant>
#include <string>
#include <list>

using namespace std;

class UI {
public:
    UI(Database& db);
    ~UI();
    // methods to manipulate the interface
    void setTextColor(WORD color);
    void setSelectIndex(int index);
    void displayScreen();
    void displayTitles(map<string, list<string>>& titles);
    void displayTaskCard(string taskDetails);
    string getUserInput(const string& prompt);
    void addAlert(const string& alert);
    void keyboardListen();
    void moveSelector(int direction);
    void changeScreen(string command);

    // methods to manage displayed boards and tasks
    Board* getBoardById(int id);
    void reloadBoards();
    void reloadBoardTasks();
    void findSelectedBoard();
    void findSelectedTask();
    void addNewBoard();
    void addNewTask();
    void deleteSelectedBoard();
    void deleteSelectedTask();

    // methods to edit displayed boards and tasks
    void editBoardTitle();
    void editTaskTitle();
    void editTaskDescription();
    void editTaskStage();
    void editTaskRating();

private:
    const WORD TEXT_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    const WORD TEXT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;

    Database db;
    map<string, string> screenMenus;
    list<string> userAlerts;
    int screenWidth;
    string padL;
    string padHeader;
    int selectedIndex;
    string currScreen;
    list<Board*> loadedBoards;
    int activeBoardId;
    int activeTaskId;
};

#endif // UI_H
