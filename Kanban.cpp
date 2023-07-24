#include "Database.h"
#include "UI.h"
#include "Board.h"
#include "Task.h"

using namespace std;

// to do: 
// - handle for first time load, when there are no boards or tasks
// - in this case, hitting the arrow key or enter crashes the program
// - there is no list to size for the selectedIndex
// - there is no list for the enter to open
// - so, handle by checking for null on the screen list
// - then any attempt to act on the list or move the selector, protect with message

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
        cerr << "A runtime error occurred: " << e.what() << endl;
        return 1; // return 1 for error
    }
    catch (exception& e) {
        cerr << "An exception occurred: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "An unknown error occurred." << endl;
        return 1;
    }

    return 0;
}
