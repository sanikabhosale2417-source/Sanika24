/*
 * InventoryManager.cpp  –  C++ layer for the Hybrid Inventory Manager
 *
 * Wraps the C backend (inventory.h / inventory.c) in a clean class.
 * Uses std::vector and std::sort as required.
 */

#include "InventoryManager.h"
#include "inventory.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

/* ------------------------------------------------------------------ */
/* Formatting helpers                                                   */
/* ------------------------------------------------------------------ */

static void print_separator()
{
    std::cout << std::string(70, '-') << '\n';
}

static void print_header()
{
    print_separator();
    std::cout
        << std::left
        << std::setw(6)  << "ID"
        << std::setw(42) << "Name"
        << std::setw(10) << "Qty"
        << std::setw(12) << "Price"
        << '\n';
    print_separator();
}

static void print_item(const Item &it)
{
    std::cout
        << std::left
        << std::setw(6)  << it.id
        << std::setw(42) << it.name
        << std::setw(10) << it.quantity
        << std::fixed << std::setprecision(2)
        << std::setw(12) << it.price
        << '\n';
}

/* ------------------------------------------------------------------ */
/* Input-validation utilities                                           */
/* ------------------------------------------------------------------ */

/** Read an integer from stdin, re-prompting on bad input. */
static int read_int(const std::string &prompt)
{
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [!] Invalid number. Please try again.\n";
    }
}

/** Read a float from stdin, re-prompting on bad input. */
static float read_float(const std::string &prompt)
{
    float v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return v;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  [!] Invalid number. Please try again.\n";
    }
}

/** Read a non-empty string from stdin, re-prompting if blank. */
static std::string read_nonempty(const std::string &prompt)
{
    std::string s;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, s);
        if (!s.empty()) return s;
        std::cout << "  [!] Name cannot be empty. Please try again.\n";
    }
}

/* ------------------------------------------------------------------ */
/* Validated field readers                                              */
/* ------------------------------------------------------------------ */

static int validated_id(const std::string &prompt)
{
    int id;
    while (true) {
        id = read_int(prompt);
        if (id > 0) return id;
        std::cout << "  [!] ID must be a positive integer.\n";
    }
}

static int validated_quantity(const std::string &prompt)
{
    int q;
    while (true) {
        q = read_int(prompt);
        if (q >= 0) return q;
        std::cout << "  [!] Quantity must be 0 or greater.\n";
    }
}

static float validated_price(const std::string &prompt)
{
    float p;
    while (true) {
        p = read_float(prompt);
        if (p >= 0.0f) return p;
        std::cout << "  [!] Price must be 0 or greater.\n";
    }
}

/* ------------------------------------------------------------------ */
/* InventoryManager public methods                                      */
/* ------------------------------------------------------------------ */

void InventoryManager::addItem()
{
    std::cout << "\n=== Add Item ===\n";
    Item it{};
    it.id       = validated_id      ("  ID       : ");
    std::string nm = read_nonempty  ("  Name     : ");
    it.quantity = validated_quantity("  Quantity : ");
    it.price    = validated_price   ("  Price    : ");
    it.is_deleted = 0;

    /* Copy name safely */
    nm.copy(it.name, MAX_NAME_LEN - 1);
    it.name[nm.size() < MAX_NAME_LEN ? nm.size() : MAX_NAME_LEN - 1] = '\0';

    if (add_item(it))
        std::cout << "  [OK] Item " << it.id << " added successfully.\n";
    else
        std::cout << "  [!] Failed – ID " << it.id << " already exists.\n";
}

void InventoryManager::viewItem()
{
    std::cout << "\n=== View Item ===\n";
    int id = validated_id("  Enter ID : ");

    Item it{};
    if (get_item(id, &it)) {
        print_header();
        print_item(it);
        print_separator();
    } else {
        std::cout << "  [!] Item " << id << " not found or has been deleted.\n";
    }
}

void InventoryManager::updateItem()
{
    std::cout << "\n=== Update Item ===\n";
    int id = validated_id("  Enter ID to update : ");

    /* Verify the item exists first */
    Item existing{};
    if (!get_item(id, &existing)) {
        std::cout << "  [!] Item " << id << " not found or has been deleted.\n";
        return;
    }

    std::cout << "  Current record:\n";
    print_header();
    print_item(existing);
    print_separator();
    std::cout << "  Enter new values (press Enter to keep existing name if "
                 "prompt is left blank for non-name fields just enter the same "
                 "value):\n";

    Item updated{};
    updated.id         = id;
    updated.is_deleted = 0;

    std::string nm = read_nonempty  ("  New Name     : ");
    updated.quantity   = validated_quantity("  New Quantity : ");
    updated.price      = validated_price   ("  New Price    : ");

    nm.copy(updated.name, MAX_NAME_LEN - 1);
    updated.name[nm.size() < MAX_NAME_LEN ? nm.size() : MAX_NAME_LEN - 1] = '\0';

    if (update_item(id, &updated))
        std::cout << "  [OK] Item " << id << " updated.\n";
    else
        std::cout << "  [!] Update failed.\n";
}

void InventoryManager::deleteItem()
{
    std::cout << "\n=== Delete Item ===\n";
    int id = validated_id("  Enter ID to delete : ");

    if (delete_item(id))
        std::cout << "  [OK] Item " << id << " deleted.\n";
    else
        std::cout << "  [!] Item " << id << " not found or already deleted.\n";
}

void InventoryManager::listItems()
{
    std::cout << "\n=== List All Items ===\n";

    const int MAX = 1024;
    std::vector<Item> buf(MAX);

    int n = list_items(buf.data(), MAX);
    buf.resize(static_cast<size_t>(n));

    if (n == 0) {
        std::cout << "  (no active items found)\n";
        return;
    }

    /* Sort by ID (change comparator to sort by name if desired) */
    std::sort(buf.begin(), buf.end(),
              [](const Item &a, const Item &b) { return a.id < b.id; });

    print_header();
    for (const auto &it : buf)
        print_item(it);
    print_separator();
    std::cout << "  Total active items: " << n << '\n';
}
