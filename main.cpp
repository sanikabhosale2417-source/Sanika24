/*
 * main.cpp  –  Entry point for the Hybrid Inventory Manager
 *
 * Displays the interactive menu and delegates each choice to the
 * InventoryManager C++ class.
 */

#include "InventoryManager.h"

#include <iostream>
#include <limits>

static void show_menu()
{
    std::cout
        << "\n╔══════════════════════════════╗\n"
        << "║   Hybrid Inventory Manager   ║\n"
        << "╠══════════════════════════════╣\n"
        << "║  1. Add item                 ║\n"
        << "║  2. View item                ║\n"
        << "║  3. Update item              ║\n"
        << "║  4. Delete item              ║\n"
        << "║  5. List all                 ║\n"
        << "║  6. Exit                     ║\n"
        << "╚══════════════════════════════╝\n"
        << "Choice: ";
}

int main()
{
    InventoryManager mgr;
    int choice = 0;

    while (true) {
        show_menu();

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [!] Please enter a number between 1 and 6.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: mgr.addItem();    break;
            case 2: mgr.viewItem();   break;
            case 3: mgr.updateItem(); break;
            case 4: mgr.deleteItem(); break;
            case 5: mgr.listItems();  break;
            case 6:
                std::cout << "Goodbye!\n";
                return 0;
            default:
                std::cout << "  [!] Invalid choice. Enter 1-6.\n";
        }
    }
}
