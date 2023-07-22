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
        UI ui; // display modification object
        ui.setTextColor(TEXT_WHITE);

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
                ui.displayScreen(currScreen, selectedIndex, boardNames);
                // Listen for user input (pauses here)
                ui.navControls(currScreen, selectedIndex, db, boards);
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
                ui.displayScreen(currScreen, selectedIndex, taskNames);
                // Listen for user input (pauses here)
                ui.navControls(currScreen, selectedIndex, db, tasks);
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
                ui.displayScreen("TaskView", -1, { taskDetail });
                // Listen for user input (pauses here)
                ui.taskEditControls(currScreen, selectedIndex, db, selectedTask);
            }
        }
    }
    catch (runtime_error& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1; // return 1 for error
    }

    return 0;
}
