#include "UI.h"

// set the console text color
void UI::setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void UI::topMenu(const string& currentScreen) {
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[currentScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << currentScreen << " |\n";
    cout << "\n";
}

void UI::displayScreen(const string& currentScreen, int& selectedIndex, const list<string>& items) {
    system("cls"); // clear the console screen
    topMenu(currentScreen);
    displayList(items, selectedIndex);
}

// display list with selection highlighted
void UI::displayList(const list<string>& items, int& selectedIndex) {
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

void UI::addBoard(Database& db, list<Board*>& boards) {
    string newBoardName = getUserInput("Enter a name for the new board: ");
    Board* newBoard = new Board(newBoardName);
    db->saveBoardData(*newBoard);
    boards = db->loadBoardData();
}

void UI::removeBoard(Database& db, list<Board*>& boards, int& selectedIndex) {
    // find the selected board
    auto it = boards.begin();
    advance(it, selectedIndex);
    db->deleteBoard(**it);
    delete* it; // delete the memory occupied by the board
    boards.erase(it); // remove the pointer from the list
    // Adjust the selected index if we're at the end of the list
    selectedIndex = max(0, static_cast<int>(boards.size()) - 1);
}

void UI::addTask(Database& db, list<Task*>& tasks) {
    string newTaskName = getUserInput("Enter a name for the new task: ");
    Task* newTask = new Task(newTaskName);
    board->addTask(newTask);
    db->saveTaskData(*board, *newTask);
    // to do: do i need to reload the tasks from the db and save them to the task list here?
}

void UI::removeTask(Database& db, list<Task*>& tasks, int& selectedIndex) {
    // find the selected task
    auto it = tasks.begin();
    advance(it, selectedIndex);
    db->deleteTask(**it);
    delete* it; // delete the memory occupied by the task
    tasks.erase(it); // remove the pointer from the list
    // Adjust the selected index if we're at the end of the list
    selectedIndex = max(0, static_cast<int>(tasks.size()) - 1);
}

string UI::getUserInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

void UI::navControls(string& screen, int& selectedIndex, Database& db, variant<list<Board*>, list<Task*>>& items) {
    // Controls for Boards List and Board View
    // Listen for keyboard commands
    while (true) {
        if (_kbhit()) {
            int ch = _getch();
            // if arrow key
            if (ch == 224) {
                ch = _getch();
                switch (ch) {
                case 72: // up arrow key
                    selectedIndex = static_cast<int>((selectedIndex - 1 + items.size()) % items.size());
                    break;
                case 80: // down arrow key
                    selectedIndex = static_cast<int>((selectedIndex + 1) % items.size());
                    break;
                }
            }
            // 'enter' pressed
            else if (ch == 13) {
                if (screen == "Boards") {
                    screen = "BoardView";
                }
                else if (screen == "BoardView") {
                    screen = "TaskView";
                }
                break;
            }
            // 'c' pressed
            else if (ch == 'c' || ch == 'C') {
                if (screen == "Boards") {
                    addBoard(db, items);
                }
                else if (screen == "BoardView") {
                    addTask(db, items);
                }
                break;
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                if (screen == "Boards") {
                    removeBoard(db, items, selectedIndex);
                }
                else if (screen == "BoardView") {
                    removeTask(db, items, selectedIndex);
                }
                break;
            }
            // to do: add sort and search controls

            // 'b' pressed
            else if (ch == 'b' || ch == 'B') {
                if (screen == "BoardView") {
                    screen = "Boards";
                }
                break;
            }
            // 'esc' pressed
            else if (ch == 27) {
                exit(0);
            }
        }
    }
}

void UI::taskEditControls(string& screen, int& selectedIndex, Database& db, Task* task) {
    // Controls for Task Edit View
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
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                // to do: display description with word wrap past certain length
                // to do: can the user entry pre-populate with the current version of desc when updating it?
                string newDescription = getUserInput("Enter a new description for the task: ");
                task->setDescription(newDescription);
            }
            // 's' pressed
            else if (ch == 's' || ch == 'S') {
                // to do: make this a hardcoded selection list, not user input
                // to do: can the user entry pre-populate with the current version of desc when updating it?
                string newStage = getUserInput("Enter a new stage for the task: ");
                task->setStage(task->stringToStage(newStage));
            }
            // 'r' pressed
            else if (ch == 'r' || ch == 'R') {
                // to do: catch error if they dont event a num 1 - 5. allow re-entry
                // to do: explain in the UI they need to enter a num 1 - 5
                // to do: can the user entry pre-populate with the current version of desc when updating it?
                string newDifficulty = getUserInput("Enter a new difficulty rating for the task: ");
                task->setDifficulty(stoi(newDifficulty));

            }
            // 'b' pressed
            else if (ch == 'b' || ch == 'B') {
                // save changes
                db->saveTaskData(*task);
                // exit to board view
                screen = "BoardView";
                break;
            }
            // 'esc' pressed
            else if (ch == 27) {
                exit(0);
            }
        }
    }
}