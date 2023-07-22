#include "UI.h"

// set the console text color
void Display::setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void Display::topMenu(const string& currentScreen) {
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[currentScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << currentScreen << " |\n";
    cout << "\n";
}

void Display::displayScreen(const string& currentScreen, int& selectedIndex, const list<string>& items) {
    system("cls"); // clear the console screen
    topMenu(currentScreen);
    displayList(items, selectedIndex);
}

// display list with selection highlighted
void Display::displayList(const list<string>& items, int& selectedIndex) {
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

string Display::getUserInput(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

void Display::createBoard(Database* db, list<Board*>& boards) {
    string newBoardName = getUserInput("Enter a name for the new board: ");
    Board* newBoard = new Board(newBoardName);
    db->saveBoardData(*newBoard);
    boards = db->loadBoardData();
}

void Display::deleteBoard(Database* db, list<Board*>& boards, int& selectedIndex) {
    // find the selected board
    auto it = boards.begin();
    advance(it, selectedIndex);
    db->deleteBoard(**it);
    delete* it; // delete the memory occupied by the board
    boards.erase(it); // remove the pointer from the list
    // Adjust the selected index if we're at the end of the list
    selectedIndex = max(0, static_cast<int>(boards.size()) - 1);
}

void Display::createTask(Database* db, list<Task*>& tasks) {
    string newTaskName = getUserInput("Enter a name for the new task: ");
    Task* newTask = new Task(newTaskName);
    board->addTask(newTask);
    db->saveTaskData(*board, *newTask);
    // to do: do i need to reload the tasks from the db and save them to the task list here?
}

void Display::deleteTask(Database* db, list<Task*>& tasks, int& selectedIndex) {
    // find the selected task
    auto it = tasks.begin();
    advance(it, selectedIndex);
    db->deleteTask(**it);
    delete* it; // delete the memory occupied by the task
    tasks.erase(it); // remove the pointer from the list
    // Adjust the selected index if we're at the end of the list
    selectedIndex = max(0, static_cast<int>(tasks.size()) - 1);
}
