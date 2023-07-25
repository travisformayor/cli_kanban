#include "UI.h"

using namespace std;

UI::UI(Database& db) : db(db) {
    this->selectedIndex = 0;
    this->currScreen = "Boards";
    this->screenMenus = {
        {"Boards", "| up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | esc: Quit |"},
        {"Board View", "| up/down: Navigate | enter: Select | c: Create Task | d: Delete Task | t: Edit Board Title | b: Back | esc: Quit |"},
        {"Task View", " | t: Edit Title | d: Edit Description | s: Edit Stage | r: Edit Difficulty Rating | b: Save & Back | esc: Save & Quit |"}
    };
    this->screenWidth = 120; // length of the longest command menu
    setTextColor(TEXT_WHITE);
}

UI::~UI() {
    // Deallocate and clear lists and pointers
    for (auto board : this->loadedBoards) {
        delete board;
        // also deletes activeBoardPtr and activeTaskPtr memory
    }
    this->loadedBoards.clear();
}

void UI::setTextColor(WORD color) {
    // set the console text color
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void UI::setSelectIndex(int index) {
    this->selectedIndex = index;
}

void UI::displayScreen() {
    /* The following code outputs a menu like this, but always centered:
    '=================================== Kanban Board ====================================
    '| up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | esc: Quit |
    '=========================== (Press key to make selection) ===========================
    '                                     | Boards |
    '
    '     * Item one
    '     * Item two
    */

    system("cls"); // clear the screen between each refresh
    // top menu
    // center the top title
    string menuTop = " Kanban Board ";
    string topPadding((this->screenWidth - menuTop.length()) / 2, '=');
    // center the current menu
    string menu = this->screenMenus[this->currScreen];
    string menuPadding((this->screenWidth - menu.length()) / 2, ' ');
    // center menu bottom
    string menuBottom = " (Press key to make selection) ";
    string bottomPadding((this->screenWidth - menuBottom.length()) / 2, '=');
    // center screen name
    string screenName = this->currScreen;
    string namePadding((this->screenWidth - screenName.length() - 4) / 2, ' ');

    // output centered menu
    cout << topPadding << menuTop << topPadding << "\n";
    cout << menuPadding << menu << menuPadding << "\n";
    cout << bottomPadding << menuBottom << bottomPadding << "\n";
    cout << "\n";
    cout << namePadding << "| " << screenName << " |" << "\n";
    cout << "\n";

    // Output active board or task title
    if (currScreen == "Board View" && this->activeBoardPtr != nullptr) {
        string boardTitle = this->activeBoardPtr->getTitle();
        string padding((this->screenWidth - boardTitle.length() - 7) / 2, ' ');
        cout << padding << "Board: " << boardTitle << "\n";
    }
    else if (currScreen == "Task View" && this->activeTaskPtr != nullptr) {
        string taskTitle = this->activeTaskPtr->getTitle();
        string padding((this->screenWidth - taskTitle.length() - 6) / 2, ' ');
        cout << padding << "Task: " << taskTitle << "\n";
    }
    cout << "\n";

    // the following code displays a selectable list of board or task titles
    list<string> titles;
    if (this->currScreen == "Boards") {
        // display list of boards
        if (this->loadedBoards.size() > 0) {
            for (Board* board : this->loadedBoards) {
                titles.push_back(board->getTitle());
            }
        }
        else {
            cout << "      [Create first board with 'c' command]" << endl;;
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "Board View") {
        // display list of tasks for active board
        if (this->activeBoardPtr != nullptr && this->activeBoardPtr->getTasks().size() > 0) {
            for (Task* task : this->activeBoardPtr->getTasks()) {
                titles.push_back(task->getTitle());
            }
        }
        else {
            cout << "      [Create first task with 'c' command]" << endl;;
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "Task View") {
        // display selected task info
        if (this->activeTaskPtr != nullptr) {
            string taskDetails = this->activeTaskPtr->getTaskCard();
            displayTaskCard(taskDetails);
        }
    }

    // Print any message to user from the last loop
    if (this->userAlerts.size() > 0) {
        cout << endl;
        for (const string& alert : this->userAlerts) {
            cout << alert << endl;
        }
        this->userAlerts.clear();
    }

    cout << endl;
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

string UI::getUserInput(const string& prompt) {
    string input;

    try {
        cout << prompt;
        getline(cin, input);

        if (cin.fail()) {
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            throw invalid_argument("Invalid input.");
        }
    }
    catch (const invalid_argument& e) {
        addAlert("debug test");
        // reset input to not return bad data
        input = "";
        // rethrow the exception upward
        throw e;
    }

    return input;
}

void UI::addAlert(const string& alert) {
    this->userAlerts.push_back(alert);
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
                    deleteSelectedBoard();
                }
                else if (this->currScreen == "Board View") {
                    deleteSelectedTask();
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
    // move selector by 1 on Boards or Board View screens, wrapping around at end or start
    if (direction == 1 || direction == -1) {
        if (this->currScreen == "Boards") {
            // how to move selector on board screen
            if (this->loadedBoards.size() > 0) {
                int listSize = static_cast<int>(this->loadedBoards.size());
                this->selectedIndex = ((this->selectedIndex + direction + listSize) % listSize);
            }
        }
        else if (this->currScreen == "Board View") {
            // how to move selector on board view screen
            if (this->activeBoardPtr != nullptr && this->activeBoardPtr->getTasks().size() > 0) {
                int listSize = static_cast<int>(this->activeBoardPtr->getTasks().size());
                this->selectedIndex = ((this->selectedIndex + direction + listSize) % listSize);
            }
        }
    }
}

void UI::changeScreen(string command) {
    // change screen
    if (command == "enter") {
        // move forward to next screen
        if (this->currScreen == "Boards") {
            // are there boards listed?
            if (this->loadedBoards.size() > 0) {
                // find which board was selected and load its tasks
                findSelectedBoard();
                reloadBoardTasks();
                // change screen
                this->currScreen = "Board View";
            }
            else {
                addAlert("No board selected.");
            }
        }
        else if (this->currScreen == "Board View") {
            // does the board have tasks?
            if (this->activeBoardPtr != nullptr && this->activeBoardPtr->getTasks().size() > 0) {
                // find which task was selected
                findSelectedTask();
                // change screen
                this->currScreen = "Task View";
            }
            else {
                addAlert("No task selected.");
            }
        }
    }
    else if (command == "back") {
        // move back to previous screen
        if (this->currScreen == "Task View") {
            // update the tasks listed for the still active board
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

void UI::reloadBoards() {
    // deallocate and clear current list
    for (auto board : this->loadedBoards) {
        delete board;
    }
    this->loadedBoards.clear();
    // reload list from db
    this->loadedBoards = this->db.loadBoardsList();
}

void UI::reloadBoardTasks() {
    // reload tasks for currently active board
    // check if a board is selected
    if (this->activeBoardPtr != nullptr) {
        // reload tasks from DB
        this->activeBoardPtr->setTasks(this->db.loadTaskData(*this->activeBoardPtr));
    }
    else {
        addAlert("Select a board before loading tasks.");
    }
}

void UI::findSelectedBoard() {
    // check if there are boards
    if (this->loadedBoards.size() > 0) {
        // find selected board, set active
        list<Board*>::iterator boardIter = this->loadedBoards.begin();
        advance(boardIter, this->selectedIndex);
        this->activeBoardPtr = *boardIter; // deref iterator returns Board*. set active.
    }
    else {
        addAlert("Missing boards.");
    }
    this->selectedIndex = 0;
}

void UI::findSelectedTask() {
    // find selected task. access tasks from the active board
    // check for active board with tasks
    if (this->activeBoardPtr != nullptr && this->activeBoardPtr->getTasks().size() > 0) {
        // set selected task as active task
        list<Task*>::iterator taskIter = this->activeBoardPtr->getTasks().begin();
        advance(taskIter, this->selectedIndex);
        this->activeTaskPtr = *taskIter; // deref iterator returns Task*. set active.
    }
    else {
        addAlert("Missing tasks.");
    }
    this->selectedIndex = 0;
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
    // check there is an active board
    if (this->activeBoardPtr != nullptr) {
        // Add new task to board
        string newTaskTitle = getUserInput("Enter a title for the new task: ");
        Task newTask(newTaskTitle, *this->activeBoardPtr);
        // save task to db
        this->db.saveTaskData(newTask);
        // reload tasks for active board
        reloadBoardTasks();
    }
    else {
        addAlert("Select a board before adding a task.");
        this->selectedIndex = 0;
    }
}

void UI::deleteSelectedBoard() {
    // check if there are boards
    if (this->loadedBoards.size() > 0) {
        // find the selected board
        list<Board*>::iterator boardIter = this->loadedBoards.begin();
        advance(boardIter, this->selectedIndex);
        // delete board from DB
        this->db.deleteBoard(**boardIter); // deref iterator gets board ptr, then deref ptr
        // reload list of boards (also deallocates board pointer)
        reloadBoards();
        // fix selected index if was at end of list
        this->selectedIndex = max(0, static_cast<int>(this->loadedBoards.size()) - 1);
    }
    else {
        addAlert("No boards to delete.");
        this->selectedIndex = 0;
    }
}

void UI::deleteSelectedTask() {
    // check if there is an active board
    if (this->activeBoardPtr != nullptr) {
        // check if there are tasks to select
        if (this->activeBoardPtr->getTasks().size() > 0) {
            // find selected task 
            list<Task*>::iterator taskIter = this->activeBoardPtr->getTasks().begin();
            advance(taskIter, this->selectedIndex);
            // delete task from DB
            this->db.deleteTask(**taskIter); // deref iterator gets task ptr, then deref prt
            // reload tasks for active board (also deallocates task pointer)
            reloadBoardTasks();
            // fix selected index if at end of list
            this->selectedIndex = max(0, static_cast<int>(this->activeBoardPtr->getTasks().size()) - 1);
        }
        else {
            addAlert("No tasks to delete.");
            this->selectedIndex = 0;
        }
    }
    else {
        addAlert("Missing active board.");
        this->selectedIndex = 0;
    }
}

void UI::editBoardTitle() {
    // to do: make this have a char length limit
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    if (this->activeBoardPtr != nullptr) {
        string newTitle = getUserInput("Enter a new title for the task: ");
        this->activeBoardPtr->setTitle(newTitle);
        // save board to db and reload board list
        this->db.saveBoardData(*this->activeBoardPtr);
        reloadBoards();
    }
    else {
        addAlert("Missing active board.");
    }
}

void UI::editTaskTitle() {
    // to do: make this have a char length limit
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    if (this->activeTaskPtr != nullptr) {
        string newTitle = getUserInput("Enter a new title for the task: ");
        this->activeTaskPtr->setTitle(newTitle);
        // save task to db and reload task list
        this->db.saveTaskData(*this->activeTaskPtr);
        reloadBoardTasks();
    }
    else {
        addAlert("Missing active task.");
    }
}

void UI::editTaskDescription() {
    // to do: display description with word wrap past certain length
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    if (this->activeTaskPtr != nullptr) {
        string newDescription = getUserInput("Enter a new description for the task: ");
        this->activeTaskPtr->setDescription(newDescription);
        // save task to db and reload task list
        this->db.saveTaskData(*this->activeTaskPtr);
        reloadBoardTasks();
    }
    else {
        addAlert("Missing active task.");
    }
}

void UI::editTaskStage() {
    // to do: make this a hardcoded selection list, not user input
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    if (this->activeTaskPtr != nullptr) {
        string newStage = getUserInput("Enter a new stage for the task: ");
        this->activeTaskPtr->setStage(Task::stringToStage(newStage), false);
        // save task to db and reload task list
        this->db.saveTaskData(*this->activeTaskPtr);
        reloadBoardTasks();
    }
    else {
        addAlert("Missing active task.");
    }
}

void UI::editTaskRating() {
    // to do: explain in the UI they need to enter a num 1 - 5
    // to do: can the user entry pre-populate with the current version of desc when updating it?
    if (this->activeTaskPtr != nullptr) {
        try {
            string strRating = getUserInput("Enter a new difficulty rating for the task: ");
            int newRating;
            try {
                newRating = stoi(strRating);
            }
            catch (invalid_argument& e) {
                throw invalid_argument("Enter a number between 1 and 5.");
            }
            this->activeTaskPtr->setDifficulty(newRating);
            this->db.saveTaskData(*this->activeTaskPtr);
            reloadBoardTasks();
        }
        catch (invalid_argument& e) {
            // catch invalid_argument from isNumber or setDifficulty
            // note: db errors return runtime error, which is caught elsewhere
            addAlert("Issue: " + string(e.what()));
        }
    }
    else {
        addAlert("Missing active task.");
    }
}
