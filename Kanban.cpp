#include <iostream>
#include <windows.h>
#include <map>
#include <list>

using namespace std;

map<string, string> screenMenus = {
    {"Users", "| up/down: Navigate | enter: Select | n: New User | d: Delete User | esc: Quit |"},
    {"Boards", "| up/down: Navigate | enter: Select | n: New Board | d: Delete Board | b: Back | esc: Quit |"},
    {"Board", "| up/down: Navigate | enter: Select Task | d: Delete Task | b: Back | esc: Quit | "},
    {"Task", "Edit | t: Title | d: Description | u: Assigned User | s: Stage | r: Rated Difficulty | b: Back"}
};

void topMenu(const string& currentScreen) {
    cout << "================================ Kanban Board ================================\n";
    cout << screenMenus[currentScreen] << "\n";
    cout << "======================= (Press key to make selection) ========================\n";
    cout << "\n";
    cout << "| " << currentScreen << " |\n";
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
        if (index == selectedIndex) {
            setTextColor(FOREGROUND_INTENSITY | FOREGROUND_BLUE); // Highlighted item color
        }
        else {
            setTextColor(FOREGROUND_INTENSITY | FOREGROUND_GREEN); // Regular item color
        }

        cout << item << endl;
        index++;
    }
}

void displayScreen(const string& currentScreen, const list<string>& items, int selectedIndex) {
    system("cls"); // clear the console screen

    topMenu(currentScreen);
    displayList(items, selectedIndex);
}

int main() {
    list<string> items = { "Item 1", "Item 2", "Item 3", "Item 4", "Item 5" };
    int selectedIndex = 0;

    displayScreen("Users", items, selectedIndex);

    while (true) {
        // Check the arrow key states
        if (GetAsyncKeyState(VK_UP) & 0x8000) { // Up arrow key
            selectedIndex = static_cast<int>((selectedIndex - 1 + items.size()) % items.size());
            displayScreen("Users", items, selectedIndex);
        }
        else if (GetAsyncKeyState(VK_DOWN) & 0x8000) { // Down arrow key
            selectedIndex = static_cast<int>((selectedIndex + 1) % items.size());
            displayScreen("Users", items, selectedIndex);
        }
    }

    return 0;
}
