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
        // open DB
        Database db("kanban_db.db");
        // create display object
        UI ui(db);

        // load boards, set user selector position
        ui.reloadBoards();
        ui.setSelectIndex(0);

        // loop screen refresh and user command listening
        while (true) {
            // Display/Update the UI
            ui.displayScreen();
            // Listen for user input (pauses here until key press)
            ui.keyboardListen();
        }
    }
    catch (runtime_error& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1; // return 1 for error
    }

    return 0;
}
