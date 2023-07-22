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

// set the console text color
void setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void topMenu(const string& currentScreen) {
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[currentScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << currentScreen << " |\n";
    cout << "\n";
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

void createBoard(Database* db, list<Board*>& boards);
void deleteBoard(Database* db, list<Board*>& boards, int selectedIndex);
void refreshBoardNames(const list<Board*>& boards, list<string>& boardNames);
void boardsScreen(Database* db, list<Board*>& boards);
void boardViewScreen(Database* db, Board* board);
void taskScreen(Database* db, Board* board, Task* task);

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
                board->addTask(newTask);
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
                selectedIndex = max(0, static_cast<int>(tasks.size()) - 1);
                displayScreen("Board", taskNames, selectedIndex);
            }
            // 'b' pressed
            else if (ch == 'b' || ch == 'B') {
                break;
            }
            // 'esc' pressed
            else if (ch == 27) {
                exit(0);
            }
        }
    }
}

void taskScreen(Database* db, Board* board, Task* task) {
    // to do: rework this to use a dedicated display task screen function
    string taskDetail = task->getTaskCard();

    displayScreen("Task", { taskDetail }, -1);

    // Listen for keyboard commands
    while (true) {
        if (_kbhit()) {
            int ch = _getch();
            // 't' pressed
            if (ch == 't' || ch == 'T') {
                // to do: make this have a char length limit
                // to do: can the user entry pre-populate with the current version of desc when updating it?
                string newTitle = getUserInput("Enter a new title for the task: ");
                task->setTitle(newTitle);
                // save changes
                db->saveTaskData(*task);
                // update ui
                taskDetail = task->getTaskCard();
                displayScreen("Task", { taskDetail }, 0);
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                // to do: display description with word wrap past certain length
                // to do: can the user entry pre-populate with the current version of desc when updating it?
                string newDescription = getUserInput("Enter a new description for the task: ");
                task->setDescription(newDescription);
                // save changes
                db->saveTaskData(*task);
                // update ui
                taskDetail = task->getTaskCard();
                displayScreen("Task", { taskDetail }, 0);
            }
            // 's' pressed
            else if (ch == 's' || ch == 'S') {
                // to do: make this a hardcoded selection list, not user input
                // to do: can the user entry pre-populate with the current version of desc when updating it?
                string newStage = getUserInput("Enter a new stage for the task: ");
                task->setStage(task->stringToStage(newStage));
                // save changes
                db->saveTaskData(*task);
                // update ui
                taskDetail = task->getTaskCard();
                displayScreen("Task", { taskDetail }, 0);
            }
            // 'r' pressed
            else if (ch == 'r' || ch == 'R') {
                // to do: catch error if they dont event a num 1 - 5. allow re-entry
                // to do: explain in the UI they need to enter a num 1 - 5
                // to do: can the user entry pre-populate with the current version of desc when updating it?
                string newDifficulty = getUserInput("Enter a new difficulty rating for the task: ");
                task->setDifficulty(stoi(newDifficulty));
                // save changes
                db->saveTaskData(*task);
                // update ui
                taskDetail = task->getTaskCard();
                displayScreen("Task", { taskDetail }, 0);
            }
            // 'b' pressed
            else if (ch == 'b' || ch == 'B') {
                break;
            }
            // 'esc' pressed
            else if (ch == 27) {
                exit(0);
            }
        }
    }
}
