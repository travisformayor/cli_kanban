#include "Database.h"
#include "UI.h"
#include "Board.h"
#include "Task.h"

using namespace std;

// to do: code
// - board view lists grouped stage titles
// - char limit on titles and descriptions
// - improve card view. padding, multiline, word wrap, election highlight, etc
// - pre-populate edit's for title and description with current value

// to do: docs
// - add method comments in header files
// - update classUML and other diagrams
// - update proposal

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
