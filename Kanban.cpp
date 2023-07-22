#include <iostream>
#include <windows.h>
#include <conio.h>
#include <map>
#include <list>
#include "Database.h"
#include "Board.h"
#include "Task.h"

using namespace std;

const WORD TEXT_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD TEXT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;

map<string, string> screenMenus = {
    {"Boards", "    | up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | esc: Quit |   "},
    {"BoardView", "| up/down: Navigate | enter: Select | c: Create Task | d: Delete Task | b: Back | esc: Quit | "},
    {"Task", " Select to Edit | t: Title | d: Description | s: Stage | r: Rated Difficulty | b: Back/Save "}
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
        cout << "    - " << item << endl;
        index++;
    }
    setTextColor(TEXT_WHITE); // Reset color to white
}

void displayScreen(const string& currentScreen, const list<string>& items, int selectedIndex) {
    system("cls"); // clear the console screen

    topMenu(currentScreen);
    displayList(items, selectedIndex);
}

string getUserInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

int main() {
    try {
        setTextColor(TEXT_WHITE);

        // Open DB
        Database db("kanban_db.db");
        // Load existing Boards
        list<Board*> boards = db.loadBoardData();

        // Load the UI
        boardsScreen(&db, boards);
    }
    catch (runtime_error& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1; // return 1 for error
    }

    return 0;
}

void createBoard(Database* db, list<Board*>& boards) {
    string newBoardName = getUserInput("Enter a name for the new board: ");
    Board* newBoard = new Board(newBoardName);
    db->saveBoardData(*newBoard);
    boards = db->loadBoardData();
}

void deleteBoard(Database* db, list<Board*>& boards, int selectedIndex) {
    auto it = boards.begin();
    advance(it, selectedIndex);
    db->deleteBoard(**it);
    delete* it; // delete the memory occupied by the board
    boards.erase(it); // remove the pointer from the list
    // Adjust the selected index if we're at the end of the list
    selectedIndex = max(0, static_cast<int>(boards.size()) - 1);
}

// Refresh the boardNames list
void refreshBoardNames(const list<Board*>& boards, list<string>& boardNames) {
    boardNames.clear();
    for (Board* board : boards) {
        boardNames.push_back(board->getName());
    }
}

void boardsScreen(Database* db, list<Board*>& boards) {
    int selectedIndex = 0;

    list<string> boardNames;
    refreshBoardNames(boards, boardNames);

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
            // 'enter' pressed
            else if (ch == 13) {
                auto it = boards.begin();
                advance(it, selectedIndex);
                boardViewScreen(db, *it);
            }
            // 'c' pressed
            else if (ch == 'c' || ch == 'C') {
                createBoard(db, boards);
                refreshBoardNames(boards, boardNames);
                displayScreen("Boards", boardNames, selectedIndex);
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                deleteBoard(db, boards, selectedIndex);
                refreshBoardNames(boards, boardNames);
                displayScreen("Boards", boardNames, selectedIndex);
            }
            // 'esc' pressed
            else if (ch == 27) {
                exit(0);
            }
        }
    }
}

void boardViewScreen(Database* db, Board* board) {
    int selectedIndex = 0;
    // Load existing Tasks
    list<Task*> tasks = board->getTasks();
    list<string> taskNames;
    for (Task* task : tasks) {
        taskNames.push_back(task->getName());
    }
    // Load the UI
    displayScreen("Board", taskNames, selectedIndex);

    // Listen for keyboard commands
    while (true) {
        if (_kbhit()) {
            int ch = _getch();

            // if arrow key
            if (ch == 224) {
                ch = _getch();
                switch (ch) {
                case 72: // up arrow key
                    selectedIndex = static_cast<int>((selectedIndex - 1 + taskNames.size()) % taskNames.size());
                    displayScreen("Board", taskNames, selectedIndex);
                    break;
                case 80: // down arrow key
                    selectedIndex = static_cast<int>((selectedIndex + 1) % taskNames.size());
                    displayScreen("Board", taskNames, selectedIndex);
                    break;
                }
            }
            // 'c' pressed
            else if (ch == 'c' || ch == 'C') {
                string newTaskName = getUserInput("Enter a name for the new task: ");
                Task* newTask = new Task(newTaskName);
                db->saveTaskData(*board, *newTask);
                tasks = board->getTasks(); // reload the task list
                taskNames.clear();
                for (Task* task : tasks) {
                    taskNames.push_back(task->getName());
                }
                displayScreen("Board", taskNames, selectedIndex);
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                auto it = tasks.begin();
                advance(it, selectedIndex);
                db->deleteTaskData(*board, **it);
                delete* it; // delete the memory occupied by the task
                tasks.erase(it); // remove the pointer from the list
                taskNames.clear();
                for (Task* task : tasks) {
                    taskNames.push_back(task->getName());
                }
                // Adjust the selected index if we're at the end of the list
                if (selectedIndex >= static_cast<int>(taskNames.size())) {
                    selectedIndex = max(0, static_cast<int>(taskNames.size()) - 1);
                }
                displayScreen("Board", taskNames, selectedIndex);
            }
            // 'enter' pressed
            else if (ch == 13) {
                auto it = tasks.begin();
                advance(it, selectedIndex);
                taskScreen(db, board, *it);
                // Reload the tasks and taskNames as they might have changed
                tasks = board->getTasks();
                taskNames.clear();
                for (Task* task : tasks) {
                    taskNames.push_back(task->getName());
                }
                displayScreen("Board", taskNames, selectedIndex);
            }
            // 'b' or 'esc' pressed
            else if (ch == 'b' || ch == 'B' || ch == 27) {
                break; // exit to the previous screen
            }
        }
    }
}

void taskScreen(Database* db, Board* board, Task* task) {
    string input;

    displayScreen("Task", { "Title: " + task->getTitle(), "Description: " + task->getDescription(),
                           "Stage: " + task->getStage(), "Rated Difficulty: " + task->getRatedDifficulty() }, -1);

    // Listen for keyboard commands
    while (true) {
        if (_kbhit()) {
            int ch = _getch();

            // 't' pressed
            if (ch == 't' || ch == 'T') {
                input = getUserInput("Enter a new title for the task: ");
                task->setTitle(input);
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                input = getUserInput("Enter a new description for the task: ");
                task->setDescription(input);
            }
            // 's' pressed
            else if (ch == 's' || ch == 'S') {
                input = getUserInput("Enter a new stage for the task: ");
                task->setStage(input);
            }
            // 'r' pressed
            else if (ch == 'r' || ch == 'R') {
                input = getUserInput("Enter a new rated difficulty for the task: ");
                task->setRatedDifficulty(input);
            }
            // 'b' or 'esc' pressed
            else if (ch == 'b' || ch == 'B' || ch == 27) {
                db->updateTaskData(*board, *task);
                break; // exit to the previous screen
            }

            displayScreen("Task", { "Title: " + task->getTitle(), "Description: " + task->getDescription(),
                                   "Stage: " + task->getStage(), "Rated Difficulty: " + task->getRatedDifficulty() }, -1);
        }
    }
}
