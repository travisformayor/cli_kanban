#include "UI.h"

using namespace std;

UI::UI(Database& db) : db(db) {
    this->selectedIndex = 0;
    this->currScreen = "Boards";

    this->screenMenus = {
        {"Boards", "    | up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | esc: Quit |   "},
        {"Board View", "| up/down: Navigate | enter: Select | c: Create Task | d: Delete Task | t: Edit Board Title | b: Back | esc: Quit | "},
        {"Task View", " | t: Edit Title | d: Edit Description | s: Edit Stage | r: Edit Difficulty Rating | b: Save & Back | esc: Save & Quit |"}
    };

    setTextColor(TEXT_WHITE);
}

void UI::topMenu() {
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[this->currScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << this->currScreen << " |\n";
    cout << "\n";
}

// set the console text color
void UI::setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void UI::setSelectIndex(int index) {
    this->selectedIndex = index;
}

void UI::loadBoards() {
    this->loadedBoards.clear();
    this->loadedBoards = db.loadBoardData();
}

void UI::getSelectedBoard() {
    // find selected board
    Board* boardPtr = this->loadedBoards.begin();
    advance(boardPtr, this->selectedIndex);
    // mark board as selected
    this->selectedBoard = *boardPtr;
    // load all tasks for selected board
    this->loadedTasks.clear();
    this->loadedTasks = db.loadTaskData(this->selectedBoard);
}

void UI::getSelectedTask() {
    // find selected task
    Task* taskPtr = this->loadedTasks.begin();
    advance(taskPtr, this->selectedIndex);
    // mark task as selected
    this->selectedTask = *taskPtr;
}

void UI::displayScreen() {
    system("cls"); // clear the console screen
    topMenu(this->currScreen);

    // load titles list or task details
    list<string> titles;
    if (this->currScreen == "Boards") {
        // display list of boards
        for (Board* board : this->loadedBoards) {
            titles.push_back(board->getTitle());
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "Board View") {
        // display list of tasks on board
        for (Task* task : this->loadedTasks) {
            titles.push_back(task->getTitle());
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "Task View") {
        // display selected task info
        string taskDetails = selectedTask->getTaskCard();
        displayTaskCard(taskDetails);
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

void UI::displayTaskCard(string taskDetails) {
    // to do: expand on the task card view screen. multiline, word wrap, election highlight, etc
    cout << taskDetails << endl;
}

void UI::addNewBoard() {
    string newBoardTitle = getUserInput("Enter a title for the new board: ");
    Board newBoard = new Board(newBoardTitle);
    // save board to db
    this->db->saveBoardData(newBoard);
    // reload list of boards
    this->loadedBoards.clear();
    this->loadedBoards = this->db->loadBoardData();
}

void UI::addNewTask() {
    string newTaskTitle = getUserInput("Enter a title for the new task: ");
    Task newTask = new Task(newTaskTitle);
    // add task to board and save to db
    this->db->saveTaskData(this->selectedBoard, newTask);
    // reload tasks for selected board
    this->loadedTasks.clear();
    this->loadedTasks = this->db.loadTaskData(this->selectedBoard);
}

void UI::removeSelectedBoard() {
    // find the selected board
    Board* boardPtr = this->loadedBoards.begin();
    advance(boardPtr, this->selectedIndex);
    // delete board and deallocate memory for pointer
    this->db->deleteBoard(*boardPtr);
    delete* boardPtr;
    // reload list of boards
    this->loadedBoards.clear();
    this->loadedBoards = this->db->loadBoardData();
    // Adjust selected index if at end of list
    this->selectedIndex = max(0, static_cast<int>(this->loadedBoards.size()) - 1);
}

void UI::removeSelectedTask() {
    // find the selected task
    Task* taskPtr = this->loadedTasks.begin();
    advance(taskPtr, this->selectedIndex);
    // delete task and deallocate memory for pointer
    this->db->deleteTask(*taskPtr);
    delete* taskPtr;
    // reload tasks for selected board
    this->loadedTasks.clear();
    this->loadedTasks = this->db.loadTaskData(this->selectedBoard);
    // Adjust selected index if at end of list
    this->selectedIndex = max(0, static_cast<int>(this->loadedTasks.size()) - 1);
}

string UI::getUserInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;

    // to do: catch and exceptions around user input
}

// to do sort: 
// - sort at db level: stage then id for tasks, title then id for boards
// - show how clears and reloads from db every time there is a change to the list of tasks or boards
// - show advance() command, now keeping everything sorted correctly lets this simple selection method work
// to do search: 
// - where clause in db searches for only tasks associated with the selected board. relationship key needed for this to work

void UI::keyboardListen() {
    bool keyPressed = false;

    while (true) {
        if (_kbhit()) {
            keyPressed = true;
            int ch = _getch();

            switch (ch) {
            case 224: // potential arrow key
                ch = _getch();
                switch (ch) {
                case 72: // up arrow
                    changeSelector(-1); // move selector up 1
                    break;
                case 80: // down arrow
                    changeSelector(1); // move selector down 1
                    break;
                }
                break;
            case 13: // enter key. move to selected screen
                changeScreen("enter");
                break;
            case 'b': // back to previous screen
                changeScreen("back");
                break;
            case 'c': // create
                if (this->currScreen == "Boards") {
                    addNewBoard();
                }
                else if (this->currScreen == "Board View") {
                    addNewTask();
                }
                break;
            case 'd': // delete or edit task description
                if (this->currScreen == "Boards") {
                    removeSelectedBoard();
                }
                else if (this->currScreen == "Board View") {
                    removeSelectedTask();
                }
                else if (this->currScreen == "Task View") {
                    editTaskDescription();
                }
                break;
            case 'r': // edit task difficulty rating
                if (this->currScreen == "Task View") {
                    editTaskRating();

                }
                break;
            case 's': // edit task stage
                if (this->currScreen == "Task View") {
                    editTaskStage();
                }
                break;
            case 't': // edit title
                if (this->currScreen == "Board View") {
                    editBoardTitle();
                }
                else if (this->currScreen == "Task View") {
                    editTaskTitle();
                }
                break;
            case 27: // 'esc', quit program
                if (this->currScreen == "Task View") {
                    // save any changes to task first
                    this->db->saveTaskData(selectedTask);
                }
                exit(0);
            }
        }

        if (keyPressed) {
            // exit while loop to allow display to react to key press
            break;
        }
    }
}

void UI::changeSelector(int direction) {
    // move selector up or down, wrapping around if at end or start
    if (direction == 1 || direction == -1) {
        this->selectedIndex = static_cast<int>((this->selectedIndex + direction + items.size()) % items.size());
    }
}

void UI::changeScreen(string command) {
    // 1. change screen
    if (command == "enter") {
        // Based on current screen, move forward to next screen
        if (this->currScreen == "Boards") {
            this->currScreen = "Board View";
        }
        else if (this->currScreen == "Board View") {
            this->currScreen = "Task View";
        }
    }
    else if (command == "back") {
        // Based on current screen, move back to previous screen
        if (this->currScreen == "Task View") {
            // save edits first
            this->db->saveTaskData(selectedTask);
            // exit to board view
            // move screens
            this->currScreen = "Board View";
        }
        else if (this->currScreen == "Board View") {
            this->currScreen = "Boards";
        }
    }

    // 2. load items for new screen and reset selector position
    if (this->currScreen == "Boards") {
        ui.loadBoards();
        ui.setSelectIndex(0);
    }
    else if (this->currScreen == "Board View") {
        ui.getSelectedBoard();
        ui.setSelectIndex(0);
    }
    else if (this->currScreen == "Task View") {
        ui.getSelectedTask();
        ui.setSelectIndex(0);
    }

    // 3. while loops, display refreshes in main()
}

void UI::editTaskDescription() {
    // to do: display description with word wrap past certain length
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newDescription = getUserInput("Enter a new description for the task: ");
    this->selectedTask->setDescription(newDescription);
}

void UI::editTaskTitle() {
    // to do: make this have a char length limit
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newTitle = getUserInput("Enter a new title for the task: ");
    this->selectedTask->setTitle(newTitle);
}

void UI::editBoardTitle() {
    // to do: make this have a char length limit
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newTitle = getUserInput("Enter a new title for the task: ");
    this->selectedBoard->setTitle(newTitle);
}

void UI::editTaskStage() {
    // to do: make this a hardcoded selection list, not user input
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newStage = getUserInput("Enter a new stage for the task: ");
    this->selectedTask->setStage(task->stringToStage(newStage));
}

void UI::editTaskRating() {
    // to do: catch error if they dont event a num 1 - 5. allow re-entry
    // to do: explain in the UI they need to enter a num 1 - 5
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newDifficulty = getUserInput("Enter a new difficulty rating for the task: ");
    this->selectedTask->setDifficulty(stoi(newDifficulty));
}