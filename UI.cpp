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

void UI::setTextColor(WORD color) {
    // set the console text color
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void UI::setSelectIndex(int index) {
    this->selectedIndex = index;
}

void UI::reloadBoards() {
    this->loadedBoards.clear();
    this->loadedBoards = this->db.loadBoardData();
}

void UI::reloadBoardTasks() {
    // reload tasks for currently selected board
    // check if a board is selected
    if (this->selectedBoardPtr != nullptr) {
        // reload tasks from DB
        this->selectedBoardPtr->setTasks(this->db.loadTaskData(*this->selectedBoardPtr));
    }
    else {
        cout << "Select a board before loading tasks." << endl;
    }
}

void UI::findSelectedBoard() {
    // find selected board
    list<Board*>::iterator boardIter = this->loadedBoards.begin();
    advance(boardIter, this->selectedIndex);
    // point selected to the correct board
    this->selectedBoardPtr = *boardIter; // dereference iterator returns Board*
}

void UI::findSelectedTask() {
    // find selected task. access tasks from the selected board
    list<Task*>::iterator taskIter = this->selectedBoardPtr->tasks.begin();
    advance(taskIter, this->selectedIndex);
    // point selected to the correct task
    this->selectedTaskPtr = *taskIter; // dereference iterator returns Task*
}

void UI::displayScreen() {
    system("cls"); // clear the console screen

    // top menu
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[this->currScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << this->currScreen << " |\n";
    cout << "\n";

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
        // display list of tasks for selected board
        for (Task* task : this->selectedBoardPtr->getTasks()) {
            titles.push_back(task->getTitle());
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "Task View") {
        // display selected task info
        string taskDetails = this->selectedTaskPtr->getTaskCard();
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
    Board newBoard(newBoardTitle);
    // save board to db
    this->db.saveBoardData(newBoard);
    // reload list of boards
    reloadBoards();
}

void UI::addNewTask() {
    string newTaskTitle = getUserInput("Enter a title for the new task: ");
    Task newTask(newTaskTitle, *this->selectedBoardPtr);
    // save task to db
    this->db.saveTaskData(newTask);
    // reload tasks for selected board
    reloadBoardTasks();
}

void UI::deleteSelectedBoard() {
    // find the selected board
    list<Board*>::iterator boardIter = this->loadedBoards.begin();
    advance(boardIter, this->selectedIndex);
    // delete board from database
    this->db.deleteBoard(**boardIter); // deref iterator = board ptr, then deref ptr
    //  deallocate memory
    delete* boardIter; // deref iterator returns board*
    // reload list of boards
    reloadBoards();
    // fix selected index if was at end of list
    this->selectedIndex = max(0, static_cast<int>(this->loadedBoards.size()) - 1);
}


void UI::deleteSelectedTask() {
    // find the selected task
    list<Task*>::iterator taskIter = this->selectedBoardPtr->getTasks().begin();
    advance(taskIter, this->selectedIndex);
    // delete task from db
    this->db.deleteTask(**taskIter); // deref iterator = task ptr, then deref prt
    // deallocate memory
    delete* taskIter; // deref iterator returns task*
    // reload tasks for selected board
    reloadBoardTasks();
    // fix selected index if was at end of list
    this->selectedIndex = max(0, static_cast<int>(this->selectedBoardPtr->getTasks().size()) - 1);
}

string UI::getUserInput(const string& prompt) {
    string input;

    try {
        cout << prompt;
        getline(cin, input);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw runtime_error("Error reading input");
        }
    }
    catch (const runtime_error& e) {
        cerr << "An error occurred: " << e.what() << endl;
        // reset input to not return bad data
        input = "";
    }

    return input;
}

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
                    moveSelector(-1); // move selector up 1
                    break;
                case 80: // down arrow
                    moveSelector(1); // move selector down 1
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
                    this->db.saveTaskData(*this->selectedTaskPtr);
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

void UI::moveSelector(int direction) {
    // move selector up or down, wrapping around if at end or start

    // find length of list on the screen
    int listSize;
    if (this->currScreen == "Boards") {
        listSize = this->loadedBoards.size();
    }
    else if (this->currScreen == "Board View") {
        listSize = this->selectedBoardPtr->getTasks().size();
    }
    else if (this->currScreen == "Task View") {
        listSize = 0; // task view does not use arrow keys
    }

    // only move selector for Boards or Board View screen
    if (this->currScreen == "Boards" || this->currScreen == "Board View") {
        if (direction == 1 || direction == -1) {
            this->selectedIndex = static_cast<int>((this->selectedIndex + direction + itemsSize) % itemsSize);
        }
    }
    else {
        this->selectedIndex = 0;
    }
}

void UI::changeScreen(string command) {
    // change screen
    if (command == "enter") {
        // move forward to next screen
        if (this->currScreen == "Boards") {
            // find which board was selected and load its tasks
            findSelectedBoard();
            reloadBoardTasks();
            this->currScreen = "Board View";
        }
        else if (this->currScreen == "Board View") {
            // find which task was selected
            findSelectedTask();
            this->currScreen = "Task View";
        }
    }
    else if (command == "back") {
        // move back to previous screen
        if (this->currScreen == "Task View") {
            // save task edits
            this->db.saveTaskData(*this->selectedTaskPtr);
            // task board still selected. reload the boards tasks
            reloadBoardTasks();
            this->currScreen = "Board View";
        }
        else if (this->currScreen == "Board View") {
            // load list of all boards
            reloadBoards();
            this->currScreen = "Boards";
        }
    }
    // reset selector position
    this->setSelectIndex(0);
}

void UI::editBoardTitle() {
    // to do: make this have a char length limit
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newTitle = getUserInput("Enter a new title for the task: ");
    this->selectedBoardPtr->setTitle(newTitle);
}

void UI::editTaskTitle() {
    // to do: make this have a char length limit
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newTitle = getUserInput("Enter a new title for the task: ");
    this->selectedTaskPtr->setTitle(newTitle);
}

void UI::editTaskDescription() {
    // to do: display description with word wrap past certain length
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newDescription = getUserInput("Enter a new description for the task: ");
    this->selectedTaskPtr->setDescription(newDescription);
}

void UI::editTaskStage() {
    // to do: make this a hardcoded selection list, not user input
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newStage = getUserInput("Enter a new stage for the task: ");
    this->selectedTaskPtr->setStage(Task::stringToStage(newStage));
}

void UI::editTaskRating() {
    // to do: catch error if they dont event a num 1 - 5. allow re-entry
    // to do: explain in the UI they need to enter a num 1 - 5
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    string newDifficulty = getUserInput("Enter a new difficulty rating for the task: ");
    this->selectedTaskPtr->setDifficulty(stoi(newDifficulty));
}
