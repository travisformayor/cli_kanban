#include "UI.h"

using namespace std;

UI::UI(Database& db) : db(db) {
    this->selectedIndex = 0;
    this->currScreen = "Boards";
    this->prevScreen = "";

    this->screenMenus = {
        {"Boards", "    | up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | esc: Quit |   "},
        {"BoardView", "| up/down: Navigate | enter: Select | c: Create Task | d: Delete Task | b: Back | esc: Quit | "},
        {"Task", " Select to Edit | t: Title | d: Description | s: Stage | r: Rated Difficulty | b: Back/Save "}
    };

    setTextColor(TEXT_WHITE);
}

// set the console text color
void UI::setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void UI::setSelectIndex(int index) {
    this->selectedIndex = index;
}

string UI::getScreen() {
    return this->currScreen;
}

bool UI::screenChanged() {
    // check if screen just changed
    // if so, return true and set to not changed
    if (this->currScreen != this->prevScreen) {
        // mark as previously shown for next time
        this->prevScreen = this->currScreen;
        return true;
    }
    else {
        return false;
    }
}

void UI::loadBoards() {
    this->loadedBoards.clear();
    this->loadedBoards = db.loadBoardData();
}

void UI::loadSelectedBoard() {
    // find selected board
    Board* ptr = this->loadedBoards.begin();
    advance(ptr, this->selectedIndex);
    // save selected board
    this->selectedBoard = *ptr;
    // load all tasks for selected board
    this->loadedTasks.clear();
    this->loadedTasks = db.loadTaskData(this->selectedBoard);
}

void UI::getSelectedTask() {
    // find selected task
    // to do: sorting the order of the tasks may break this
    Task* ptr = this->loadedTasks.begin();
    advance(ptr, this->selectedIndex);
    // save selected task
    this->selectedTask = *ptr;
}

void UI::displayScreen() {
    system("cls"); // clear the console screen
    topMenu(this->currScreen);

    // load titles
    list<string> titles;
    if (this->currScreen == "Boards") {
        for (Board* board : this->loadedBoards) {
            titles.push_back(board->getTitle());
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "BoardView") {
        for (Task* task : this->loadedTasks) {
            titles.push_back(task->getTitle());
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "TaskView") {
        // get task info
        string taskDetails = selectedTask->getTaskCard();
        displayTaskInfo(taskDetails);
    }
}

void UI::displayTitles(list<string>& titles) {
    // display titles with selected title highlighted
    int index = 0;
    for (const string title : titles) {
        if (index == this->selectedIndex) {
            setTextColor(TEXT_GREEN); // highlighted item color
        }
        else {
            setTextColor(TEXT_WHITE); // regular item color
        }
        // print title
        cout << "    * " << title << endl;
        index++;
    }
    setTextColor(TEXT_WHITE); // Reset color to white
}

void UI::topMenu() {
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[this->currScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << this->currScreen << " |\n";
    cout << "\n";
}

void UI::addBoard() {
    string newBoardTitle = getUserInput("Enter a title for the new board: ");
    Board* newBoard = new Board(newBoardTitle);
    this->db->saveBoardData(*newBoard);
    this->loadedBoards = this->db->loadBoardData();
}

void UI::removeBoard() {
    // find the selected board
    Board* ptr = this->loadedBoards.begin();
    advance(ptr, this->selectedIndex);
    // delete board
    this->db->deleteBoard(*ptr);
    delete* ptr; // delete the memory occupied by the board
    this->loadedBoards.erase(it); // remove the pointer from the list
    // Adjust selected index if at end of list
    this->selectedIndex = max(0, static_cast<int>(this->loadedBoards.size()) - 1);
}

void UI::addTask() {
    string newTaskTitle = getUserInput("Enter a title for the new task: ");
    Task* newTask = new Task(newTaskTitle);
    // add task to board and save
    this->db->saveTaskData(this->selectedBoard, *newTask);
    // reload tasks for selected board
    this->loadedTasks.clear();
    this->loadedTasks = db.loadTaskData(this->selectedBoard);
}

void UI::removeTask() {
    // find the selected task
    Task* ptr = this->loadedTasks.begin();
    advance(ptr, this->selectedIndex);
    // delete task
    this->db->deleteTask(*ptr);
    delete* it; // delete the memory occupied by the task
    this->loadedTasks.erase(ptr); // remove the pointer from the list
    // Adjust selected index if at end of list
    this->selectedIndex = max(0, static_cast<int>(tasks.size()) - 1);
}

string UI::getUserInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

void UI::navControls(variant<list<Board*>, list<Task*>>& items) {
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
                    this->selectedIndex = static_cast<int>((this->selectedIndex - 1 + items.size()) % items.size());
                    break;
                case 80: // down arrow key
                    this->selectedIndex = static_cast<int>((this->selectedIndex + 1) % items.size());
                    break;
                }
            }
            // 'enter' pressed
            else if (ch == 13) {
                if (this->currScreen == "Boards") {
                    this->currScreen = "BoardView";
                }
                else if (this->currScreen == "BoardView") {
                    this->currScreen = "TaskView";
                }
                break;
            }
            // 'c' pressed
            else if (ch == 'c' || ch == 'C') {
                if (this->currScreen == "Boards") {
                    addBoard(items);
                }
                else if (this->currScreen == "BoardView") {
                    addTask(items);
                }
                break;
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                if (this->currScreen == "Boards") {
                    removeBoard(items);
                }
                else if (this->currScreen == "BoardView") {
                    removeTask(items);
                }
                break;
            }
            // to do: add sort and search controls

            // 'b' pressed
            else if (ch == 'b' || ch == 'B') {
                if (this->currScreen == "BoardView") {
                    this->currScreen = "Boards";
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

void UI::taskEditControls(Task* task) {
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
                this->db->saveTaskData(*task);
                // exit to board view
                this->currScreen = "BoardView";
                break;
            }
            // 'esc' pressed
            else if (ch == 27) {
                exit(0);
            }
        }
    }
}