#include "Database.h"
#include "UI.h"
#include "Board.h"
#include "Task.h"
#include <iostream>
#include <map>
#include <list>

using namespace std;

int main() {
    try {
        setTextColor(TEXT_WHITE);

        string currScreen = "Boards";
        string prevScreen = "";

        list<Board*> boards;
        list<Task*> tasks;
        Task* selectedTask;

        list<string> boardNames;
        list<string> taskNames;

        // Open DB
        Database db("kanban_db.db");

        // loop screen selection
        while (true) {
            // ==== boards screen
            if (currScreen == "Boards") {
                if (prevScreen != currScreen) {
                    // screen just changed to boards list
                    // Load existing Boards
                    boards.clear();
                    boards = db.loadBoardData();

                    // reset selection index
                    int selectedIndex = 0;
                    // mark screen as previously shown for next loop
                    prevScreen = currScreen;
                }

                // load/reload board names
                boardNames.clear();
                for (Board* board : boards) {
                    boardNames.push_back(board->getName());
                }
                // Load the UI
                displayScreen(currScreen, selectedIndex, boardNames);
                // Listen for user input (pauses here)
                navControls(currScreen, selectedIndex, boards);
            }

            // ==== board view screen
            else if (currScreen == "BoardView") {
                if (prevScreen != currScreen) {
                    // screen just changed to board view
                    // determine which board was selected
                    auto it = boards.begin();
                    advance(it, selectedIndex);
                    // Load existing selected board Tasks
                    tasks.clear();
                    tasks = db.loadTaskData(*it);
                    // reset selection index
                    int selectedIndex = 0;
                    // mark screen as previously shown for next loop
                    prevScreen = currScreen;
                }

                taskNames.clear();
                for (Task* task : tasks) {
                    taskNames.push_back(task->getName());
                }
                // Load the UI
                displayScreen(currScreen, selectedIndex, taskNames);
                // Listen for user input (pauses here)
                navControls(currScreen, selectedIndex, tasks);
            }
            // ==== task view screen
            else if (currScreen == "TaskView") {
                if (prevScreen != currScreen) {
                    // screen just changed to task view
                    // determine which task was selected
                    auto it = tasks.begin();
                    advance(it, selectedIndex);
                    selectedTask = *it;
                    // reset selection index
                    int selectedIndex = 0;
                    // mark screen as previously shown for next loop
                    prevScreen = currScreen;
                }

                // get updated task info
                taskDetail = selectedTask->getTaskCard();
                // Load the UI
                displayScreen("TaskView", -1, { taskDetail });
                // Listen for user input (pauses here)
                taskEditControls(currScreen, selectedIndex, selectedTask);
            }
        }
    }
    catch (runtime_error& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1; // return 1 for error
    }

    return 0;
}

void navControls(string& screen, int& selectedIndex, variant<list<Board*>, list<Task*>>& items) {
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
                    createBoard(db, items);
                }
                else if (screen == "BoardView") {
                    createTask(db, items);
                }
                break;
            }
            // 'd' pressed
            else if (ch == 'd' || ch == 'D') {
                if (screen == "Boards") {
                    deleteBoard(db, items, selectedIndex);
                }
                else if (screen == "BoardView") {
                    deleteTask(db, items, selectedIndex);
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

void taskEditControls(string& screen, int& selectedIndex, Task* task) {
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
