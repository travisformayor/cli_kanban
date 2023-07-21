#include <iostream>
#include <windows.h>
#include <map>
#include <list>

using namespace std;

const WORD TEXT_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const WORD TEXT_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN;

map<string, string> screenMenus = {
    {"Boards", "| up/down: Navigate | enter: Select | n: New Board | d: Delete Board | b: Back | esc: Quit |"},
    {"Board", "| up/down: Navigate | enter: Select Task | d: Delete Task | b: Back | esc: Quit | "},
    {"Task", "Edit | t: Title | d: Description | s: Stage | r: Rated Difficulty | b: Back/Save"}
};

void topMenu(const string& currentScreen) {
    cout << "======================================= Kanban Board =======================================\n";
    cout << screenMenus[currentScreen] << "\n";
    cout << "============================== (Press key to make selection) ===============================\n";
    cout << "\n";
    cout << "                                       | " << currentScreen << " |\n";
    cout << "\n";
}

// set the console text color
void setTextColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// display list with selection highlighted
void displayList(const list<string>& items, int selectedIndex) {
    int index = 0;
    for (const auto& item : items) {
        // 1) set the color
        if (index == selectedIndex) {
            setTextColor(TEXT_GREEN); // highlighted item color
        }
        else {
            setTextColor(TEXT_WHITE); // regular item color
        }
        // 2) print the item
        cout << item << endl;
        index++;
    }
    setTextColor(TEXT_WHITE); // Reset color to white
}

void displayScreen(const string& currentScreen, const list<string>& items, int selectedIndex) {
    system("cls"); // clear the console screen

    topMenu(currentScreen);
    displayList(items, selectedIndex);
}

int main() {
    // test items for ui
    list<string> items = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
    int selectedIndex = 0;

    displayScreen("Boards", items, selectedIndex);

    while (true) {
        // Check the arrow key states
        if (GetAsyncKeyState(VK_UP) & 0x8000) { // Up arrow key
            selectedIndex = static_cast<int>((selectedIndex - 1 + items.size()) % items.size());
            displayScreen("Boards", items, selectedIndex);
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000) { // Down arrow key
            selectedIndex = static_cast<int>((selectedIndex + 1) % items.size());
            displayScreen("Boards", items, selectedIndex);
        }
    }

    return 0;
}
