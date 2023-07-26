#include "UI.h"

using namespace std;

UI::UI(Database& db) : db(db) {
    this->selectedIndex = 0;
    this->activeBoardId = 0;
    this->activeTaskId = 0;
    this->currScreen = "Boards";
    this->screenMenus = {
        {"Boards", "| up/down: Navigate | enter: Select | c: Create Board | d: Delete Board | esc: Quit |"},
        {"Board View", "| up/down: Navigate | enter: Select | c: Create Task | d: Delete Task | t: Edit Board Title | b: Back | esc: Quit |"},
        {"Task View", " | t: Edit Title | d: Edit Description | s: Edit Stage | r: Edit Difficulty Rating | b: Save & Back | esc: Save & Quit |"}
    };
    this->screenWidth = 120; // length of the longest command menu
    string leftPadding(10, ' ');
    string headerPadding(30, '=');
    this->padL = leftPadding;
    this->padHeader = headerPadding;
    setTextColor(TEXT_WHITE);
}

UI::~UI() {
    // Deallocate and clear lists and pointers
    for (auto board : this->loadedBoards) {
        delete board;
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
    '     | Boards |
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

    // output centered menu
    cout << topPadding << menuTop << topPadding << endl;
    cout << menuPadding << menu << menuPadding << endl;
    cout << bottomPadding << menuBottom << bottomPadding << "=" << endl;
    cout << endl;

    // the following code displays a selectable list of board or task titles
    map<string, list<string>> titles;
    if (this->currScreen == "Boards") {
        // display title of board list view
        cout << this->padL << "| Boards |" << endl << endl;

        // display list of boards
        if (this->loadedBoards.size() > 0) {
            for (Board* board : this->loadedBoards) {
                titles["Board"].push_back(board->getTitle());
            }
        }
        else {
            cout << this->padL << "[Create first board with 'c' command]" << endl;;
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "Board View" && this->activeBoardId != 0) {
        // display title of board view, ie the name of the board
        string boardTitle = getBoardById(this->activeBoardId)->getTitle();
        cout << this->padL << "| Board Name: " << boardTitle << " |" << endl << endl;

        // display list of tasks for active board
        list<Task*>& tasks = getBoardById(this->activeBoardId)->getTasks();
        if (tasks.size() > 0) {
            // add each stage with empty list so each gets displayed
            // prefixed numbers keep sorting correct in a map
            titles["1. To Do"] = list<string>();
            titles["2. In Progress"] = list<string>();
            titles["3. Done"] = list<string>();

            for (Task* task : tasks) {
                // add task to each stage list
                switch (task->getStage()) {
                case Stage::ToDo:
                    titles["1. To Do"].push_back(task->getTitle());
                    break;
                case Stage::InProgress:
                    titles["2. In Progress"].push_back(task->getTitle());
                    break;
                case Stage::Done:
                    titles["3. Done"].push_back(task->getTitle());
                    break;
                }
            }
        }
        else {
            cout << "    ======= To Do =======" << this->padHeader << "\n" << endl;
            cout << "\n    ======= In Progress =" << this->padHeader << "\n" << endl;
            cout << "\n    ======= Done ========" << this->padHeader << "\n" << endl;
            cout << endl;
            cout << this->padL << "[Create first task with 'c' command]" << endl;;
        }
        displayTitles(titles);
    }
    else if (this->currScreen == "Task View" && this->activeBoardId != 0 && this->activeTaskId != 0) {
        // display selected task info
        Task* taskPtr = getBoardById(this->activeBoardId)->getTaskById(this->activeTaskId);
        displayTaskCard(taskPtr);
    }

    // Print any Alert messages to the user from the last loop
    if (this->userAlerts.size() > 0) {
        cout << endl;
        for (const string& alert : this->userAlerts) {
            cout << this->padL << alert << endl;
        }
        this->userAlerts.clear();
    }

    cout << endl;
}

void UI::displayTitles(map<string, list<string>>& titles) {
    // display titles with selected title highlighted and stage name titles for tasks
    bool shownToDo = false;
    bool shownInProgress = false;
    bool shownDone = false;

    int index = 0;
    for (const auto& [key, value] : titles) { // loop map keys
        // display stages once per presorted group of tasks with the stage
        if (key == "1. To Do" && !shownToDo) {
            cout << "    ======= To Do =======" << this->padHeader << "\n" << endl;
            shownToDo = true;
        }
        else if (key == "2. In Progress" && !shownInProgress) {
            cout << "\n    ======= In Progress =" << this->padHeader << "\n" << endl;
            shownInProgress = true;
        }
        else if (key == "3. Done" && !shownDone) {
            cout << "\n    ======= Done ========" << this->padHeader << "\n" << endl;
            shownDone = true;
        }

        for (const string& title : value) { // loop titles in list for the key
            if (index == this->selectedIndex) {
                setTextColor(TEXT_GREEN); // highlighted item color
            }
            else {
                setTextColor(TEXT_WHITE); // regular item color
            }
            // print title
            cout << this->padL << "* " << title << endl;
            setTextColor(TEXT_WHITE); // reset item color regular

            index++;
        }
    }
}

void UI::displayTaskCard(Task* task) {
    // display card of task information
    cout << endl;
    // print Title
    cout << this->padL << "Title: " << setTextColor(TEXT_GREEN) << task->getTitle() << setTextColor(TEXT_WHITE) << endl;

    // print Description
    cout << this->padL << "Description: " << setTextColor(TEXT_GREEN) << task->getDescription() << setTextColor(TEXT_WHITE) << endl;

    // print Stage
    cout << this->padL << "Stage: " << setTextColor(TEXT_GREEN) << this->stageToString(task->getStage()) << setTextColor(TEXT_WHITE) << endl;

    // print Rated Difficulty
    cout << this->padL << "Rated Difficulty: " << setTextColor(TEXT_GREEN) << task->getDifficultyRating() << setTextColor(TEXT_WHITE) << endl;
}

string UI::getUserInput(const string& prompt) {
    string input;

    try {
        cout << this->padL << prompt;
        getline(cin, input);

        if (cin.fail()) {
            cin.clear();
            cin.ignore((numeric_limits<streamsize>::max)(), '\n');
            throw invalid_argument("Invalid input.");
        }
    }
    catch (const invalid_argument&) {
        // reset input to not return bad data & rethrow exception upward
        input = "";
        throw;
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
                case 72: moveSelector(-1); break; // up arrow. move selector up 1
                case 80: moveSelector(1); break; // down arrow. move selector down 1
                }
                break;
            case 13: changeScreen("enter"); break; // enter key. move to selected screen
            case 'b': changeScreen("back"); break; // back to previous screen
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
            case 27: exit(0); // 'esc', quit program
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
        if (this->currScreen == "Boards" && this->loadedBoards.size() > 0) {
            // how to move selector on board screen
            int listSize = static_cast<int>(this->loadedBoards.size());
            this->selectedIndex = ((this->selectedIndex + direction + listSize) % listSize);
        }
        else if (this->currScreen == "Board View" && this->activeBoardId != 0) {
            list<Task*>& tasks = getBoardById(this->activeBoardId)->getTasks();
            if (tasks.size() > 0) {

                // how to move selector on board view screen
                int listSize = static_cast<int>(tasks.size());
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
        else if (this->currScreen == "Board View" && this->activeBoardId != 0) {
            // does the board have tasks?
            if (getBoardById(this->activeBoardId)->getTasks().size() > 0) {
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

Board* UI::getBoardById(int id) {
    for (Board* board : this->loadedBoards) {
        if (board->getId() == id) {
            return board;
        }
    }
    throw runtime_error("Board with id " + to_string(id) + " not found.");
    return nullptr;
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
    if (this->activeBoardId != 0) {
        // reload tasks from DB
        Board* board = getBoardById(this->activeBoardId);
        board->setTasks(this->db.loadTaskData(*board));
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
        this->activeBoardId = (*boardIter)->getId(); // deref iterator returns Board*. set active board id
    }
    else {
        addAlert("Missing boards.");
    }
    this->selectedIndex = 0;
}

void UI::findSelectedTask() {
    // find selected task. access tasks from the active board
    // check for active board with tasks
    if (this->activeBoardId != 0) {
        list<Task*>& tasks = getBoardById(this->activeBoardId)->getTasks();
        if (tasks.size() > 0) {
            // set selected task as active task
            list<Task*>::iterator taskIter = tasks.begin();
            advance(taskIter, this->selectedIndex);

            if (taskIter != tasks.end()) { // a result was found
                this->activeTaskId = (*taskIter)->getId(); // deref iterator returns Task*, get id.
            }
            else {
                addAlert("Missing task.");
            }
        }
        else {
            addAlert("No tasks.");
        }
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
    if (this->activeBoardId != 0) {
        // Add new task to board
        string newTaskTitle = getUserInput("Enter a title for the new task: ");
        Task newTask(newTaskTitle, *getBoardById(this->activeBoardId));
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
        // reload list of boards
        reloadBoards();
        // fix selected index if was at end of list
        this->selectedIndex = min(this->selectedIndex, static_cast<int>(this->loadedBoards.size()) - 1);
    }
    else {
        addAlert("No boards to delete.");
        this->selectedIndex = 0;
    }
}

void UI::deleteSelectedTask() {
    // check if there is an active board, then get its tasks
    if (this->activeBoardId != 0) {
        list<Task*>& tasks = getBoardById(this->activeBoardId)->getTasks();

        if (tasks.size() > 0) {
            // find selected task
            list<Task*>::iterator taskIter = tasks.begin();
            advance(taskIter, this->selectedIndex);
            // delete task from DB
            this->db.deleteTask(**taskIter); // deref iterator gets task ptr, then deref ptr
            // reload tasks for active board
            reloadBoardTasks();
            // fix selected index if at end of list
            this->selectedIndex = min(this->selectedIndex, static_cast<int>(tasks.size()) - 1);
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
    if (this->activeBoardId != 0) {
        try {
            string newTitle = getUserInput("Enter a new title for the board: ");

            Board* activeBoard = getBoardById(this->activeBoardId);
            activeBoard->setTitle(newTitle);
            // save board to db and reload board list
            this->db.saveBoardData(*activeBoard);
            reloadBoards();
            // reload tasks in board view (reloadBoards drops tasks lists)
            reloadBoardTasks();
        }
        catch (invalid_argument& e) {
            // catch invalid_argument from setTitle or getUserInput
            // note: db errors return runtime error, which is caught elsewhere
            addAlert(string(e.what()) + " 't' to retry.");
        }
    }
    else {
        addAlert("Missing active board.");
    }
}

void UI::editTaskTitle() {
    if (this->activeBoardId != 0 && this->activeTaskId != 0) {
        Task* activeTask = getBoardById(this->activeBoardId)->getTaskById(this->activeTaskId);

        try {
            string newTitle = getUserInput("Enter a new title for the task: ");
            activeTask->setTitle(newTitle);
            // save task to db and reload task list
            this->db.saveTaskData(*activeTask);
            reloadBoardTasks();
        }
        catch (invalid_argument& e) {
            // catch invalid_argument from setTitle or getUserInput
            // note: db errors return runtime error, which is caught elsewhere
            addAlert(string(e.what()) + " 't' to retry.");
        }
    }
    else {
        addAlert("Missing active task.");
    }
}

void UI::editTaskDescription() {
    if (this->activeBoardId != 0 && this->activeTaskId != 0) {
        Task* activeTask = getBoardById(this->activeBoardId)->getTaskById(this->activeTaskId);

        try {
            string newDescription = getUserInput("Enter a new description for the task: ");
            activeTask->setDescription(newDescription);
            // save task to db and reload task list
            this->db.saveTaskData(*activeTask);
            reloadBoardTasks();
        }
        catch (invalid_argument& e) {
            // catch invalid_argument from setDescription or getUserInput
            // note: db errors return runtime error, which is caught elsewhere
            addAlert(string(e.what()) + " 'd' to retry.");
        }
    }
    else {
        addAlert("Missing active task.");
    }
}

void UI::editTaskStage() {
    if (this->activeBoardId != 0 && this->activeTaskId != 0) {
        Task* activeTask = getBoardById(this->activeBoardId)->getTaskById(this->activeTaskId);

        try {
            string options = this->padL + "  1. To Do\n" + this->padL + "  2. In Progress\n" + this->padL + "  3. Done\n" + this->padL + "  ";
            string strStage = getUserInput("Select a new stage for the task. Enter a number 1 - 3.\n" + options);
            Stage newStage;
            try {
                // attempt convert to int and stage selection
                int stageNum = stoi(strStage);
                switch (stageNum) {
                case 1: newStage = Stage::ToDo; break;
                case 2: newStage = Stage::InProgress; break;
                case 3: newStage = Stage::Done; break;
                default: throw invalid_argument("");
                }
            }
            catch (invalid_argument&) {
                // catches failed stoi() convert or switch default
                throw invalid_argument("Invalid stage selection. Enter 1, 2 or 3.");
            }
            // update task and save to db
            activeTask->setStage(newStage, false);
            this->db.saveTaskData(*activeTask);
            // reload board tasks from db
            reloadBoardTasks();
        }
        catch (invalid_argument& e) {
            // catch invalid_argument from above try or setStage
            // note: db errors return runtime error, which is caught elsewhere
            addAlert(string(e.what()) + " 's' to retry.");
        }
    }
    else {
        addAlert("Missing active task.");
    }
}

void UI::editTaskRating() {
    if (this->activeBoardId != 0 && this->activeTaskId != 0) {
        Task* activeTask = getBoardById(this->activeBoardId)->getTaskById(this->activeTaskId);

        try {
            string strRating = getUserInput("Enter difficulty rating for the task (number 1 - 5): ");
            int newRating;
            try {
                newRating = stoi(strRating);
            }
            catch (invalid_argument&) {
                // catches failed stoi() convert
                throw invalid_argument("Enter a number between 1 and 5.");
            }
            // update task and save to db
            activeTask->setDifficulty(newRating);
            this->db.saveTaskData(*activeTask);
            // reload board tasks from db
            reloadBoardTasks();
        }
        catch (invalid_argument& e) {
            // catch invalid_argument from above try or setDifficulty
            // note: db errors return runtime error, which is caught elsewhere
            addAlert(string(e.what()) + " 'r' to retry.");
        }
    }
    else {
        addAlert("Missing active task.");
    }
}
