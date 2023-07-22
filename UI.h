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
    void topMenu();
    void setTextColor(WORD color);
    void setSelectIndex(int index);
    void loadBoards();
    void loadSelectedBoard();
    void getSelectedTask();
    void displayScreen();
    void displayTitles(list<string>& titles);
    void displayTaskCard(string taskDetails);
    void addNewBoard();
    void removeSelectedBoard();
    void addNewTask();
    void removeSelectedTask();
    string getUserInput(const string& prompt);
    void keyboardListen();
    void changeSelector(int direction);
    void changeScreen(string command);
    void editTaskDescription();
    void editTaskTitle();
    void editBoardTitle();
    void editTaskStage();
    void editTaskRating();

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
    map<string, string> screenMenus;
};

#endif // UI_H