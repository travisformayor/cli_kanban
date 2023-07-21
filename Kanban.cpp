#include <iostream>
#include <windows.h>
#include <map>
#include <list>
#include <conio.h>
#include "Database.h"
#include "Board.h"

using namespace std;

const WORD TEXT_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD TEXT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;

map<string, string> screenMenus = {
    {"Boards", "| up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | b: Back | esc: Quit |"},
    {"Board", "| up/down: Navigate | enter: Select | c: Create Task | d: Delete Task | b: Back | esc: Quit | "},
    {"Task", "Edit | t: Title | d: Description | s: Stage | r: Rated Difficulty | b: Back/Save"}
};

void topMenu(const string& currentScreen) {
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[currentScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << currentScreen << " |\n";
    cout << "\n";
}

// set the console text color
void setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// display list with selection highlighted
void displayList(const list<string>& items, int selectedIndex) {
    int index = 0;
    for (const auto& item : items) {
        // set the color
        if (index == selectedIndex) {
            setTextColor(TEXT_GREEN); // highlighted item color
        }
        else {
            setTextColor(TEXT_WHITE); // regular item color
        }
        // print the item
        cout << item << endl;
        index++;
    }
    setTextColor(TEXT_WHITE); // Reset color to white
}

void displayScreen(const string& currentScreen, const list<string>& items, int selectedIndex) {
    system("cls"); // clear the console screen

    topMenu(currentScreen);
    displayList(items, selectedIndex);
}

string getUserInput() {
    cout << "Enter a name for the new board: ";
    string boardName;
    getline(cin, boardName);
    return boardName;
}

int main() {
    try {
        int selectedIndex = 0;

        // Open DB
        Database db("kanban.db");
        // Load existing Boards
        list<Board*> boards = db.loadBoardData();
        list<string> boardNames;
        for (Board* board : boards) {
            boardNames.push_back(board->getName());
        }

        // Load the UI
        displayScreen("Boards", boardNames, selectedIndex);

        // Listen for keyboard commands
        while (true) {
            if (_kbhit()) {
                int ch = _getch();

                // if arrow key
                if (ch == 224) {
                    ch = _getch();
                    switch (ch) {
                    case 72: // up arrow key
                        selectedIndex = static_cast<int>((selectedIndex - 1 + boardNames.size()) % boardNames.size());
                        displayScreen("Boards", boardNames, selectedIndex);
                        break;
                    case 80: // down arrow key
                        selectedIndex = static_cast<int>((selectedIndex + 1) % boardNames.size());
                        displayScreen("Boards", boardNames, selectedIndex);
                        break;
                    }
                }
                // 'c' pressed
                else if (ch == 'c' || ch == 'C') {
                    string newBoardName = getUserInput();
                    Board* newBoard = new Board(newBoardName);
                    db.saveBoardData(*newBoard);
                    boards = db.loadBoardData();
                    boardNames.clear();
                    for (Board* board : boards) {
                        boardNames.push_back(board->getName());
                    }
                    displayScreen("Boards", boardNames, selectedIndex);
                }
                // 'd' pressed
                else if (ch == 'd' || ch == 'D') {
                    auto it = boards.begin();
                    advance(it, selectedIndex);
                    db.deleteBoard(**it);
                    delete* it; // delete the memory occupied by the board
                    boards.erase(it); // remove the pointer from the list
                    boardNames.clear();
                    for (Board* board : boards) {
                        boardNames.push_back(board->getName());
                    }
                    // Adjust the selected index if we're at the end of the list
                    if (selectedIndex >= static_cast<int>(boardNames.size())) {
                        selectedIndex = max(0, static_cast<int>(boardNames.size()) - 1);
                    }
                    displayScreen("Boards", boardNames, selectedIndex);
                }
            }
        }
    }
    catch (runtime_error& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1; // return 1 for error
    }

    return 0;
}
