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
        // Open DB
        Database db("kanban_db.db");

        UI ui(db); // display modification object

        // loop screen selection
        while (true) {
            // ==== boards screen
            if (ui.getScreen() == "Boards") {
                if (ui.screenChanged()) {
                    // screen just changed to boards list
                    // Load Boards
                    ui.loadBoards();
                    // reset selection index
                    ui.setSelectIndex(0);
                }
                // Display/Update the UI
                ui.displayScreen();
                // Listen for user input (pauses here)
                ui.boardListControls();
            }

            // ==== board view screen
            else if (ui.getScreen() == "BoardView") {
                if (ui.screenChanged()) {
                    // screen just changed to board view
                    // load selected board
                    ui.loadSelectedBoard();
                    // reset selection index
                    ui.setSelectIndex(0);
                }
                // Display/Update the UI
                ui.displayScreen();
                // Listen for user input (pauses here)
                ui.boardViewControls();
            }
            // ==== task view screen
            else if (ui.getScreen() == "TaskView") {
                if (ui.screenChanged()) {
                    // screen just changed to task view
                    // get selected task
                    ui.getSelectedTask();
                    // reset selection index
                    ui.setSelectIndex(0);
                }
                // Display/Update the UI
                ui.displayScreen();
                // Listen for user input (pauses here)
                ui.taskViewControls();
            }
        }
    }
    catch (runtime_error& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1; // return 1 for error
    }

    return 0;
}
